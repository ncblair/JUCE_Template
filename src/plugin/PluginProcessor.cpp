#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../matrix/Matrix.h"

#if JucePlugin_IsSynth
    #include "../audio/SynthVoice.h"
#endif

//==============================================================================
PluginProcessor::PluginProcessor()
{
    #if JucePlugin_IsSynth
        synth.clearVoices();
        for (auto i = 0; i < Matrix::NUM_VOICES; ++i) {
            synth.addVoice(new SynthVoice());
        }
        synth.setVoiceStealingEnabled(false);

        disable_mpe();
    #endif

    matrix.reset(new Matrix(this));
    samples_to_next_control_update = Matrix::CONTROL_RATE_SAMPLES;
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto out_channels = getTotalNumOutputChannels();

    // prepare synth voices
    #if JucePlugin_IsSynth
        synth.setCurrentPlaybackSampleRate(sampleRate);
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                voice->prepareToPlay(sampleRate, samplesPerBlock, out_channels, matrix.get(), i + 1);
            }
        }
    #else
        juce::ignoreUnused(sampleRate, samplesPerBlock);
    #endif
}


void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    #if JucePlugin_IsSynth
        process_synth(buffer, midiMessages);
    #endif
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = matrix->get_state();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
    
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    matrix->load_from(xmlState.get());
    #if NEEDS_CLI
        matrix->load_cli_history();
    #endif
}

void PluginProcessor::numChannelsChanged()
{
    // This event will be triggered when the number of channels in any of the
    // input/output busses changes.
    #if NEEDS_SIDECHAIN
        size_t out_channels = size_t(getTotalNumOutputChannels());
        write_pointers.resize(out_channels);
        sidechain_read_pointers.resize(out_channels);
    #endif
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

void PluginProcessor::update_parameters() {
    // get global ms_elapsed, last_release_time, etc
    #if NEEDS_MODULATORS
        #if JucePlugin_IsSynth
            double ms_elapsed = std::numeric_limits<double>::max();
            double last_release_time = std::numeric_limits<double>::max();
            bool all_released = true;
            bool any_note_on = false;
            for (int i = 0; i < synth.getNumVoices(); ++i) {
                if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                    if (voice->isActive()) {
                        any_note_on = true;
                        ms_elapsed = std::min(ms_elapsed, voice->getMsElapsed());
                        if (voice->isPlayingButReleased()) {
                            last_release_time = std::min(last_release_time, voice->getReleaseTime());
                        }
                        else {
                            all_released = false;
                        }
                    }
                }
            }
            // TODO: change this to get the most recent note and most recently released note
            if (!any_note_on) {
                main_state.set_time(0.0);
            }
            else {
                main_state.set_time(ms_elapsed);
            }
            main_state.set_release_time(last_release_time);
        #else
            //TODO: implement this for non-synth, time should be the global time
            main_state.set_time(0.0);
            main_state.set_release_time(std::numeric_limits<double>::max());
        #endif

        // updates the parameters associated with the modulators + updates with thread safe objects
        matrix->update_state(&main_state);
    #endif

    #if JucePlugin_IsSynth
        // updates the parameters associated with synth voices
        for (int i = 0; i < synth.getNumVoices(); ++i) {
            if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
                if (voice->isActive()) {
                    voice->update_parameters();
                }
            }
        }
    #endif

    // updates the global processor parameters
    // #if NEEDS_MODULATORS
    //     matrix->updateModulatorCache(&main_state);
    // #endif
    // for (size_t p_id = 0; p_id < PARAM::TOTAL_NUMBER_PARAMETERS; ++p_id) {
    //     if (!PARAMETER_AUTOMATABLE[p_id]) {
    //         params[p_id] = matrix->paramValue(p_id);
    //     }
    //     else {
    //         #if NEEDS_MODULATORS
    //             params[p_id] = matrix->modulatedParamValue(p_id, &main_state, true);
    //         #else
    //             params[p_id] = matrix->paramValue(p_id);
    //         #endif
    //     }
    // }
    params[PARAM::ENABLE_MPE] = matrix->paramValue(PARAM::ENABLE_MPE);
    update_mpe();
}

#if JucePlugin_IsSynth
    void PluginProcessor::process_synth(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
        // main loop for a synthesiser 
        juce::ScopedNoDenormals noDenormals;
        // auto totalNumInputChannels  = getTotalNumInputChannels();
        // auto output_channels = getTotalNumOutputChannels();
        auto num_samples = buffer.getNumSamples();
        auto samples_left = num_samples;

        // Process MIDI events along with Virtual Keyboard Input
        keyboard_state.processNextMidiBuffer(midiMessages, 0, num_samples, true);
        // for a synthesiser, clear the buffer
        buffer.clear();

        // split render loop into chunks of CONTROL_RATE_SAMPLES samples
        while (samples_left != 0) {
            // get number of samples to compute this loop
            auto samples_this_time = std::min(samples_left, samples_to_next_control_update);
            // render next block
            synth.renderNextBlock(buffer, midiMessages, num_samples - samples_left, samples_this_time);

            // update logic variables
            samples_to_next_control_update -= samples_this_time;
            samples_left -= samples_this_time;

            // if we have processed CONTROL_RATE_SAMPLES samples, update parameters
            if (samples_to_next_control_update == 0) {
                samples_to_next_control_update = Matrix::CONTROL_RATE_SAMPLES;
                update_parameters();
            }
        }

        midiMessages.clear();
    }

    void PluginProcessor::update_mpe() {
        if (is_mpe_enabled != bool(params[PARAM::ENABLE_MPE])) {
            if (bool(params[PARAM::ENABLE_MPE])) {
                enable_mpe();
            } 
            else {
                disable_mpe();
            }
        }
    }

    void PluginProcessor::enable_mpe() {
        is_mpe_enabled = true;
        zone_layout.clearAllZones();
        zone_layout.setLowerZone(15, 48, 2);
        synth.setZoneLayout(zone_layout);
        synth.setPitchbendTrackingMode(MPEInstrument::lastNotePlayedOnChannel);
    }

    void PluginProcessor::disable_mpe() {
        is_mpe_enabled = false;
        synth.enableLegacyMode();
        synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);
    }
#endif