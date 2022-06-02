#include "ADSRComponent.h"
#include "LabeledKnobComponent.h"
#include "ModulatorLabel.h"
#include "ADSRViewer.h"
#include "../matrix/Matrix.h"
#include "../modulators/ADSRModulator.h"

// #include "../managers/matrix/ADSRDefines.h"


ADSRComponent::ADSRComponent(Matrix* matrix, int mod_id) {

    auto idx = std::to_string(mod_id);
    auto atk_param_id = MODULATOR_PARAMS[mod_id][ADSR_PARAM::ATK];
    auto atk_tooltip = "Attack in (ms) of " + MODULATOR_NAMES[mod_id];
    auto atk_label = "ATTACK";
    attack_knob = std::make_unique<LabeledKnobComponent>(matrix, atk_param_id, atk_tooltip, atk_label);
    addAndMakeVisible(*attack_knob);

    auto dec_param_id = MODULATOR_PARAMS[mod_id][ADSR_PARAM::DEC];
    auto dec_tooltip = "Decay in (ms) of " + MODULATOR_NAMES[mod_id];
    auto dec_label = "DECAY";
    decay_knob = std::make_unique<LabeledKnobComponent>(matrix, dec_param_id, dec_tooltip, dec_label);
    addAndMakeVisible(*decay_knob);

    auto sus_param_id = MODULATOR_PARAMS[mod_id][ADSR_PARAM::SUS];
    auto sus_tooltip = "Sustain in (ms) of " + MODULATOR_NAMES[mod_id];
    auto sus_label = "SUSTAIN";
    sustain_knob = std::make_unique<LabeledKnobComponent>(matrix, sus_param_id, sus_tooltip, sus_label);
    addAndMakeVisible(*sustain_knob);

    auto rel_param_id = MODULATOR_PARAMS[mod_id][ADSR_PARAM::REL];
    auto rel_tooltip = "Release in (ms) of " + MODULATOR_NAMES[mod_id];
    auto rel_label = "RELEASE";
    release_knob = std::make_unique<LabeledKnobComponent>(matrix, rel_param_id, rel_tooltip, rel_label);
    addAndMakeVisible(*release_knob);

    adsr_viewer = std::make_unique<ADSRViewer>(matrix, mod_id);
    addAndMakeVisible(*adsr_viewer);    
}

// ADSRComponent::~ADSRComponent() {

// }

void ADSRComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::skyblue);
}

void ADSRComponent::resized() {
    auto bounds = getLocalBounds();
    adsr_viewer->setBounds(bounds.removeFromTop(proportionOfHeight(0.7f)));
    attack_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    decay_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    sustain_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    release_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
}

//==============================================================================
// ADSRParentComponent
//==============================================================================

ADSRParentComponent::ADSRParentComponent(Matrix* matrix, std::vector<int>& mod_ids) 
    : ModulatorParentComponent(matrix, mod_ids)
{
    init_child_components_and_visibility_attachment(matrix, mod_ids);
}

void ADSRParentComponent::init_child_components_and_visibility_attachment(Matrix* matrix, std::vector<int>& mod_ids)
{
    for (int i = 0; i < num_children; ++i) {
        child_components.push_back(std::make_unique<ADSRComponent>(matrix, mod_ids[i]));
        addChildComponent(*(child_components[i]));
    }

    visible_child_attachment = std::make_unique<juce::ParameterAttachment>(
        *(matrix->get_parameter(PARAM::ADSR_VISIBLE_ID)), 
        std::function<void (float)>([this](float x){set_visible(int(x));}), 
        matrix->getUndoManager()
    );
    set_visible(visible_child);
}