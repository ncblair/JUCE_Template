#include <JuceHeader.h>
#pragma once

class FooterComponent : public  juce::Component {
  public:
    FooterComponent(juce::AudioProcessorValueTreeState& apvts) {
       //mpe button
        addAndMakeVisible(mpe_button);
        
        // set colours of mpe button
        mpe_button.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
        mpe_button.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
        mpe_button.setTooltip("Enable Midi Polyphonic Expression (MPE)");
        //attach mpe button
        mpe_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "MPE_PARAM", mpe_button);
        
        //labels
        addAndMakeVisible(title_label);
        title_label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(created_by_label);
        created_by_label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(mpe_button_label);
        mpe_button_label.setJustificationType(juce::Justification::centred);
    }

    void paint (juce::Graphics& g) override {
        g.setColour (juce::Colours::black);

        juce::Line<float> line1 (juce::Point<float> (0, 0),
                                juce::Point<float> (getWidth(), 0));
        
        juce::Line<float> line2 (juce::Point<float> (proportionOfWidth(0.3333333f), 0),
                                juce::Point<float> (proportionOfWidth(0.3333333f), getHeight()));
        
        juce::Line<float> line3 (juce::Point<float> (proportionOfWidth(0.6666667f), 0),
                                juce::Point<float> (proportionOfWidth(0.6666667f), getHeight()));
    
        g.drawLine (line1, 1.0);
        g.drawLine (line2, 1.0);
        g.drawLine (line3, 1.0);
    }

    void resized() override {
        title_label.setBoundsRelative(0.0f, 0.0f, 0.3333333f, 1.0f);
        created_by_label.setBoundsRelative(0.6666667f, 0.0f, 0.3333333f, 1.0f);
        mpe_button_label.setBoundsRelative(0.3666666f, 0.0f, 0.1933333f, 1.0f);
        mpe_button.setBounds(proportionOfWidth(0.56f), 0, 21, getHeight());
    }

  private:
    juce::Label mpe_button_label{"Enable MPE", "enable mpe"};
    juce::ToggleButton mpe_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mpe_button_attachment;

    juce::Label title_label{"Title Label", "template synthesizer"};
    juce::Label created_by_label{"Created By Label", "created by @nthnblair"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FooterComponent)
};