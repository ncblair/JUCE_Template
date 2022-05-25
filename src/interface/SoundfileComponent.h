#pragma once

class Matrix;

#include <JuceHeader.h>

class SoundfileComponent : public juce::Component, public juce::ValueTree::Listener {
  public:
    SoundfileComponent(Matrix* m, juce::ValueTree audiotree);

    // void paint(juce::Graphics& g) override;
    void resized() override;
    // void play_button_clicked();

    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    
  private:

    juce::ValueTree audio_tree;
    Matrix* matrix;

    juce::TextButton load_file_button;
    juce::TextButton remove_file_button;
    // juce::TextButton play_button;

    juce::Label file_name_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SoundfileComponent)
};