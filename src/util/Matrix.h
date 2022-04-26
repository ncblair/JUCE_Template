#pragma once

class Modulator;

#include <JuceHeader.h>

// class ParamWithModList {
//   public:
//     // ParamAttachment* param;
//     std::map<juce::String, ModulatorInfo> modulators;

//     int& ParamWithModList::operator[](const int& index) {
//         return modulators[index];
//     }
//     int num_modulators() {
//         return modulators.size();
//     }
// };

struct ModulatorInfo {
    bool is_active{false};
    float depth{0.0f};
    Modulator* mod_ptr{nullptr};
};



class Matrix {
  public:
    Matrix(const juce::StringArray& param_names, juce::AudioProcessorValueTreeState* apvts_, std::map<juce::String, Modulator*>* modulators);

    float value_at(juce::String param_name, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    // Modulator* get_modulator(juce::String modulator_name) {
    //     return mods[modulator_name];
    // }

    // void connect(juce::String param_name, juce::String mod_name)
    // void setDepth(juce::String param_name, juce::String mod_name, float depth)

    // std::vector<ModulatorInfo> find(juce::String param_name) {
    //     return matrix.find(param_name);
    // }

  private:
    std::map<juce::String, std::map<juce::String, ModulatorInfo>> matrix;
    // std::map<juce::String, Modulator*> mods;
    juce::AudioProcessorValueTreeState* apvts;
};