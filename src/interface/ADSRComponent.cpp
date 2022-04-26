#include "ADSRComponent.h"
#include "LabeledKnobComponent.h"


ADSRComponent::ADSRComponent(juce::AudioProcessorValueTreeState& apvts, int index) {
    auto idx = std::to_string(index);
    auto atk_param_name = "ADSR_" + idx + "_ATTACK";
    auto atk_tooltip = "Attack in (ms) of Envelope " + idx;
    auto atk_label = "attack";
    attack_knob = std::make_unique<LabeledKnobComponent>(apvts, atk_param_name, atk_tooltip, atk_label);

    auto dec_param_name = "ADSR_" + idx + "_DECAY";
    auto dec_tooltip = "Decay in (ms) of Envelope " + idx;
    auto dec_label = "decay";
    decay_knob = std::make_unique<LabeledKnobComponent>(apvts, dec_param_name, dec_tooltip, dec_label);

    auto sus_param_name = "ADSR_" + idx + "_SUSTAIN";
    auto sus_tooltip = "Sustain in (ms) of Envelope " + idx;
    auto sus_label = "sustain";
    sustain_knob = std::make_unique<LabeledKnobComponent>(apvts, sus_param_name, sus_tooltip, sus_label);

    auto rel_param_name = "ADSR_" + idx + "_RELEASE";
    auto rel_tooltip = "Release in (ms) of Envelope " + idx;
    auto rel_label = "release";
    release_knob = std::make_unique<LabeledKnobComponent>(apvts, rel_param_name, rel_tooltip, rel_label);
}

void ADSRComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::blue);
}

void ADSRComponent::resized() {
    attack_knob->setBoundsRelative(0.0f, 0.8f, 0.25f, 0.2f);
    decay_knob->setBoundsRelative(0.25f, 0.8f, 0.25f, 0.2f);
    sustain_knob->setBoundsRelative(0.5f, 0.8f, 0.25f, 0.2f);
    release_knob->setBoundsRelative(0.75f, 0.8f, 0.25f, 0.2f);
}

//==============================================================================
// ADSRParentComponent
//==============================================================================

ADSRParentComponent::ADSRParentComponent(juce::AudioProcessorValueTreeState& apvts, int number_envelopes) {
    num_envelopes = number_envelopes;
    for (int i = 0; i < num_envelopes; ++i) {
        envelopes.push_back(std::make_unique<ADSRComponent>(apvts, i));
        addChildComponent(*(envelopes[i]));
    }
    envelopes[visible_envelope]->setVisible(true);
}
void ADSRParentComponent::resized() {
    for (auto& envelope : envelopes) {
        envelope->setBounds(getLocalBounds());
    }
}
