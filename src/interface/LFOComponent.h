#pragma once

class LabeledKnobComponent;
class ModulatorLabel;
class Matrix;
class LFOViewer;
#include "ModulatorParentComponent.h"
#include <JuceHeader.h>

class LFOComponent : public juce::Component {
  public:
    LFOComponent(Matrix* matrix, int mod_id);
    // ~LFOComponent() override;
    void paint (juce::Graphics& g) override;
    void resized() override;

  private:
    // LFO Knobs
    std::unique_ptr<LabeledKnobComponent> frequency_knob;
    std::unique_ptr<LabeledKnobComponent> tempo_knob;
    std::unique_ptr<LabeledKnobComponent> rise_knob;

    std::unique_ptr<LFOViewer> lfo_viewer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOComponent)
};

class LFOParentComponent: public ModulatorParentComponent {
  public:
    LFOParentComponent(Matrix* matrix, std::vector<int>& mod_ids);
    void init_child_components_and_visibility_attachment(Matrix* matrix, std::vector<int>& mod_ids) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOParentComponent)
};
