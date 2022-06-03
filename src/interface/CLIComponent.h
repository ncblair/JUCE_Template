#pragma once

class Matrix;

#include <JuceHeader.h>
#include <regex>

class CLIComponent : public juce::Component , 
                     public juce::KeyListener, 
                     public juce::TextEditor::Listener {
  public:
    CLIComponent(Matrix* m);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void send_command();

    // CLI
    juce::String get_next_command(juce::String root);
    juce::String get_prev_command(juce::String root);
    juce::String process_command(juce::String text);

    bool keyPressed(const juce::KeyPress &kp, juce::Component *orig) override;
    void textEditorTextChanged(TextEditor &) override;

    juce::ValueTree get_state();
    void set_state(juce::ValueTree master_state);
    
  private:
    Matrix* matrix;
    // const juce::String placehold_text = "Enter your command here";
    juce::String m_root = "";
    juce::Label cli_prompt;
    juce::Label error_message_container;
    juce::TextEditor cli_entry;

    std::regex cmd_regex;

    juce::String process_connect(juce::String args);
    juce::String process_disconnect(juce::String args);

    std::unordered_map<juce::String, std::function<juce::String(juce::String)>> COMMANDS;

    juce::String join(const std::unordered_map<juce::String, std::function<juce::String(juce::String)>>& string_map, juce::String delimiter);

    
    // static const juce::String COMMANDS = {"CONNECT", "DISCONNECT"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CLIComponent)
};