#pragma once

class LabeledKnobComponent;

#include <JuceHeader.h>

class ADSRComponent : public  juce::Component {
  public:
    ADSRComponent(juce::AudioProcessorValueTreeState& apvts, int index);
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
    ADSRParentComponent(juce::AudioProcessorValueTreeState& apvts, int number_envelopes);
    void resized() override;
  private:
    std::vector<std::unique_ptr<ADSRComponent>> envelopes;
    int num_envelopes;
    int visible_envelope{0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRParentComponent)
};
