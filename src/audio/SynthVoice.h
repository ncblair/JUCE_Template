#pragma once

#include <JuceHeader.h>

class SynthVoice : public juce::MPESynthesiserVoice {
  // For the template i'll do a simple Sine Voice
  public: 
    void noteStarted() override;
    void noteStopped(bool allowTailOff) override;
    void notePitchbendChanged() override;
    void notePressureChanged() override;
    void noteTimbreChanged() override;
    void noteKeyStateChanged() override;
    // void controllerMoved(int controllerNumber, int newControllerValue) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void setCurrentSampleRate (double newRate) override;
    void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    bool isActive() const override;
    // void aftertouchChanged(int newAftertouchValue) override;
    // void channelPressureChanged(int newChannelPressureValue) override;
    // void setCurrentPlaybackSampleRate (double newRate) override;
    // bool isPlayingChannel (int midiChannel) override;
    void update_gain_param(const float param); // i'll set this to the gain

  private:
    float vel;
    double frequency;

    double phase;
    bool note_on{false};

    juce::SmoothedValue<float> gain;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsr_params{0.1f, 1.0f, 1.0f, 0.1f};
    juce::AudioBuffer<float> process_buffer; // for applying adsr
};
