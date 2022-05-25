#include "SoundfileComponent.h"
#include "../managers/matrix/Matrix.h"

SoundfileComponent::SoundfileComponent(Matrix* m, juce::ValueTree audiotree) : audio_tree(audiotree) {
    matrix = m;

    addAndMakeVisible (&load_file_button);
    load_file_button.setButtonText("open");
    load_file_button.onClick = [this] { matrix->load_audio_file(); };
    
    addAndMakeVisible (&remove_file_button);
    remove_file_button.setButtonText("remove");
    remove_file_button.onClick = [this] { matrix->remove_audio_file(); };

    // addAndMakeVisible (&play_button);
    // play_button.setButtonText("play");
    // play_button.onClick = [this] { playButtonClicked(); };
    // // play_button.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    // play_button.setEnabled (false);

    addAndMakeVisible(&file_name_label);
    file_name_label.setText("No File Loaded", juce::dontSendNotification);
    valueTreePropertyChanged(audio_tree, juce::Identifier("NAME")); // init name correctly
    audio_tree.addListener(this);
}

// void SoundfileComponent::paint(juce::Graphics& g) {

// }

void SoundfileComponent::resized() {
    // play_button.setBounds();
    auto area = getLocalBounds();
    load_file_button.setBounds(area.removeFromLeft(proportionOfWidth(0.25f)));
    remove_file_button.setBounds(area.removeFromLeft(proportionOfWidth(0.25f)));
    file_name_label.setBounds(area);
}

// void SoundfileComponent::play_button_clicked() {
//     auto is_currently_playing = soundfile_tree.getProperty("IS_PLAYING");
//     play_button.setButtonText(is_currently_playing ? "play" : "stop");
//     soundfile_tree.setProperty("IS_PLAYING", !is_currently_playing, matrix->getUndoManager());
// }

void SoundfileComponent::valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property){
    if (treeWhosePropertyHasChanged.hasType("AUDIO") && property == juce::Identifier("NAME")) {
        if (treeWhosePropertyHasChanged.hasProperty("NAME")) {
            file_name_label.setText(treeWhosePropertyHasChanged.getProperty("NAME"), juce::dontSendNotification);
            // play_button.setEnabled(true);
        }
        else {
            file_name_label.setText("No File Loaded", juce::dontSendNotification);
            // play_button.setEnabled(false);
        }
    }
}