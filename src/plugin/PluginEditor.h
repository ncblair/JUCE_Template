#pragma once

#include <BinaryData.h>
#include "PluginProcessor.h"
#include "../interface/CustomLookAndFeel.h"
#include "../interface/FooterComponent.h"
// #include "bgfx/bgfx.h"

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CustomLookAndFeel look_and_feel;

    std::unique_ptr<FooterComponent> footer;
    std::unique_ptr<juce::MidiKeyboardComponent> midi_keyboard;
    juce::TooltipWindow tooltip{ this };
    // juce::Component midi_keyboard{"MidiKeyboard"};
    // juce::Component panel_1{"Panel1"};
    // juce::Component panel_envelope{"PanelEnvelope"};

    juce::Slider gain_slider{juce::Slider::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow};
    juce::Label gain_label{"GainLabel", "Gain"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gain_slider_attachment;

    // juce::Label mpe_button_label{"Enable MPE", "enable mpe"};
    // juce::ToggleButton mpe_button;
    // std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mpe_button_attachment;

    // juce::Label title_label{"Title Label", "template synthesizer"};
    // juce::Label created_by_label{"Created By Label", "created by @nthnblair"};
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    // UNCOMMENT FOR: Background Image
    //const juce::Image bg_img = juce::ImageCache::getFromMemory(BinaryData::BG_png, BinaryData::BG_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
