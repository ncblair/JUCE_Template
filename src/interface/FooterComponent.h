#pragma once

class Matrix;

#include <JuceHeader.h>

class FooterComponent : public  juce::Component {
  public:
    FooterComponent(Matrix* m, juce::UndoManager* undoManager);

    void paint (juce::Graphics& g) override;

    void resized() override;

  private:
    // undo redo
    juce::TextButton undoButton, redoButton;
    juce::UndoManager* undo_manager;

    juce::Label mpe_button_label{"Enable MPE", "enable mpe"};
    juce::ToggleButton mpe_button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mpe_button_attachment;

    juce::Label title_label{"Title Label", "template synthesizer"};
    juce::Label created_by_label{"Created By Label", "created by @nthnblair"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FooterComponent)
};