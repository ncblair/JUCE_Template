#include "FooterComponent.h"
#include "../matrix/Matrix.h"

FooterComponent::FooterComponent(Matrix* m) : undoButton("undo"), redoButton("redo") {
    // undo redo
    addAndMakeVisible (undoButton);
    addAndMakeVisible (redoButton);
    undo_manager = m->getUndoManager();
    undoButton.onClick = [this] { undo_manager->undo(); };
    redoButton.onClick = [this] { undo_manager->redo(); };

    //mpe button
    addAndMakeVisible(mpe_button);
    
    // set colours of mpe button
    mpe_button.setColour(juce::ToggleButton::ColourIds::tickColourId, juce::Colours::black);
    mpe_button.setColour(juce::ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);
    mpe_button.setTooltip("Enable Midi Polyphonic Expression (MPE)");
    mpe_button.setName(PARAMETER_NAMES[PARAM::ENABLE_MPE]);
    // //attach mpe button
    mpe_button_attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(*(m->getPropertyTree()), PARAMETER_NAMES[PARAM::ENABLE_MPE], mpe_button);
    
    //labels
    addAndMakeVisible(title_label);
    title_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(created_by_label);
    created_by_label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mpe_button_label);
    mpe_button_label.setJustificationType(juce::Justification::centred);
}

void FooterComponent::paint (juce::Graphics& g) {
    g.setColour (juce::Colours::black);

    juce::Line<float> line1 (juce::Point<float> (0, 0),
                            juce::Point<float> (getWidth(), 0));
    
    juce::Line<float> line2 (juce::Point<float> (proportionOfWidth(0.46666666666f), 0),
                            juce::Point<float> (proportionOfWidth(0.46666666666f), getHeight()));
    
    juce::Line<float> line3 (juce::Point<float> (proportionOfWidth(0.73333333333f), 0),
                            juce::Point<float> (proportionOfWidth(0.73333333333f), getHeight()));

    g.drawLine (line1, 1.0);
    g.drawLine (line2, 1.0);
    g.drawLine (line3, 1.0);
}

void FooterComponent::resized() {
    undoButton.setBoundsRelative(0.0f, 0.0f, 0.1, 1.0f);
    redoButton.setBoundsRelative(0.1f, 0.0f, 0.1, 1.0f);
    title_label.setBoundsRelative(0.2f, 0.0f, 0.26666666666f, 1.0f);
    created_by_label.setBoundsRelative(0.73333333333f, 0.0f, 0.26666666666f, 1.0f);
    mpe_button_label.setBoundsRelative(0.4933333333f, 0.0f, 0.15333333f, 1.0f);
    mpe_button.setBounds(proportionOfWidth(0.648f), 0, 21, getHeight());
}