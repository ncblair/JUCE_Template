#pragma once

class Matrix;
// class NoteState;

#include <JuceHeader.h>
#include "../modulators/NoteState.h"
// #include "../util/ParamAttachment.h"

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
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, Matrix* m);
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
    // double ms_elapsed;
    // bool is_released{true};
    // double release_time{std::numeric_limits<double>::max()};

    NoteState note_state;

    // MIDI information
    // float vel;
    // double frequency;

    // internally, voice gain is always hard coded mapped to ADSR 1
    // This value also controls the note off logic
    juce::SmoothedValue<float> gain;

    // synthesis variables
    double phase;

    // sampler variables
    std::vector<const float*> sample_read_pointers;
    double position_in_sample;
    // juce::AudioBuffer<float>* audio_buffer_1;    
};
