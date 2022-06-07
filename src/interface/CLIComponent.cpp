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
    float_regex = std::regex("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)");

    cli_tree = matrix->getCLITree();
    cli_history_index = 0;
    m_root = "";
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
    add_to_cli_history(cmd);
    cli_history_index = 0;
    m_root = "";
    cli_entry.clear();
    cli_entry.moveCaretToEnd();
    error_message_container.setText(error_msg, juce::NotificationType::dontSendNotification);
    repaint();
    
}

juce::String CLIComponent::get_next_command(juce::String root) {
    auto history_depth = cli_tree.getNumChildren();
    if (history_depth == 0) {
        return "";
    }
    cli_history_index = cli_history_index >= history_depth ? history_depth : cli_history_index + 1;
    return  cli_tree.getChild(history_depth - cli_history_index).getProperty(Matrix::CLI_COMMAND_ID).toString();
}

juce::String CLIComponent::get_prev_command(juce::String root) {
    auto history_depth = cli_tree.getNumChildren();
    if (history_depth == 0) {
        return "";
    }
    cli_history_index = cli_history_index <= 0 ? 0 : cli_history_index - 1;
    if (cli_history_index == 0) {
        return root;
    }
    else {
        return cli_tree.getChild(history_depth - cli_history_index).getProperty(Matrix::CLI_COMMAND_ID).toString();
    }
}

void CLIComponent::add_to_cli_history(juce::String cmd) {
    if (cmd != "") {
        auto new_cmd_child = juce::ValueTree(Matrix::CLI_COMMAND_ID);
        new_cmd_child.setProperty(Matrix::CLI_COMMAND_ID, cmd, nullptr);
        cli_tree.appendChild(new_cmd_child, nullptr);
        if (cli_tree.getNumChildren() > Matrix::MAX_CLI_HISTORY) {
            cli_tree.removeChild(0, nullptr);
        }
        matrix->save_cli_history();
    }
}


bool CLIComponent::keyPressed(const juce::KeyPress &kp, juce::Component *orig) {
    if (cli_entry.isTextInputActive()) {
        if (kp == juce::KeyPress::upKey) {
            cli_entry.setText(get_next_command(m_root), juce::NotificationType::dontSendNotification);
        }
        else if (kp == juce::KeyPress::downKey) {
            cli_entry.setText(get_prev_command(m_root), juce::NotificationType::dontSendNotification);
        }
    }
    return false;
}

void CLIComponent::textEditorTextChanged(TextEditor& editor) {
    // error_message_container.setText("", juce::NotificationType::dontSendNotification);
    m_root = editor.getText();
}

// juce::ValueTree CLIComponent::get_state() {

// }
// void CLIComponent::set_state(juce::ValueTree master_state) {

// }

juce::String CLIComponent::process_command(juce::String text) {
    // cmd(arg1, arg2, arg3)
    auto trimmed = text.toUpperCase().replace(" ", "", false);
    if (!trimmed.contains("(")) {
        return "Error: Invalid Command, missing '('";
    }
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
    auto args_list = get_args(args);
    if (args_list.size() < 3) {
        return "Error: too few arguments. expected 3, got " + std::to_string(args_list.size());
    }
    else if (args_list.size() > 3) {
        return "Error: too many arguments. expected 3, got " + std::to_string(args_list.size());
    }
    auto mod_id = args_list[0];
    auto param_id = args_list[1];
    auto depth = args_list[2];

    auto m_id = MODULATOR_NAMES.indexOf(mod_id); 
    auto p_id = PARAMETER_NAMES.indexOf(param_id);

    float d;
    if (std::regex_match(depth.toStdString(), float_regex)) {
        d = depth.getFloatValue();
    } else {
        return "Error: wrong type for argument depth. expected float, got " + depth;
    }
    if (d < 0.0f || d > 1.0f) {
        return "Error: modulation depth out of range. expected 0-1, got " + depth;
    }
    if (m_id == -1) {
        return "Error: modulator not recognized. got " + mod_id;
    }
    if (p_id == -1) {
        return "Error: parameter not recognized. got " + param_id;
    }
    matrix->connect(m_id, p_id, d);

    return "CONNECT COMMAND: " + args;
}


juce::String CLIComponent::process_disconnect(juce::String args) {
        auto args_list = get_args(args);
    if (args_list.size() < 2) {
        return "Error: too few arguments. expected 3, got " + std::to_string(args_list.size());
    }
    else if (args_list.size() > 2) {
        return "Error: too many arguments. expected 3, got " + std::to_string(args_list.size());
    }
    auto mod_id = args_list[0];
    auto param_id = args_list[1];

    auto m_id = MODULATOR_NAMES.indexOf(mod_id);
    auto p_id = PARAMETER_NAMES.indexOf(param_id);
    if (m_id == -1) {
        return "Error: modulator not recognized. got " + mod_id;
    }
    if (p_id == -1) {
        return "Error: parameter not recognized. got " + param_id;
    }
    matrix->disconnect(m_id, p_id);

    return "DISCONNECT COMMAND: " + args;
}

std::vector<juce::String> CLIComponent::get_args(juce::String args) {
    std::cout << args.toStdString() << std::endl;
    auto arg_list = std::vector<juce::String>();
    int nest = 0;
    int index = 0;
    auto arg_length = args.length();
    while (index < arg_length) {
        auto cur = args[index];
        if (cur == '(') {
            nest++;
        }
        else if (cur == ')') {
            if (nest == 0) {
                arg_list.push_back(args.substring(0, index));
                std::cout << args.substring(0, index).toStdString() << std::endl;
                return arg_list;
            }
            else {
                nest--;
            }
        }
        else if (cur == ',') {
            if (nest == 0) {
                arg_list.push_back(args.substring(0, index));
                std::cout << args.substring(0, index).toStdString() << std::endl;
                args = args.substring(index+1);
                arg_length = args.length();
                index = 0;
            }
        }
        index++;
    }
    if (nest > 0) {
        std::cout << "No end Paren" << std::endl;
    }
    return arg_list;
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