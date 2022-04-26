#pragma once

#include <JuceHeader.h>
// #include "../util/ParamAttachment.h"
#include "../util/Modulator.h"
// #include "../util/Matrix.h"

// //=====================================================================
// // Parent Class for creating Multiple ADSRModulator Objects
// // --- Compare with ADSRParentComponent, which houses multiple ADSR objects in the UI
// //=====================================================================
// class ADSRParent {
//   public:
//     ADSRParent(int number_envelopes);
//     // void attach(juce::AudioProcessorValueTreeState* apvts);
//     // std::vector<std::unique_ptr<juce::RangedAudioParameter>>* get_apvts_params();
//     std::vector<Modulator*> getChildren();
//   private:
//     int num_envelopes;
//     std::vector<std::unique_ptr<ADSR>> envelopes;
//     // std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
// };


//=====================================================================
// ADSRModulator is a type of modulator for ADSR envelopes
//=====================================================================
class ADSRModulator : public Modulator{
  public: 
    ADSRModulator(juce::String attack, juce::String decay, juce::String sustain, juce::String release);
    void prepareToPlay(PluginProcessor* processor) override;
    // void set(float a, float d, float s, float r);
    // void set(Matrix* m);
    float get(float ms, float release_time=std::numeric_limits<float>::max()) override;
    // std::vector<std::unique_ptr<juce::RangedAudioParameter>>* get_apvts_params(int index);
    // void attach(juce::AudioProcessorValueTreeState apvts, int index);

  private:
    // ms
    // float attack;
    // float decay;
    // float sustain;
    // float release;
    // float cur;
    // juce::String modulator_name;
    juce::String atk_str;
    juce::String dec_str;
    juce::String sus_str;
    juce::String rel_str;
    std::map<juce::String, float>* params;

    // float attack{0.01f};
    // float decay{1.0f};
    // float sustain{1.0f};
    // float release{1.0f};
    // ParamAttachment<float> attack;
    // ParamAttachment<float> decay;
    // ParamAttachment<float> sustain;
    // ParamAttachment<float> release;
    // std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
};
