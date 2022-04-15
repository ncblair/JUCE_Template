#include <JuceHeader.h>
#pragma once

class LabeledKnobComponent : public  juce::Component {
  public:
    LabeledKnobComponent(juce::String tooltip, juce::String label_str) {
        addAndMakeVisible(knob);
        addAndMakeVisible(label);
        knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
        knob.setTooltip(tooltip);
        label.setText (label_str);
    }
    LabeledKnobComponent(juce::AudioProcessorValueTreeState& apvts, juce::String param_name, juce::String tooltip, juce::String label_str) {
        addAndMakeVisible(knob);
        knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
        knob.setTooltip(tooltip);
        knob_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, param_name, knob);
        addAndMakeVisible(label);
        label.setText (label_str);
        label.setJustificationType(juce::Justification::centred);
    // gain_label.setText ("Gain", juce::dontSendNotification);
    gain_label.attachToComponent (&gain_slider, false); // false sets to top, true sets to left
    }

    // void paint (juce::Graphics& g) override {
    // }

    void resized() override {
        knob.setBounds(getLocalBounds());
    }

  private:
    juce::Slider knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow};
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};