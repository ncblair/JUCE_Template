#pragma once

class Matrix;

#include <JuceHeader.h>

class LabeledKnobComponent : public juce::Component, public juce::DragAndDropTarget {
  public:
    LabeledKnobComponent(Matrix* m, int p_id, juce::String tooltip, juce::String label_str);
    // void paint (juce::Graphics& g) override {
    // }

    void resized() override;

    // Drag and Drop Methods
    bool isInterestedInDragSource (const SourceDetails &dragSourceDetails) override;
    void itemDropped (const SourceDetails &dragSourceDetails) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
  private:
    juce::Slider knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox};
    juce::Label label;

    // juce::Label value_popup;
    // std::unique_ptr<juce::CallOutBox> value_popup_box;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob_attachment;
    Matrix* matrix;
    int param_id;
    juce::PopupMenu right_click_menu;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledKnobComponent)
};