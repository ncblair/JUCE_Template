#include "SynthVoice.h"

void SynthVoice::noteStarted() {
    vel = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    vel = juce::Decibels::decibelsToGain(vel * 18.0f - 18.0f); //18db velocity range default
    note_on = true;
    phase = 0.0;

    notePitchbendChanged(); // used to set frequency

    adsr.reset();
    adsr.noteOn();
}

void SynthVoice::noteStopped(bool allowTailOff) {
    adsr.noteOff();
    juce::ignoreUnused (allowTailOff);
}

bool SynthVoice::isActive() const{
    return note_on;
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

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) {
    adsr.setSampleRate(sampleRate);
    adsr.setParameters(adsr_params);

    process_buffer.setSize(1, samplesPerBlock * 2); // 2x for safety

    gain.reset(sampleRate, 0.001);

    juce::ignoreUnused (outputChannels);
}

void SynthVoice::setCurrentSampleRate (double newRate) {
    if (currentSampleRate != newRate)
    {
        currentSampleRate = newRate;
    }
}

void SynthVoice::renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) {
    if (isActive()) {
        if (numSamples > process_buffer.getNumSamples()) {
            // THIS SHOULDN'T HAPPEN, ALLOCATES ON AUDIO BUFFER IN WORST CASE
            process_buffer.setSize(1, numSamples * 2);
            std::cout << "WARNING: Allocate on Audio Buffer in SynthVoice::renderNextBlock" << std::endl;
        }

        process_buffer.clear();
        auto write_pointer = process_buffer.getWritePointer(0);

        for (int i = 0; i < numSamples; ++i) {
            // GET NEXT SAMPLE / UPDATE STEP

            // write result to output buffer
            // highpass to remove DC offset
            write_pointer[i] = std::sin(2.0*M_PI*phase)*gain.getNextValue();

            phase += frequency / currentSampleRate;
        }
        
        // apply envelope
        adsr.applyEnvelopeToBuffer(process_buffer, 0, numSamples);

        // add to output buffer
        outputBuffer.addFrom(0, startSample, process_buffer, 0, 0, numSamples, vel);

        if (!adsr.isActive()) {
            // TURN VOICE OFF
            note_on = false;
            clearCurrentNote();
         }
    }
}

void SynthVoice::update_gain_param(const float param) {
    auto amplitude = juce::Decibels::decibelsToGain(param);
    gain.setTargetValue(amplitude);
}