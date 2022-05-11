#pragma once

//forward declarations
class FooterComponent;
class ADSRParentComponent;
class LabeledKnobComponent;

//includes
#include "PluginProcessor.h"
#include "../interface/CustomLookAndFeel.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::DragAndDropContainer, public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor (PluginProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void timerCallback() override;

private:
    CustomLookAndFeel look_and_feel;

    std::unique_ptr<FooterComponent> footer;
    std::unique_ptr<juce::MidiKeyboardComponent> midi_keyboard;
    std::unique_ptr<ADSRParentComponent> envelopes;
    std::unique_ptr<LabeledKnobComponent> gain_knob;
    std::unique_ptr<LabeledKnobComponent> semitones_knob;

    juce::TooltipWindow tooltip{ this };

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;

    // UNCOMMENT FOR: Background Image
    //const juce::Image bg_img = juce::ImageCache::getFromMemory(BinaryData::BG_png, BinaryData::BG_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
