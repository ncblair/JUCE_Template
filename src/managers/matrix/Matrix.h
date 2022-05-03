#pragma once

class Modulator;
class PluginProcessor;
class DummyProcessor;

#include <JuceHeader.h>
#include "ParameterDefines.h"
#include "ModulatorDefines.h"
#include "PropertyDefines.h"

// struct ModulatorInfo {
//     float depth{0.0f};
//     int mod_id;
// };

class Matrix {
  public:
    Matrix(PluginProcessor* proc);
    ~Matrix();
    float paramValue(int param_id);
    juce::NormalisableRange<float> paramRange(int param_id);
    float modulatorValue(int mod_id, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    float modulatedParamValue(int param_id, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    juce::AudioProcessorValueTreeState* getParamTree();
    juce::AudioProcessorValueTreeState* getPropertyTree();
    void connect(int mod_id, int param_id, float depth);
    void disconnect(int mod_id, int param_id);
    void update_modulator_parameters(double ms_elapsed, double last_release_time);

    float propertyValue(int property_id);
    juce::NormalisableRange<float> propertyRange(int property_id);
    juce::UndoManager* getUndoManager();
    
    // constants
    const float DEFAULT_MOD_DEPTH{0.3};
  private:
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts_ptr;
    // PluginProcessor* processor;
    // std::array<std::vector<ModulatorInfo>, TotalNumberParameters> matrix;
    juce::ValueTree matrix;
    std::array<std::unique_ptr<Modulator>, TotalNumberModulators> modulators;

    // properties (non-automatable)
    std::unique_ptr<juce::AudioProcessorValueTreeState> property_tree_ptr;
    std::unique_ptr<DummyProcessor> plugin_processor_dummy;
    // juce::ValueTree property_tree;
    // const juce::Identifier PROPERTY_TREE_IDENTIFIER{"PROPERTIES"};

    // Undo Manager
    juce::UndoManager undo_manager;
};