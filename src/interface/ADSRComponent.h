#include <JuceHeader.h>
#include "LabeledKnobComponent.h"
#pragma once

class ADSRParentComponent: public juce::Component {
  public:
    ADSRParentComponent(juce::AudioProcessorValueTreeState& apvts, int number_envelopes) {
        num_envelopes = number_envelopes;
        for (int i = 0; i < num_envelopes; ++i) {
            envelopes.push_back(std::make_unique<ADSRComponent>(apvts, i));
            addChildComponent(*(envelopes[i]));
        }
        makeVisible(*(enevelopes[visible_envelope]));
    }
  private:
    std::vector<std::unique_ptr<ADSRComponent>> envelopes;
    int num_envelopes;
    int visible_envelope{0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRParentComponent)
};

class ADSRComponent : public  juce::Component {
  public:
    ADSRComponent(juce::AudioProcessorValueTreeState& apvts, int index) {
        auto atk_param_name = "ADSR_" << index << "_ATTACK";
        auto atk_tooltip = "Attack in (ms) of Envelope " << index;
        auto atk_label = "attack";
        attack_knob = std::make_unique<LabeledKnobComponent>(apvts, atk_param_name, atk_tooltip, atk_label);

        auto dec_param_name = "ADSR_" << index << "_DECAY";
        auto dec_tooltip = "Decay in (ms) of Envelope " << index;
        auto dec_label = "decay";
        decay_knob = std::make_unique<LabeledKnobComponent>(apvts, dec_param_name, dec_tooltip, dec_label);

        auto sus_param_name = "ADSR_" << index << "_SUSTAIN";
        auto sus_tooltip = "Sustain in (ms) of Envelope " << index;
        auto sus_label = "sustain";
        sustain_knob = std::make_unique<LabeledKnobComponent>(apvts, sus_param_name, sus_tooltip, sus_label);

        auto rel_param_name = "ADSR_" << index << "_RELEASE";
        auto rel_tooltip = "Release in (ms) of Envelope " << index;
        auto rel_label = "release";
        release_knob = std::make_unique<LabeledKnobComponent>(apvts, rel_param_name, rel_tooltip, rel_label);
    }

    void paint (juce::Graphics& g) override {
        g.fillAll (juce::Colours::blue);
    }

    void resized() override {
        for (int i = 0; i < num_envelopes; ++i) {
            attack_knob->setBoundsRelative(0.0f, 0.8f, 0.25f, 0.2f);
            decay_knob->setBoundsRelative(0.25f, 0.8f, 0.25f, 0.2f);
            sustain_knob->setBoundsRelative(0.5f, 0.8f, 0.25f, 0.2f);
            release_knob->setBoundsRelative(0.75f, 0.8f, 0.25f, 0.2f);
        }
    }

  private:
    // ADSR Knobs
    std::unique_ptr<LabeledKnobComponent> attack_knob;
    std::unique_ptr<LabeledKnobComponent> decay_knob;
    std::unique_ptr<LabeledKnobComponent> sustain_knob;
    std::unique_ptr<LabeledKnobComponent> release_knob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRComponent)
};