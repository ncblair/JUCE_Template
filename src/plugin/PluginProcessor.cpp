#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../audio/SynthVoice.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : apvts(*this, nullptr, "Parameters", createParameters()), 
        matrix(PARAMETER_NAMES, &apvts, &modulators)
{
    synth.clearVoices();
    for (auto i = 0; i < NUM_VOICES; ++i) {
        synth.addVoice(new SynthVoice());
    }
    synth.setVoiceStealingEnabled(false);

    synth.enableLegacyMode();
    synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);

    // Attach Parameters to callback function
    // gain_param.attachToParameter(apvts.getParameter("GAIN_PARAM"));
    // gain_param.onParameterChanged = [&] { update_gain_param(); };
    // mpe_param.attachToParameter(apvts.getParameter("MPE_PARAM"));
    // mpe_param.onParameterChanged = [&] { update_mpe_param(); };
    // envelopes.attach(apvts);
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    // add parameters
    for (auto& param_name : PROCESSOR_PARAM_NAMES) {
        processor_parameters[param_name] = 0.0f;
    }
    for (auto& param_name : SMOOTHED_PARAM_NAMES) {
        smoothed_parameters[param_name] = juce::SmoothedValue<float>();
        smoothed_parameters[param_name].reset(sampleRate/(5*CONTROL_RATE_SAMPLES)); // smooth over 0.2 seconds
    }

    // prepare modulators
    for (auto& modulator : modulators) {
        modulator.prepareToPlay(&processor_parameters);
    }

    // prepare synth voices
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels(), this);
        }
    }
    // update_gain_param();
    // update_mpe_param();
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

    // split render loop into chunks of CONTROL_RATE_SAMPLES samples
    while (samples_left != 0) {
        // get number of samples to compute this loop
        auto samples_this_time = std::min(samples_left, samples_to_next_control_update);

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
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

// Add parameters to apvts
juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameters() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;


    //==============================================================================
    //-> ADD PARAMS
    //==============================================================================

    for (int i = 0; i < PARAMETER_NAMES.size(); ++i) {
        params.push_back(
            std::make_unique<juce::AudioParameterFloat>(
                PARAMETER_NAMES[i],  // parameter ID
                PARAMETER_NAMES[i],  // parameter name
                PARAMETER_RANGES[i],  // range
                PARAMETER_DEFAULTS[i],         // default value
                "", // parameter label (description?)
                juce::AudioProcessorParameter::Category::genericParameter,
                [this, i](float value, int maximumStringLength) { // Float to String Precision 2 Digits
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << PARAMETER_RANGES[i].convertTo0to1(value);
                    return juce::String(ss.str());
                },
                [this, i](juce::String text) {
                    return PARAMETER_RANGES[i].convertFrom0to1(text.getFloatValue()); // Convert Back to Value
                }
            )
        );
    }

    // params.push_back(std::make_unique<BoolParameter>("MPE_PARAM", "MPE Parameter", false, false));

    // //==============================================================================
    // //-> CONCATENATE PARAMS FROM EACH SUB-PROCESSOR
    // //==============================================================================

    // // get and add apvts parameters from each of the adsrs
    // auto adsr_params = envelopes.get_apvts_params();
    // for (auto& p : *adsr_params) {
    //     params.push_back(std::move(p));
    // }
    // params.insert(params.end(), adsr_params->begin(), adsr_params->end());

    return {params.begin(), params.end()};
}

// std::map<juce::String, Modulator>* PluginProcessor::createModulators() {
//     return &modulators
// }

void PluginProcessor::update_parameters() {
    // Updates the global processor parameters (doesn't worry about voice parameters)

    // first, get position of "global" envelope and update voice parameters
    float ms_elapsed = std::numeric_limits<float>::max();
    float last_release_time = std::numeric_limits<float>::max();
    bool all_released = true;
    bool any_note_on = false;
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            if (voice->isActive()) {
                voice->update_parameters();
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
    // then, if any notes are on, update global processor parameters
    if (any_note_on) {
        for (auto& [param_name, param] : smoothed_parameters) {
            // parameters that are smoothed
            param.setTargetValue(matrix.value_at(param_name, ms_elapsed, last_releast_time));
        }
        for (auto& [param_name, param] : processor_parameters) {
            // parameters that are just what the processor has
            processor_parameters[param_name] = matrix.value_at(param_name, ms_elapsed, last_releast_time);
        }
    }
}

// void PluginProcessor::update_gain_param() {
//     for (int i = 0; i < synth.getNumVoices(); ++i) {
//         if (auto voice = dynamic_cast<juce::MPESynthesiserVoice*>(synth.getVoice(i))) {
//             //update voice parameters from value tree
//             auto synthvoice = dynamic_cast<SynthVoice*>(voice);
//             synthvoice->update_gain_param(gain_param.getValue());
//         }
//     }
// }

// void PluginProcessor::update_mpe_param() {
//     if (bool(mpe_param.getValue())) {
//         zone_layout.clearAllZones();
//         zone_layout.setLowerZone(15, 48, 2);
//         synth.setZoneLayout(zone_layout);
//         synth.setPitchbendTrackingMode(MPEInstrument::lastNotePlayedOnChannel);
//     }
//     else {
//         synth.enableLegacyMode();
//         synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);
//     }
// }

