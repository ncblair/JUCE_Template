#pragma once

class Matrix;

#include <JuceHeader.h>
#include "../modulators/NoteState.h"

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
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, Matrix* m, int voice_id);
    void setCurrentSampleRate (double newRate) override;
    void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    bool isActive() const override;
    double getReleaseTime();
    double getMsElapsed();
    // void aftertouchChanged(int newAftertouchValue) override;
    // void channelPressureChanged(int newChannelPressureValue) override;
    // void setCurrentPlaybackSampleRate (double newRate) override;
    // bool isPlayingChannel (int midiChannel) override;
    void update_parameters();

  private:
    //==============================================================================
    // PARAMETERS
    //==============================================================================
    enum {
        level,
        semitones,
        NumVoiceParams
    };
    std::array<juce::SmoothedValue<float>, NumVoiceParams> params;
    // Parameters the voice must keep track of a local copy of, separate from the global parameters
    std::array<int, NumVoiceParams> param_ids;
    Matrix* matrix;

    //Channel Management
    std::vector<float*> write_pointers;
    
    // Voice Management
    bool note_on{false};

    NoteState note_state;

    // internally, voice gain is always hard coded mapped to ADSR 1
    // This value also controls the note off logic
    juce::SmoothedValue<float> gain;

    // synthesis variables
    double phase;

    // sampler variables
    std::vector<const float*> sample_read_pointers;
    double position_in_sample;
};
