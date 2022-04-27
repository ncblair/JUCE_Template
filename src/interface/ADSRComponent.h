#pragma once

class LabeledKnobComponent;
class ModulatorLabel;
class Matrix;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};

class ADSRParentComponent: public juce::Component {
  public:
    ADSRParentComponent(Matrix* matrix, std::vector<int>& mod_ids);
    ~ADSRParentComponent() override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
  private:
    std::vector<std::unique_ptr<ADSRComponent>> envelopes;
    std::vector<std::unique_ptr<ModulatorLabel>> modulator_labels;
    int num_envelopes;
    int visible_envelope{0};
    juce::MouseListener active_envelope_listener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRParentComponent)
};
