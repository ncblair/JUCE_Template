#include "LabeledKnobComponent.h"

LabeledKnobComponent::LabeledKnobComponent(juce::AudioProcessorValueTreeState& apvts, juce::String param_name, juce::String tooltip, juce::String label_str) {
    addAndMakeVisible(knob);
    knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    knob.setTooltip(tooltip);
    knob_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, param_name, knob);
    addAndMakeVisible(label);
    label.setText (label_str, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    // gain_label.setText ("Gain", juce::dontSendNotification);
    // gain_label.attachToComponent (&gain_slider, false); // false sets to top, true sets to left
}

void LabeledKnobComponent::resized() {
    knob.setBounds(getLocalBounds());
}

