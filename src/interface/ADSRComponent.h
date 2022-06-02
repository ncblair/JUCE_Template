#pragma once

class LabeledKnobComponent;
class ModulatorLabel;
class Matrix;
class ADSRViewer;

#include "ModulatorParentComponent.h"
#include <JuceHeader.h>

class ADSRComponent : public  juce::Component {
  public:
    ADSRComponent(Matrix* matrix, int mod_id);
    // ~ADSRComponent() override;
    void paint (juce::Graphics& g) override;
    void resized() override;

  private:
    // ADSR Knobs
    std::unique_ptr<LabeledKnobComponent> attack_knob;
    std::unique_ptr<LabeledKnobComponent> decay_knob;
    std::unique_ptr<LabeledKnobComponent> sustain_knob;
    std::unique_ptr<LabeledKnobComponent> release_knob;

    std::unique_ptr<ADSRViewer> adsr_viewer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};

class ADSRParentComponent: public ModulatorParentComponent {
  public:
    ADSRParentComponent(Matrix* m, std::vector<int>& mod_ids);
    void init_child_components_and_visibility_attachment(Matrix* matrix, std::vector<int>& mod_ids) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRParentComponent)
};
