#pragma once

#include <JuceHeader.h>

class PopupParameterComponent : public juce::Component, public juce::Timer {
  public:
    PopupParameterComponent();

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
  private:
    juce::String getStringFor(juce::Component& c);
    juce::Label parameter_text;
    juce::Component* current_component;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupParameterComponent)
};