#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
  private: 
    juce::Font font{juce::Typeface::createSystemTypefaceFor(BinaryData::AnekTamilRegular_ttf,
                                                           BinaryData::AnekTamilRegular_ttfSize)};
    
  public:
    CustomLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

    // void setFontSize(float fontSize);


    //tooltip
    float tooltip_height = 0.05f; // percent of total height
    juce::Rectangle<int> getTooltipBounds(const String& tipText, juce::Point<int> screenPos, juce::Rectangle<int> parentArea) override;
    void drawTooltip (Graphics& g, const String& text, int width, int height) override;
};