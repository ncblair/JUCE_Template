#pragma once

class Matrix;

#include <JuceHeader.h>

class CLIComponent : public juce::Component , 
                     public juce::KeyListener, 
                     public juce::TextEditor::Listener {
  public:
    CLIComponent(Matrix* m);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void send_command();

    bool keyPressed(const juce::KeyPress &kp, juce::Component *orig) override;
    void textEditorTextChanged(TextEditor &) override;
    
  private:
    Matrix* matrix;
    // const juce::String placehold_text = "Enter your command here";
    juce::String root = "";
    juce::Label cli_prompt;
    juce::Label error_message_container;
    juce::TextEditor cli_entry;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CLIComponent)
};