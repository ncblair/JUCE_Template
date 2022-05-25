#include "SynthVoice.h"
// #include "../plugin/PluginProcessor.h"
#include "../managers/matrix/Matrix.h"
#include "../modulators/NoteState.h"

void SynthVoice::noteStarted() {
    update_parameters();
    note_on = true;

    auto vel = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    note_state.set_velocity(juce::Decibels::decibelsToGain(vel * 18.0f - 18.0f)); //18db velocity range default
    note_state.mark_unreleased();
    note_state.set_time(0.0);
    // is_released = false;
    // release_time = std::numeric_limits<double>::max();
    phase = 0.0;
    position_in_sample = 0.0;
    // ms_elapsed = 0.0;

    notePitchbendChanged(); // used to set frequency

    gain.setCurrentAndTargetValue(0.0f);
    // std::cout << "NOTE ON: " << frequency << "hz" << std::endl;
}

void SynthVoice::noteStopped(bool allowTailOff) {
    if (!note_state.is_released()) {
        // is_released = true;
        // release_time = ms_elapsed;
        note_state.release();
        // std::cout << "NOTE OFF: " << frequency << "hz" << std::endl;
    }
    juce::ignoreUnused (allowTailOff);
}

bool SynthVoice::isActive() const{
    return note_on;
}

double SynthVoice::getReleaseTime() {
    return note_state.get_release_time();
}

double SynthVoice::getMsElapsed() {
    return note_state.get_time();
}

void SynthVoice::notePitchbendChanged() {
    note_state.set_frequency(currentlyPlayingNote.getFrequencyInHertz());
    // frequency = currentlyPlayingNote.getFrequencyInHertz();
}


void SynthVoice::notePressureChanged() {

}
void SynthVoice::noteTimbreChanged() {
    
}
void SynthVoice::noteKeyStateChanged() {
    
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, Matrix* m) {

    // set up parameters
    matrix = m;

    param_ids = {LEVEL, SEMITONES}; 
    for (auto& param : params) {
        param.reset(sampleRate, 0.001);
    }

    gain.reset(sampleRate, 0.001);

    write_pointers.resize(outputChannels);
    sample_read_pointers.resize(outputChannels);
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

        
        // HERE, I LOAD THE AUDIO BUFFER FOR THE SAMPLES
        auto sample_buffer = matrix->get_audio_buffer();

        // SET UP WRITE POINTERS FOR EACH CHANNEL
        auto num_channels = write_pointers.size();
        for (int c = 0; c < num_channels; ++c) {
            write_pointers[c] = outputBuffer.getWritePointer(c);
            sample_read_pointers[c] = sample_buffer->getReadPointer(juce::jmin(c, sample_buffer->getNumChannels() - 1));
        }

        // auto sample_pos = juce::jmin(int(ms_elapsed * currentSampleRate / 1000.0f), sample_buffer->getNumSamples() - 1);
        // auto num_samples_from_buffer = juce::jmin(numSamples, sample_buffer->getNumSamples() - sample_pos);
        // for (int c = 0; c < num_channels; ++c) {
        //     outputBuffer.addFrom(c, startSample, *sample_buffer, juce::jmin(c, sample_buffer->getNumChannels() - 1), sample_pos, num_samples_from_buffer, 1.0f); // 1.0f is gain
        // }

        // ITERATE THROUGH SAMPLES
        for (int i = startSample; i < startSample + numSamples; ++i) {
            // STEP PARAMETERS
            for (auto& param : params) {
                param.getNextValue();
            }
            float amplitude = gain.getNextValue();

            // DO LOGIC HERE FOR ALL CHANNELS
            float level_gain = juce::Decibels::decibelsToGain(params[level].getCurrentValue());
            float sample = juce::dsp::FastMathApproximations::sin(2.0*M_PI*phase);
            
            // ITERATE OVER CHANNELS
            for (int c = 0; c < num_channels; ++c) {
                // WRITE NEXT SAMPLE
                auto mix = sample;
                if (position_in_sample < sample_buffer->getNumSamples()) {
                    mix += sample_read_pointers[c][int(position_in_sample)];
                }
                write_pointers[c][i] += mix*amplitude*level_gain*note_state.get_velocity();
            }
            // UPDATE STATE
            phase += note_state.get_frequency() * std::pow(2.0f, params[semitones].getCurrentValue()/12.0f) / currentSampleRate;
            while (phase > 0.5) { // phase in (-0.5, 0.5]
                phase -= 1.0;
            }
            position_in_sample += 1.0;
        }

        // advance time
        note_state.increment_time(numSamples, currentSampleRate);

        if (note_state.is_released() && gain.getCurrentValue() == 0.0f) {
            // NOTE_OFF
            note_on = false;
        }
    }
}

void SynthVoice::update_parameters() {
    for (int p_id = 0; p_id < NumVoiceParams; ++p_id) {
        params[p_id].setTargetValue(matrix->modulatedParamValue(param_ids[p_id], note_state));
    }
    // gain is taken directly from env 1 value, not an apvts parameter
    gain.setTargetValue(matrix->modulatorValue(ADSR_1, note_state));
    // auto adsr_lvl = matrix->modulatorValue(ADSR_1, ms_elapsed, release_time);
    // if (adsr_lvl == 0.0f) {
    //     gain.setTargetValue(0.0f);
    // }
    // else {
    //     gain.setTargetValue(juce::Decibels::decibelsToGain(60.0f*adsr_lvl - 60.0f));
    // }
    
}