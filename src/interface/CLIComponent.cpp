#include "CLIComponent.h"
#include "../matrix/Matrix.h"


CLIComponent::CLIComponent(Matrix* m) :
    COMMANDS({
        {"CONNECT", [this](juce::String args) -> juce::String { return process_connect(args);}},
        {"DISCONNECT", [this](juce::String args) -> juce::String { return process_disconnect(args);}},
    })
{
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

    auto cmd_regex_string = juce::String("(") + join(COMMANDS, "|") + ")";
    cmd_regex = std::regex(cmd_regex_string.toStdString());
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
    auto error_msg = process_command(cmd);
    cli_entry.clear();
    error_message_container.setText(error_msg, juce::NotificationType::dontSendNotification);
}

juce::String CLIComponent::get_next_command(juce::String root) {
    return "";
}
juce::String CLIComponent::get_prev_command(juce::String root) {
    return "";
}


bool CLIComponent::keyPressed(const juce::KeyPress &kp, juce::Component *orig) {
    if (cli_entry.isTextInputActive()) {
        if (kp == juce::KeyPress::upKey) {
            cli_entry.setText(get_prev_command(m_root), juce::NotificationType::dontSendNotification);
        }
        else if (kp == juce::KeyPress::downKey) {
            cli_entry.setText(get_next_command(m_root), juce::NotificationType::dontSendNotification);
        }
    }
    return false;
}

void CLIComponent::textEditorTextChanged(TextEditor &) {
    // error_message_container.setText("", juce::NotificationType::dontSendNotification);
}

juce::ValueTree CLIComponent::get_state() {

}
void CLIComponent::set_state(juce::ValueTree master_state) {

}

juce::String CLIComponent::process_command(juce::String text) {
    // cmd(arg1, arg2, arg3)
    auto trimmed = text.toUpperCase().replace(" ", "", false);
    auto cmd = trimmed.upToFirstOccurrenceOf("(", false, false);
    auto rest = trimmed.substring(cmd.length()+1);
    auto result = juce::String("");
    if (std::regex_match(cmd.toStdString(), cmd_regex)) {
        result += COMMANDS[cmd](rest);
    }
    else {
        result = "Error: Can't understand " + cmd;
    }
    return result;
}

juce::String CLIComponent::process_connect(juce::String args) {
    // connect(int mod_id, int param_id, float depth);
    auto mod_id = args.initialSectionNotContaining(",");
    auto rest = args.substring(mod_id.length()+1);
    auto param_id = rest.initialSectionNotContaining(",");
    rest = rest.substring(param_id.length()+1);
    auto depth = rest.initialSectionNotContaining(")");

    auto m_id = MODULATOR_NAMES.indexOf(mod_id); 
    auto p_id = PARAMETER_NAMES.indexOf(param_id);
    // TODO: Check if can be turned into Float
    auto d = depth.getFloatValue();

    if (m_id == -1 || p_id == -1 || d > 1.0f || d < 0.0f)  {
        // return "ERROR WRONG: " + std::to_string(m_id) + " " + std::to_string(p_id) + " " + std::to_string(d);
        return "ERROR WRONG: " + mod_id + " " + param_id + " " + depth;

    }
    matrix->connect(m_id, p_id, d);

    return "CONNECT COMMAND: " + args;
}


juce::String CLIComponent::process_disconnect(juce::String args) {
    return "DISCONNECT COMMAND: " + args;
}

juce::String CLIComponent::join(const std::unordered_map<juce::String, std::function<juce::String(juce::String)>>& string_map, juce::String delimiter) {
    auto value = juce::String("");
    bool firstTime{true};
    for (auto& e : string_map) {
        if (firstTime) {
            firstTime = false;
        } else {
            value += delimiter;
        }
        value += e.first;
    }
    return value;
}