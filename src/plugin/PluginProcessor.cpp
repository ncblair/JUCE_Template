#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../audio/SynthVoice.h"
#include "../managers/matrix/Matrix.h"
#include "../modulators/NoteState.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    //  : apvts(*this, nullptr, "Parameters", createParameters())
        // matrix(parameter_names, &apvts, &modulators)
{
    std::cout << "Processor Constructor " << NUM_VOICES << std::endl;
    synth.clearVoices();
    for (auto i = 0; i < NUM_VOICES; ++i) {
        synth.addVoice(new SynthVoice());
    }
    synth.setVoiceStealingEnabled(false);

    synth.enableLegacyMode();
    synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);

    matrix.reset(new Matrix(this));
    // property_manager.reset(new PropertyManager(this));
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // prepare synth voices
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels(), matrix.get());
        }
    }
}


void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();
    auto samples_left = numSamples;

    // Process MIDI events along with Virtual Keyboard Input
    keyboard_state.processNextMidiBuffer(midiMessages, 0, numSamples, true);
    // for a synthesiser, clear the buffer
    buffer.clear();

    // synth.renderNextBlock(buffer, midiMessages, 0, numSamples);
    // update_parameters();
    // split render loop into chunks of CONTROL_RATE_SAMPLES samples
    while (samples_left != 0) {
        // get number of samples to compute this loop
        auto samples_this_time = std::min(samples_left, samples_to_next_control_update);
        // std::cout << "SAMPS THIS TIME: " << samples_this_time << std::endl;
        // std::cout << "SAMP LEFT: " << samples_left << std::endl;
        // render next block
        synth.renderNextBlock(buffer, midiMessages, numSamples - samples_left, samples_this_time);

        // update logic variables
        samples_to_next_control_update -= samples_this_time;
        samples_left -= samples_this_time;

        // if we have processed CONTROL_RATE_SAMPLES samples, update parameters
        if (samples_to_next_control_update == 0) {
            samples_to_next_control_update = CONTROL_RATE_SAMPLES;
            update_parameters();
        }
    }

    // // copy to fill up all channels with same info
    // auto read_pointer = buffer.getReadPointer(0, 0);
    // for (auto c = 1; c < totalNumOutputChannels; ++c) {
    //     buffer.copyFrom(c, 0, read_pointer, buffer.getNumSamples());
    // }
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

    // // add binary data (audio files)
    // auto size          = ((uint32_t *)destData.getData())[1];
    // auto out_stream = juce::MemoryOutputStream(destData, true);
    // out_stream.setPosition(size + 9);
    // matrix->write_audio_to_binary(out_stream);
    
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    matrix->load_from(xmlState.get());
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

void PluginProcessor::update_parameters() {
    auto main_state = NoteState();

    // get global ms_elapsed, last_release_time, etc
    double ms_elapsed = std::numeric_limits<double>::max();
    double last_release_time = std::numeric_limits<double>::max();
    bool all_released = true;
    // bool any_note_on = false;
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            if (voice->isActive()) {
                // any_note_on = true;
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

    main_state.set_time(ms_elapsed);
    main_state.set_release_time(last_release_time);

    // updates the parameters associated with the modulators
    matrix->update_modulator_parameters(main_state);

    // update the matrix's audio buffers in a thread safe way
    matrix->update_audio_buffers();
    
    // updates the parameters associated with active voices
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            if (voice->isActive()) {
                voice->update_parameters();
            }
        }
    }

    // updates the global processor parameters
    update_MPE_enable();
}

void PluginProcessor::update_MPE_enable() {
    if (is_mpe_enabled != bool(matrix->propertyValue(ENABLE_MPE))) {
        is_mpe_enabled = bool(matrix->propertyValue(ENABLE_MPE));
        if (is_mpe_enabled) {
            zone_layout.clearAllZones();
            zone_layout.setLowerZone(15, 48, 2);
            synth.setZoneLayout(zone_layout);
            synth.setPitchbendTrackingMode(MPEInstrument::lastNotePlayedOnChannel);
        }
        else {
            synth.enableLegacyMode();
            synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);
        }
    }
}