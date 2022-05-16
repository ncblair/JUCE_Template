#include "ADSRComponent.h"
#include "LabeledKnobComponent.h"
#include "ModulatorLabel.h"
#include "ModulatorViewer.h"
#include "../managers/matrix/Matrix.h"

// #include "../managers/matrix/ADSRDefines.h"


ADSRComponent::ADSRComponent(Matrix* matrix, int mod_id) {

    auto idx = std::to_string(mod_id);
    auto atk_param_id = MODULATOR_PARAMS[mod_id][ATK];
    auto atk_tooltip = "Attack in (ms) of " + MODULATOR_NAMES[mod_id];
    auto atk_label = "ATTACK";
    attack_knob = std::make_unique<LabeledKnobComponent>(matrix, atk_param_id, atk_tooltip, atk_label);
    addAndMakeVisible(*attack_knob);

    auto dec_param_id = MODULATOR_PARAMS[mod_id][DEC];
    auto dec_tooltip = "Decay in (ms) of " + MODULATOR_NAMES[mod_id];
    auto dec_label = "DECAY";
    decay_knob = std::make_unique<LabeledKnobComponent>(matrix, dec_param_id, dec_tooltip, dec_label);
    addAndMakeVisible(*decay_knob);

    auto sus_param_id = MODULATOR_PARAMS[mod_id][SUS];
    auto sus_tooltip = "Sustain in (ms) of " + MODULATOR_NAMES[mod_id];
    auto sus_label = "SUSTAIN";
    sustain_knob = std::make_unique<LabeledKnobComponent>(matrix, sus_param_id, sus_tooltip, sus_label);
    addAndMakeVisible(*sustain_knob);

    auto rel_param_id = MODULATOR_PARAMS[mod_id][REL];
    auto rel_tooltip = "Release in (ms) of " + MODULATOR_NAMES[mod_id];
    auto rel_label = "RELEASE";
    release_knob = std::make_unique<LabeledKnobComponent>(matrix, rel_param_id, rel_tooltip, rel_label);
    addAndMakeVisible(*release_knob);

    envelope_viewer = std::make_unique<ModulatorViewer>(matrix, mod_id);
    addAndMakeVisible(*envelope_viewer);    
}

// ADSRComponent::~ADSRComponent() {

// }

void ADSRComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::skyblue);
}

void ADSRComponent::resized() {
    auto bounds = getLocalBounds();
    envelope_viewer->setBounds(bounds.removeFromTop(proportionOfHeight(0.7f)));
    attack_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    decay_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    sustain_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    release_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
}

//==============================================================================
// ADSRParentComponent
//==============================================================================

ADSRParentComponent::ADSRParentComponent(Matrix* matrix, std::vector<int>& mod_ids) {
    num_envelopes = mod_ids.size();

    for (int i = 0; i < num_envelopes; ++i) {
        envelopes.push_back(std::make_unique<ADSRComponent>(matrix, mod_ids[i]));
        addChildComponent(*(envelopes[i]));
        
        modulator_labels.push_back(std::make_unique<ModulatorLabel>(
            matrix, 
            mod_ids[i],
            "Draggable Component for Modulator " + MODULATOR_NAMES[mod_ids[i]], 
            MODULATOR_NAMES[mod_ids[i]]
        ));
        addAndMakeVisible(*(modulator_labels[i]));
        modulator_labels[i]->addMouseListener(this, true);
    }
    envelopes[visible_envelope]->setVisible(true);
    modulator_labels[visible_envelope]->setSelected(true);

    // addMouseListener(&active_envelope_listener, true);
}

ADSRParentComponent::~ADSRParentComponent() {

}

void ADSRParentComponent::resized() {
    auto area = getLocalBounds();
    auto bottom_bar = area.removeFromBottom(area.proportionOfHeight(0.1));
    for (auto& envelope : envelopes) {
        envelope->setBounds(area);
    }
    auto mod_label_width = area.proportionOfWidth(1.0/double(num_envelopes));
    for (auto& mod_label : modulator_labels) {
        mod_label->setBounds(bottom_bar.removeFromLeft(mod_label_width));
    }
}

void ADSRParentComponent::mouseDown (const MouseEvent& e) {
    if (auto comp = dynamic_cast<ModulatorLabel*>(e.eventComponent)) {
        envelopes[visible_envelope]->setVisible(false);
        modulator_labels[visible_envelope]->setSelected(false);
        visible_envelope = comp->getModID();
        envelopes[visible_envelope]->setVisible(true);
        modulator_labels[visible_envelope]->setSelected(true);
    }
}