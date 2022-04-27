#include "SynthVoice.h"
// #include "../plugin/PluginProcessor.h"
#include "../managers/matrix/Matrix.h"

void SynthVoice::noteStarted() {
    update_parameters();

    vel = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    vel = juce::Decibels::decibelsToGain(vel * 18.0f - 18.0f); //18db velocity range default
    note_on = true;
    is_released = false;
    release_time = std::numeric_limits<double>::max();
    phase = 0.0;
    ms_elapsed = 0.0;

    notePitchbendChanged(); // used to set frequency

    gain.setCurrentAndTargetValue(0.0f);
    std::cout << "NOTE ON: " << frequency << "hz" << std::endl;
}

void SynthVoice::noteStopped(bool allowTailOff) {
    if (!is_released) {
        is_released = true;
        release_time = ms_elapsed;
        std::cout << "NOTE OFF: " << frequency << "hz" << std::endl;
    }
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

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, Matrix* m) {

    // set up parameters
    matrix = m;
    param_ids = {LEVEL, SEMITONES}; 
    for (auto& param : params) {
        param.reset(sampleRate, 0.001);
    }

    gain.reset(sampleRate, 0.001);

    write_pointers.resize(outputChannels);
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
            for (auto& param : params) {
                param.getNextValue();
            }
            float amplitude = gain.getNextValue();

            // DO LOGIC HERE FOR ALL CHANNELS
            float level_gain = juce::Decibels::decibelsToGain(params[level].getCurrentValue());
            float sample = juce::dsp::FastMathApproximations::sin(2.0*M_PI*phase);
            sample *= amplitude*level_gain*vel;
            
            // ITERATE OVER CHANNELS
            for (int c = 0; c < num_channels; ++c) {
                // WRITE NEXT SAMPLE
                write_pointers[c][i] += sample;
            }
            // UPDATE STATE
            phase += frequency * std::pow(2.0f, params[semitones].getCurrentValue()/12.0f) / currentSampleRate;
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

void SynthVoice::update_parameters() {
    for (int p_id = 0; p_id < NumVoiceParams; ++p_id) {
        params[p_id].setTargetValue(matrix->modulatedParamValue(param_ids[p_id], ms_elapsed, release_time));
    }
    // gain is taken directly from env 1 value, not an apvts parameter
    gain.setTargetValue(matrix->modulatorValue(ADSR_1, ms_elapsed, release_time));
}