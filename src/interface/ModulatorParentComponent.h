#pragma once

class ModulatorLabel;
class Matrix;

#include <JuceHeader.h>

class ModulatorParentComponent: public juce::Component, juce::AudioProcessorValueTreeState::Listener {
  public:
    ModulatorParentComponent(Matrix* m, std::vector<int>& mod_ids);
    ~ModulatorParentComponent() override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    // virtual void init_child_components_and_visibility_attachment(Matrix* m, std::vector<int>& mod_ids)=0;
    virtual void parameterChanged (const juce::String &parameterID, float newValue) override;
  protected:
    // void set_visible(int child_id);
    std::vector<std::unique_ptr<juce::Component>> child_components;
    // std::unique_ptr<juce::ParameterAttachment> visible_child_attachment;
    int num_children;
    int visible_child{0};
  private:
    std::vector<std::unique_ptr<ModulatorLabel>> modulator_labels;
    std::map<int, int> mod_id_to_index;
    Matrix* matrix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorParentComponent)
};
