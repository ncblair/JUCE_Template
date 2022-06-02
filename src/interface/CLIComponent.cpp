#include "CLIComponent.h"
#include "../matrix/Matrix.h"


CLIComponent::CLIComponent(Matrix* m)  {
    // undo redo
    matrix = m;

    cli_entry.setJustification(juce::Justification::centredLeft);
    cli_entry.setColour(juce::TextEditor::backgroundColourId, juce::Colours::transparentWhite);
    cli_entry.setFont(juce::Font(16, juce::Font::bold));
    cli_entry.setColour(juce::TextEditor::textColourId, juce::Colours::black);

    cli_prompt.setFont(juce::Font(16, juce::Font::bold));
    cli_prompt.setColour(juce::Label::outlineColourId, juce::Colours::transparentWhite);
    cli_prompt.setJustificationType(juce::Justification(juce::Justification::centredRight));
    cli_prompt.setText("> ", juce::NotificationType::dontSendNotification);

    error_message_container.setFont(juce::Font(16, juce::Font::bold));
    error_message_container.setJustificationType(juce::Justification::centredRight);
    error_message_container.setColour(juce::Label::outlineColourId, juce::Colours::transparentWhite);
    error_message_container.setColour(juce::Label::textColourId, juce::Colours::red);

    addAndMakeVisible(cli_prompt);
    addAndMakeVisible(cli_entry);
    addAndMakeVisible(error_message_container);
    cli_entry.onReturnKey = [this]() {send_command();};
    cli_entry.addKeyListener(this);
    cli_entry.addListener(this);
    
}

void CLIComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 2);
}

void CLIComponent::resized() {
    auto bounds = getLocalBounds();
    int  prompt_w = cli_prompt.getFont().getStringWidth(cli_prompt.getText());
    cli_prompt.setBounds(bounds.removeFromLeft(proportionOfWidth(0.01f) + prompt_w));
    cli_entry.setBounds(bounds);
    error_message_container.setBounds(bounds);
}

void CLIComponent::send_command() {
    auto cmd = cli_entry.getText();
    auto error_msg = matrix->enter_command(cmd);
    cli_entry.clear();
    error_message_container.setText(error_msg, juce::NotificationType::dontSendNotification);
}

bool CLIComponent::keyPressed(const juce::KeyPress &kp, juce::Component *orig) {
    if (cli_entry.isTextInputActive()) {
        if (kp == juce::KeyPress::upKey) {
            cli_entry.setText(matrix->get_prev_command(root), juce::NotificationType::dontSendNotification);
        }
        else if (kp == juce::KeyPress::downKey) {
            cli_entry.setText(matrix->get_next_command(root), juce::NotificationType::dontSendNotification);
        }
    }
    return false;
}

void CLIComponent::textEditorTextChanged(TextEditor &) {
    // error_message_container.setText("", juce::NotificationType::dontSendNotification);
}