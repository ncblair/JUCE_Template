#pragma once

class Matrix;

#include <JuceHeader.h>

class LabeledKnobComponent : public juce::Component, public juce::DragAndDropTarget, public juce::AudioProcessorValueTreeState::Listener, juce::ValueTree::Listener {
  public:
    LabeledKnobComponent(Matrix* m, int p_id, juce::String tooltip, juce::String label_str);
    ~LabeledKnobComponent();
    void paint (juce::Graphics& g) override;

    void resized() override;

    // Drag and Drop Methods
    bool isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    // void sliderValueChanged (juce::Slider *slider) override; // for mod_knob
    // for mod knob
    void parameterChanged (const juce::String &parameterID, float newValue); 
    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;
    void valueTreeChildAdded (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
  private:
    void redirect_mod_knob(int mod_id);
    void num_modulators_changed();
    void reset_tooltip();

    juce::Slider knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox};
    juce::Slider mod_knob{juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox};
    juce::Label label;
    juce::String tooltip_text;

    // juce::Label value_popup;
    // std::unique_ptr<juce::CallOutBox> value_popup_box;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knob_attachment;
    Matrix* matrix;
    int param_id;
    juce::PopupMenu right_click_menu;
    juce::ValueTree matrix_tree;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabeledKnobComponent)
};