#include "LFOComponent.h"
#include "LabeledKnobComponent.h"
#include "ModulatorLabel.h"
#include "LFOViewer.h"
#include "../matrix/Matrix.h"
#include "../modulators/LFOModulator.h"

LFOComponent::LFOComponent(Matrix* matrix, int mod_id) {

    auto idx = std::to_string(mod_id);
    auto freq_param_id = MODULATOR_PARAMS[mod_id][LFO_PARAM::FREQ];
    auto freq_tooltip = "Frequency in (hz) of " + MODULATOR_NAMES[mod_id];
    auto freq_label = "FREQ";
    frequency_knob = std::make_unique<LabeledKnobComponent>(matrix, freq_param_id, freq_tooltip, freq_label);
    addAndMakeVisible(*frequency_knob);

    auto rise_param_id = MODULATOR_PARAMS[mod_id][LFO_PARAM::RISE];
    auto rise_tooltip = "Rise in (ms) of " + MODULATOR_NAMES[mod_id];
    auto rise_label = "RISE";
    rise_knob = std::make_unique<LabeledKnobComponent>(matrix, rise_param_id, rise_tooltip, rise_label);
    addAndMakeVisible(*rise_knob);

    lfo_viewer = std::make_unique<LFOViewer>(matrix, mod_id);
    addAndMakeVisible(*lfo_viewer);
}

LFOComponent::~LFOComponent() {
    
}

void LFOComponent::paint (juce::Graphics& g) {
    g.fillAll (juce::Colours::skyblue);
}

void LFOComponent::resized() {
    auto bounds = getLocalBounds();
    lfo_viewer->setBounds(bounds.removeFromTop(proportionOfHeight(0.7f)));
    frequency_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
    rise_knob->setBounds(bounds.removeFromLeft(proportionOfWidth(0.25f)));
}

//==============================================================================
// LFOParentComponent
//==============================================================================


// LFOParentComponent::LFOParentComponent(Matrix* m, std::vector<int>& mod_ids) 
//     : ModulatorParentComponent(m, mod_ids)
// {
//     init_child_components_and_visibility_attachment(m, mod_ids);
// }

// void LFOParentComponent::init_child_components_and_visibility_attachment(Matrix* m, std::vector<int>& mod_ids)
// {
//     for (int i = 0; i < num_children; ++i) {
//         child_components.push_back(std::make_unique<LFOComponent>(m, mod_ids[i]));
//         addChildComponent(*(child_components[i]));
//     }

//     // visible_child_attachment = std::make_unique<juce::ParameterAttachment>(
//     //     *(m->get_parameter(PARAM::LFO_VISIBLE_ID)), 
//     //     std::function<void (float)>([this](float x){set_visible(int(x));}), 
//     //     m->getUndoManager()
//     // );
//     set_visible(visible_child);
// }