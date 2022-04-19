#pragma once

#include <JuceHeader.h>
#include "../plugin/PluginProcessor.h"
#include "../util/ParamAttachment.h"

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
    // void update_gain_param(const float param); // i'll set this to the gain

  private:
    AudioPluginAudioProcessor* processor;
    double ms_elapsed;

    float vel;
    double frequency;

    double phase;
    bool note_on{false};

    // juce::SmoothedValue<float> gain;
    ModulatedFloatParam gain;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsr_params{0.1f, 1.0f, 1.0f, 0.1f};
    juce::AudioBuffer<float> process_buffer; // for applying adsr

    //EACH VOICE NEEDS TO KNOW: FOR EACH PARAM, WHAT ARE THE MODULATORS AND WHAT ARE THE VALUES
    // ALTERNATIVELY: PARAM.GET() -> APPLIES CURRENT MODULATORS (DOWNSIDE WE ARE GETTING MODULATOR VALUES MORE OFTEN)
    // OR: AT SAMPLE TIME: FOR EACH MODULATOR, GET THE VALUE AND STORE WITHIN THE VOICE
    // FOR EACH PARAMETER, ADD ALL MODULATORS
    // COULD HAVE A PARAMETERS CLASS WITHIN VOICE. 
    // ---- HAS: GLOBAL PARAMETER VALUE FOR EACH PARAMETER, MODULATION VALUE FOR EACH VOICE FOR EACH MODULATOR, PTR TO MATRIX (ON PROCESSOR)
    // ---- IN CALLBACK, UPDATE MODULATION VALUE FIRST, THEN RUN PARAMETERS.VALUE("GAIN") FOR EXAMPLE
};
