#pragma once

#include <JuceHeader.h>

class TooltipComponent : public juce::Component, public juce::Timer {
  public:
    TooltipComponent();

    // void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
  private:
    juce::String getTipFor(juce::Component& c);
    juce::Label tooltip_text;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TooltipComponent)
};