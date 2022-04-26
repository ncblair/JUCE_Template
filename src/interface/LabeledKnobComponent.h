#pragma once

#include <JuceHeader.h>

class LabeledKnobComponent : public  juce::Component {
  public:
    // LabeledKnobComponent(juce::String tooltip, juce::String label_str) {
    //     addAndMakeVisible(knob);
    //     addAndMakeVisible(label);
    //     knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    //     knob.setTooltip(tooltip);
    //     label.setText (label_str, juce::dontSendNotification);
    // }
    LabeledKnobComponent(juce::AudioProcessorValueTreeState& apvts, juce::String param_name, juce::String tooltip, juce::String label_str);

    // void paint (juce::Graphics& g) override {
    // }

    void resized() override;
  private:
    juce::Slider knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow};
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob_attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledKnobComponent)
};