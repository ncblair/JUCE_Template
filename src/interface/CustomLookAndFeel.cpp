#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel() {
    // setColour(juce::TooltipWindow::backgroundColourId, juce::Colours::white);
    // setColour(juce::TooltipWindow::textColourId, juce::Colours::black);
    // setColour(juce::TooltipWindow::outlineColourId, juce::Colours::transparentWhite);
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(getTypefaceForFont(font));
}

//https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
void CustomLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) {
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width  * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // fill circle
    if (slider.isEnabled()) {
        g.setColour(juce::Colours::black);
    }
    else {
        g.setColour (juce::Colours::lightgrey);
    }
    g.fillEllipse (rx, ry, rw, rw);

    // pointer
    juce::Path p;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 2.0f;
    p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);

    // p.addEllipse(-pointerLength / 2.0f, -radius * 0.7f, pointerLength, pointerLength);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

    // draw pointer
    g.setColour (juce::Colours::white);
    g.fillPath (p);
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    g.fillAll (label.findColour (Label::backgroundColourId));

    auto alpha = label.isEnabled() ? 1.0f : 0.5f;
    // const Font font (getLabelFont (label));

    g.setColour (juce::Colours::black.withMultipliedAlpha(alpha));
    // g.setFont (font);

    auto textArea = getLabelBorderSize (label).subtractedFrom (label.getLocalBounds());
    g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                        jmax (1, (int) ((float) textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());
}

// void CustomLookAndFeel::setFontSize(float fontSize) {
//     font.setHeight(fontSize);
// }

// juce::Rectangle<int> CustomLookAndFeel::getTooltipBounds(const String& tipText, juce::Point<int> screenPos, juce::Rectangle<int> parentArea) {
//     parentArea.removeFromLeft(parentArea.proportionOfWidth(0.02f));
//     parentArea.removeFromRight(parentArea.proportionOfWidth(0.05f));
//     return parentArea.removeFromBottom(parentArea.proportionOfHeight(tooltip_height));;
// }

// void CustomLookAndFeel::drawTooltip (Graphics& g, const String& text, int width, int height)
// {
//     Rectangle<int> bounds (width, height);
//     auto cornerSize = 0.0f;

//     g.setColour (findColour (juce::TooltipWindow::backgroundColourId));
//     g.fillRoundedRectangle (bounds.toFloat(), cornerSize);

//     g.setColour (findColour (juce::TooltipWindow::outlineColourId));
//     g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

//     const int maxToolTipWidth = 400;

//     AttributedString s;
//     s.setJustification (juce::Justification::centredLeft);
//     s.append (text, font, findColour(juce::TooltipWindow::textColourId));

//     TextLayout tl;
//     tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
//     tl.draw (g, {static_cast<float> (width), static_cast<float> (height)});
// }
