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
    juce::MidiKeyboardState keyboard_state; // create MidiKeyboardState for MIDI Visualization and Playback
    std::unique_ptr<Matrix> matrix;
    // std::unique_ptr<juce::UndoManager> undo_manager;
    // juce::FileLogger logger;

  private:
    //==============================================================================
    // State Variables
    // + info from the synthesiser for modulation
    //==============================================================================
    int samples_to_next_control_update{CONTROL_RATE_SAMPLES};

    //==============================================================================
    // MPE
    //==============================================================================
    juce::MPESynthesiser synth;
    juce::MPEZoneLayout zone_layout;

    //==============================================================================
    // Parameters
    //==============================================================================
    void update_parameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};