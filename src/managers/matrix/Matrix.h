#pragma once

class Modulator;
class PluginProcessor;

#include <JuceHeader.h>
#include "ParameterDefines.h"
#include "ModulatorDefines.h"

struct ModulatorInfo {
    float depth{0.0f};
    int mod_id;
};

class Matrix {
  public:
    Matrix(PluginProcessor* proc);
    ~Matrix();
    float paramValue(int param_id);
    juce::NormalisableRange<float> paramRange(int param_id);
    float modulatorValue(int mod_id, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    float modulatedParamValue(int param_id, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    juce::AudioProcessorValueTreeState* getValueTree();
    void connect(int mod_id, int param_id, float depth);
    void disconnect(int mod_id, int param_id);
    void update_modulator_parameters(double ms_elapsed, double last_release_time);
    
    // constants
    const float DEFAULT_MOD_DEPTH{0.3};
  private:
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts_ptr;
    PluginProcessor* processor;
    std::array<std::vector<ModulatorInfo>, TotalNumberParameters> matrix;
    std::array<std::unique_ptr<Modulator>, TotalNumberModulators> modulators;
};