#pragma once

class Matrix;
#if NEEDS_MODULATORS
  class NoteState;
#endif

#include "PluginProcessorBase.h"
#include "../matrix/ParameterDefines.h"
#if NEEDS_MODULATORS
  #include "../modulators/NoteState.h"
#endif

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
    void numChannelsChanged() override;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

    //==============================================================================
    // Matrix
    //==============================================================================
    std::unique_ptr<Matrix> matrix;

    //==============================================================================
    // Synthesiser stuff
    //==============================================================================
    #if JucePlugin_IsSynth
      juce::MidiKeyboardState keyboard_state; // create MidiKeyboardState for MIDI Visualization and Playback
    #endif

  private:
    //==============================================================================
    // State Variables
    // + info from the synthesiser for modulation
    //==============================================================================
    #if NEEDS_MODULATORS
      NoteState main_state;
    #endif
    int samples_to_next_control_update;

    //==============================================================================
    // MPE Synth
    //==============================================================================
    #if JucePlugin_IsSynth
      juce::MPESynthesiser synth;
      juce::MPEZoneLayout zone_layout;
      bool is_mpe_enabled{false};

      void process_synth(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

      void update_mpe();
      void enable_mpe();
      void disable_mpe();
    #endif

    //==============================================================================
    // Parameters
    //==============================================================================
    void update_parameters();
    std::array<float, PARAM::TOTAL_NUMBER_PARAMETERS> params;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};