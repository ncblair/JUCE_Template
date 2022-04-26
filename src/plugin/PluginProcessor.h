#pragma once

class Matrix;

#include "PluginProcessorBase.h"
#include "../audio/ADSR.h"
// #include "../util/Matrix.h"
#include "../util/Modulator.h"


//==============================================================================
class PluginProcessor  : public PluginProcessorBase
{
public:
    //==============================================================================
    PluginProcessor();
    ~PluginProcessor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

    //==============================================================================
    // Constants
    //==============================================================================
    const int NUM_VOICES{20};
    const int CONTROL_RATE_SAMPLES{64};

    //==============================================================================
    // Managers
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts; // create audio processor value tree state for parameters
    juce::MidiKeyboardState keyboard_state; // create MidiKeyboardState for MIDI Visualization and Playback
    std::unique_ptr<Matrix> matrix;
    // std::unique_ptr<juce::UndoManager> undo_manager;
    // juce::FileLogger logger;

    // ParamAttachment<float> gain_param;
    // ParamAttachment<bool> mpe_param; // We will use this bool parameter for mpe
    // void update_gain_param();
    // void update_mpe_param();

    // void update_parameters();


    //==============================================================================
    // Processor Parameters - accessed by modulators, fx, ... so they are public
    //==============================================================================
    std::map<juce::String, float> processor_parameters;
    std::map<juce::String, juce::SmoothedValue<float>> smoothed_parameters;
    //==============================================================================
    // Modulator direct access TODO: transfer this functionality to matrix
    //==============================================================================
    float modulator_value_at(juce::String modulator_name, double ms_elapsed, double release_time=std::numeric_limits<double>::max());
    

  private:
    //==============================================================================
    // State Variables
    // + info from the synthesiser for modulation
    //==============================================================================
    int samples_to_next_control_update{CONTROL_RATE_SAMPLES};
    // double ms_elapsed;
    // bool all_released{true}; // true if all voices released
    // bool any_note_on{false}; // true is any note on
    // double last_release_time;

    //==============================================================================
    // MPE
    //==============================================================================
    juce::MPESynthesiser synth;
    juce::MPEZoneLayout zone_layout;

    //==============================================================================
    // Modulators
    //==============================================================================
    std::map<juce::String, Modulator*> modulators;
    void createModulators();

    //==============================================================================
    // Parameters
    //==============================================================================
    juce::StringArray getParamNames();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void update_parameters();

    // Parameters updated on the processor and smoothed at control rate
    juce::StringArray smoothed_param_names;

    // Parameter that will be updated on the processor without being smoothed
    juce::StringArray processor_param_names = juce::StringArray(
        "ADSR_1_ATTACK",
        "ADSR_1_DECAY",
        "ADSR_1_SUSTAIN",
        "ADSR_1_RELEASE",
        "ADSR_2_ATTACK",
        "ADSR_2_DECAY",
        "ADSR_2_SUSTAIN",
        "ADSR_2_RELEASE",
        "ADSR_3_ATTACK",
        "ADSR_3_DECAY",
        "ADSR_3_SUSTAIN",
        "ADSR_3_RELEASE"
    );
    // // std::cout << "processorParamNames " << std::endl;

    // // APVTS PARAMETERS
    // juce::StringArray parameter_names = juce::StringArray(
    //     "LEVEL",
    //     "SEMITONES",
    //     "ADSR_1_ATTACK",
    //     "ADSR_1_DECAY",
    //     "ADSR_1_SUSTAIN",
    //     "ADSR_1_RELEASE",
    //     "ADSR_2_ATTACK",
    //     "ADSR_2_DECAY",
    //     "ADSR_2_SUSTAIN",
    //     "ADSR_2_RELEASE",
    //     "ADSR_3_ATTACK",
    //     "ADSR_3_DECAY",
    //     "ADSR_3_SUSTAIN",
    //     "ADSR_3_RELEASE"
    // );

    // // std::cout << "ParamNames " << std::endl;

    // //Parameter Ranges, low, high, grain, exp
    // juce::Array<juce::NormalisableRange<float>> parameter_ranges = juce::Array<juce::NormalisableRange<float>> (
    //     // LEVEL
    //     juce::NormalisableRange<float>(-60.0f, 6.0f, 0.0f, 1.0f),
    //     // SEMITONES
    //     juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f, 1.0f),
    //     // ADSR_1_ATTACK
    //     juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.75f),
    //     // ADSR_1_DECAY
    //     juce::NormalisableRange<float> (0.001f, 60.0f, 0.0f, 0.3f),
    //     // ADSR_1_SUSTAIN
    //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    //     // ADSR_1_RELEASE
    //     juce::NormalisableRange<float> (0.001f, 10.0f, 0.0f, 0.75f),
    //     // ADSR_2_ATTACK
    //     juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.75f),
    //     // ADSR_2_DECAY
    //     juce::NormalisableRange<float> (0.001f, 60.0f, 0.0f, 0.3f),
    //     // ADSR_2_SUSTAIN
    //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    //     // ADSR_2_RELEASE
    //     juce::NormalisableRange<float> (0.001f, 10.0f, 0.0f, 0.75f),
    //     // ADSR_3_ATTACK
    //     juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.75f),
    //     // ADSR_3_DECAY
    //     juce::NormalisableRange<float> (0.001f, 60.0f, 0.0f, 0.3f),
    //     // ADSR_3_SUSTAIN
    //     juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    //     // ADSR_3_RELEASE
    //     juce::NormalisableRange<float> (0.001f, 10.0f, 0.0f, 0.75f)
    // );

    // // std::cout << "ParamRanges " << std::endl;

    // juce::Array<float> parameter_defaults = juce::Array<float> (
    //     // LEVEL
    //     -12.0f,
    //     // SEMITONES
    //     0.0f,
    //     // ADSR_1_ATTACK
    //     0.01f,
    //     // ADSR_1_DECAY
    //     1.0f,
    //     // ADSR_1_SUSTAIN
    //     1.0f,
    //     // ADSR_1_RELEASE
    //     1.0f,
    //     // ADSR_2_ATTACK
    //     0.01f,
    //     // ADSR_2_DECAY
    //     1.0f,
    //     // ADSR_2_SUSTAIN
    //     1.0f,
    //     // ADSR_2_RELEASE
    //     1.0f,
    //     // ADSR_3_ATTACK
    //     0.01f,
    //     // ADSR_3_DECAY
    //     1.0f,
    //     // ADSR_3_SUSTAIN
    //     1.0f,
    //     // ADSR_3_RELEASE
    //     1.0f
    // );


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};