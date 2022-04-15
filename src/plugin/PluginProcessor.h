#pragma once

// #include <juce_audio_processors/juce_audio_processors.h>
#include <JuceHeader.h>
#include "../audio/SynthVoice.h"
#include "../audio/ADSR.h"
#include "../util/ParamAttachment.h"
#include "../util/CustomParameters.h"

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts; // create audio processor value tree state for parameters
    juce::MidiKeyboardState keyboard_state; // create MidiKeyboardState for MIDI Visualization and Playback
    // std::unique_ptr<juce::UndoManager> undo_manager;
    // juce::FileLogger logger;

    ParamAttachment<float> gain_param;
    ParamAttachment<bool> mpe_param; // We will use this bool parameter for mpe
    void update_gain_param();
    void update_mpe_param();

    ADSRParent envelopes{3};

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::MPESynthesiser synth;
    juce::MPEZoneLayout zone_layout;
    const int NUM_VOICES = 20;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};