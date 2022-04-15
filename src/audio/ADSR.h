#pragma once

#include <JuceHeader.h>
#include "../util/ParamAttachment.h"

class ADSRParent {
  public:
    ADSRParent(int number_envelopes);
    void attach(juce::AudioProcessorValueTreeState apvts);
  private:
    int num_envelopes;
    std::vector<std::unique_ptr<ADSR>> envelopes;
};

class ADSR {
  // For the template i'll do a simple Sine Voice
  public: 
    void set(float a, float d, float s, float r);
    void get(float ms, is_released=false);
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> get_apvts_params(int index);
    void attach(uce::AudioProcessorValueTreeState apvts, int index);

  private:
    // ms
    // float attack;
    // float decay;
    // float sustain;
    // float release;
    float cur;
    ParamAttachment<float> attack;
    ParamAttachment<float> decay;
    ParamAttachment<float> sustain;
    ParamAttachment<float> release;
};
