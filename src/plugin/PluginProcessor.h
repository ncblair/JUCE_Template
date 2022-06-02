#pragma once

class Matrix;
class PropertyManager;
class NoteState;

#include "PluginProcessorBase.h"
#include "../modulators/NoteState.h"
// #include "../audio/ADSR.h"

// #include "../util/Matrix.h"
// #include "../util/Modulator.h"


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

    //==============================================================================
    // Managers
    //==============================================================================
    juce::MidiKeyboardState keyboard_state; // create MidiKeyboardState for MIDI Visualization and Playback
    std::unique_ptr<Matrix> matrix;
    // std::unique_ptr<PropertyManager> property_manager;
    // juce::FileLogger logger;

  private:
    //==============================================================================
    // State Variables
    // + info from the synthesiser for modulation
    //==============================================================================
    NoteState main_state;
    int samples_to_next_control_update;

    //==============================================================================
    // MPE
    //==============================================================================
    juce::MPESynthesiser synth;
    juce::MPEZoneLayout zone_layout;
    bool is_mpe_enabled{false};

    //==============================================================================
    // Parameters
    //==============================================================================
    void update_parameters();
    void update_MPE_enable();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};