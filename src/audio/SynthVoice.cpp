#include "SynthVoice.h"
#include "../plugin/PluginProcessor.h"
#include "../util/Matrix.h"

void SynthVoice::noteStarted() {
    update_parameters();

    vel = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    vel = juce::Decibels::decibelsToGain(vel * 18.0f - 18.0f); //18db velocity range default
    note_on = true;
    is_released = false;
    release_time = std::numeric_limits<double>::max();
    phase = 0.0;
    ms_elapsed = 0.0;
    // samples_to_next_control_update = processor->CONTROL_RATE_SAMPLES;

    notePitchbendChanged(); // used to set frequency
    std::cout << "NOTE ON: " << frequency << "hz" << std::endl;

    // adsr.reset();
    // adsr.noteOn();
}

void SynthVoice::noteStopped(bool allowTailOff) {
    is_released = true;
    release_time = ms_elapsed;
    std::cout << "NOTE OFF: " << frequency << "hz" << std::endl;
    juce::ignoreUnused (allowTailOff);
}

bool SynthVoice::isActive() const{
    return note_on;
}

double SynthVoice::getReleaseTime() {
    return release_time;
}

double SynthVoice::getMsElapsed() {
    return ms_elapsed;
}

void SynthVoice::notePitchbendChanged() {
    frequency = currentlyPlayingNote.getFrequencyInHertz();
}


void SynthVoice::notePressureChanged() {

}
void SynthVoice::noteTimbreChanged() {
    
}
void SynthVoice::noteKeyStateChanged() {
    
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, PluginProcessor* processor_ptr) {
    // adsr.setSampleRate(sampleRate);
    // adsr.setParameters(adsr_params);

    // process_buffer.setSize(1, samplesPerBlock * 2); // 2x for safety

    // add parameters
    for (auto& param_name : VOICE_PARAM_NAMES) {
        parameters[param_name] = juce::SmoothedValue<float>();
        parameters[param_name].reset(sampleRate, 0.001);
    }

    gain.reset(sampleRate, 0.001);
    processor = processor_ptr;

    write_pointers.resize(outputChannels);

    // juce::ignoreUnused (outputChannels);
}

void SynthVoice::setCurrentSampleRate (double newRate) {
    if (currentSampleRate != newRate)
    {
        currentSampleRate = newRate;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    // Every CONTROL_RATE_SAMPLES samples, update parameters for the voice

    if (isActive()) {
        // SET UP WRITE POINTERS FOR EACH CHANNEL
        auto num_channels = write_pointers.size();
        for (int c = 0; c < num_channels; ++c) {
            write_pointers[c] = outputBuffer.getWritePointer(c);
        }
        // ITERATE THROUGH SAMPLES
        for (int i = startSample; i < startSample + numSamples; ++i) {
            // STEP PARAMETERS
            for (const auto& [param_name, param] : parameters) {
                param.getNextValue();
            }
            float amplitude = gain.getNextValue();

            // DO LOGIC HERE FOR ALL CHANNELS
            float level_gain = juce::Decibels::decibelsToGain(parameters["LEVEL"].getCurrentValue());
            float sample = juce::dsp::FastMathApproximations::sin(2.0*M_PI*phase);
            sample *= amplitude*level_gain*vel;
            // std::cout << "amp " << amplitude << std::endl;
            // std::cout << "vel " << vel << std::endl;
            // std::cout << "level " << parameters["LEVEL"].getCurrentValue() << std::endl;
            // std::cout << "level_gain " << level_gain << std::endl;
            // std::cout << amplitude*level_gain*vel << std::endl;
            // ITERATE OVER CHANNELS
            for (int c = 0; c < num_channels; ++c) {
                // WRITE NEXT SAMPLE
                write_pointers[c][i] += sample;
            }
            // UPDATE STATE
            phase += frequency / currentSampleRate;
            while (phase > 0.5) { // phase in (-0.5, 0.5]
                phase -= 1.0;
            }
        }
        // advance time
        ms_elapsed += double(numSamples)*1000.0 / currentSampleRate;

        if (is_released && gain.getCurrentValue() == 0.0f) {
            // NOTE_OFF
            note_on = false;
        }
    }
}

// void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
//     // Every CONTROL_RATE_SAMPLES samples, update parameters for the voice

//     if (isActive()) {
//         auto samples_left = numSamples;
//         // split render loop into chunks of CONTROL_RATE_SAMPLES samples
//         while (samples_left != 0) {
//             auto samples_this_time = std::min(samples_left, samples_to_next_control_update);
//             // render next block
//             renderNextBlockInternal(outputBuffer, numSamples - samples_left, samples_this_time);

//             // update logic variables
//             samples_to_next_control_update -= samples_this_time;
//             samples_left -= samples_this_time;
//             ms_elapsed += samples_this_time*1000./currentSampleRate;

//             // if we have processed CONTROL_RATE_SAMPLES samples, update parameters
//             if (samples_to_next_control_update == 0) {
//                 samples_to_next_control_update = processor->CONTROL_RATE_SAMPLES;
//                 update_parameters();
//             }
//         }
//     }
// }

// void SynthVoice::renderNextBlockInternal(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
//     /*
//     -> Parameters have been updated, voice is active. this is our DSP block. 
//     */
//     auto write_pointer = outputBuffer.getWritePointer(0);

//     for (int i = startSample; i < startSample + numSamples; ++i) {
//         // GET NEXT SAMPLE / UPDATE STEP

//         // write result to output buffer
//         // highpass to remove DC offset
//         write_pointer[i] = std::sin(2.0*M_PI*phase)*parameters["LEVEL"].getNextValue();

//         phase += frequency / currentSampleRate;
//     }
// }

void SynthVoice::update_parameters() {
    for (auto& [param_name, param] : parameters) {
        param.setTargetValue(processor->matrix->value_at(param_name, ms_elapsed, release_time));
    }
    // gain is taken directly from env 1 value, not an apvts parameter
    gain.setTargetValue(processor->modulator_value_at("ADSR_1", ms_elapsed, release_time));
}

// void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) {
//     if (isActive()) {
//         if (numSamples > process_buffer.getNumSamples()) {
//             // THIS SHOULDN'T HAPPEN, ALLOCATES ON AUDIO BUFFER IN WORST CASE
//             process_buffer.setSize(1, numSamples * 2);
//             std::cout << "WARNING: Allocate on Audio Buffer in SynthVoice::renderNextBlock" << std::endl;
//         }

//         process_buffer.clear();
//         auto write_pointer = process_buffer.getWritePointer(0);

//         for (int i = 0; i < numSamples; ++i) {
//             // GET NEXT SAMPLE / UPDATE STEP

//             // write result to output buffer
//             // highpass to remove DC offset
//             write_pointer[i] = std::sin(2.0*M_PI*phase)*gain.getNextValue();

//             phase += frequency / currentSampleRate;
//             ms_elapsed += 1000./currentSampleRate;
//         }
        
//         // apply envelope
//         adsr.applyEnvelopeToBuffer(process_buffer, 0, numSamples);

//         // add to output buffer
//         outputBuffer.addFrom(0, startSample, process_buffer, 0, 0, numSamples, vel);

//         if (!adsr.isActive()) {
//             // TURN VOICE OFF
//             note_on = false;
//             clearCurrentNote();
//          }
//     }
// }

// void SynthVoice::update_gain_param(const float param) {
//     auto amplitude = juce::Decibels::decibelsToGain(param);
//     gain.setTargetValue(amplitude);
// }