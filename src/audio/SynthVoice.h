#pragma once

class PluginProcessor;

#include <JuceHeader.h>
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
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels, PluginProcessor* processor_ptr);
    void setCurrentSampleRate (double newRate) override;
    void renderNextBlock (juce::AudioBuffer< float > &outputBuffer, int startSample, int numSamples) override;
    // void renderNextBlockInternal(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples);
    bool isActive() const override;
    double getReleaseTime();
    double getMsElapsed();
    // void aftertouchChanged(int newAftertouchValue) override;
    // void channelPressureChanged(int newChannelPressureValue) override;
    // void setCurrentPlaybackSampleRate (double newRate) override;
    // bool isPlayingChannel (int midiChannel) override;
    // void update_gain_param(const float param); // i'll set this to the gain
    void update_parameters();

  private:
    // Pointer to the processor
    PluginProcessor* processor;

    //==============================================================================
    // PARAMETERS
    //==============================================================================
    std::map<juce::String, juce::SmoothedValue<float>> parameters;
    // Parameters the voice must keep track of a local copy of, separate from the global parameters
    const juce::StringArray VOICE_PARAM_NAMES {
        "LEVEL",
        "SEMITONES"
    };
    
    //Channel Management
    std::vector<float*> write_pointers;
    
    // Voice Management
    bool note_on{false};
    double ms_elapsed;
    bool is_released{true};
    double release_time{std::numeric_limits<double>::max()};

    // Voice Parameter/Control Management
    // int samples_to_next_control_update;

    // MIDI information
    float vel;
    double frequency;

    // internally, voice gain is always hard coded mapped to ADSR 1
    // This value also controls the note off logic
    juce::SmoothedValue<float> gain;
    

    double phase;
    // const int CONTROL_RATE_SAMPLES{256};
    // float gain;

    // juce::ADSR adsr;
    // juce::ADSR::Parameters adsr_params{0.1f, 1.0f, 1.0f, 0.1f};
    // juce::AudioBuffer<float> process_buffer; // for applying adsr

    //EACH VOICE NEEDS TO KNOW: FOR EACH PARAM, WHAT ARE THE MODULATORS AND WHAT ARE THE VALUES
    // ALTERNATIVELY: PARAM.GET() -> APPLIES CURRENT MODULATORS (DOWNSIDE WE ARE GETTING MODULATOR VALUES MORE OFTEN)
    // OR: AT SAMPLE TIME: FOR EACH MODULATOR, GET THE VALUE AND STORE WITHIN THE VOICE
    // FOR EACH PARAMETER, ADD ALL MODULATORS
    // COULD HAVE A PARAMETERS CLASS WITHIN VOICE. 
    // ---- HAS: GLOBAL PARAMETER VALUE FOR EACH PARAMETER, MODULATION VALUE FOR EACH VOICE FOR EACH MODULATOR, PTR TO MATRIX (ON PROCESSOR)
    // ---- IN CALLBACK, UPDATE MODULATION VALUE FIRST, THEN RUN PARAMETERS.VALUE("GAIN") FOR EXAMPLE
};
