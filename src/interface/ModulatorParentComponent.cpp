#include "ModulatorParentComponent.h"
#include "ModulatorLabel.h"
#include "../matrix/Matrix.h"
#include "LFOComponent.h"
#include "ADSRComponent.h"

//==============================================================================
// ModulatorParentComponent
//==============================================================================

ModulatorParentComponent::ModulatorParentComponent(Matrix* m, std::vector<int>& mod_ids) {
    matrix = m;

    num_children = mod_ids.size();

    for (int i = 0; i < num_children; ++i) {
        auto m_id = mod_ids[i];
        mod_id_to_index[m_id] = i;

        if (MODULATOR_TYPES[m_id] == MOD_TYPES::LFO_MOD) {
            child_components.push_back(std::make_unique<LFOComponent>(m, mod_ids[i]));
        } 
        else if (MODULATOR_TYPES[m_id] == MOD_TYPES::ADSR_MOD) {
            child_components.push_back(std::make_unique<ADSRComponent>(m, mod_ids[i]));
        }

        addChildComponent(*(child_components[i]));
        
        modulator_labels.push_back(std::make_unique<ModulatorLabel>(
            matrix, 
            m_id,
            "Draggable Component for Modulator " + MODULATOR_NAMES[m_id], 
            MODULATOR_NAMES[m_id]
        ));
        addAndMakeVisible(*(modulator_labels[i]));
        modulator_labels[i]->addMouseListener(this, true);
    }
    matrix->addParameterListener(PARAM::SELECTED_MODULATOR_ID, this);
    auto mod_type = MODULATOR_TYPES[mod_ids[visible_child]];
    if (mod_type == MOD_TYPES::LFO_MOD) {
        parameterChanged(PARAMETER_NAMES[PARAM::SELECTED_MODULATOR_ID], matrix->paramValue(PARAM::LFO_VISIBLE_ID));
    }
    else if (mod_type == MOD_TYPES::ADSR_MOD) {
        parameterChanged(PARAMETER_NAMES[PARAM::SELECTED_MODULATOR_ID], matrix->paramValue(PARAM::ADSR_VISIBLE_ID));
    }
    parameterChanged(PARAMETER_NAMES[PARAM::SELECTED_MODULATOR_ID], matrix->paramValue(PARAM::SELECTED_MODULATOR_ID));
}

ModulatorParentComponent::~ModulatorParentComponent() {
    matrix->removeParameterListener(PARAM::SELECTED_MODULATOR_ID, this);
}

void ModulatorParentComponent::resized() {
    auto area = getLocalBounds();
    auto bottom_bar = area.removeFromBottom(area.proportionOfHeight(0.1));
    for (auto& mod_comp : child_components) {
        mod_comp->setBounds(area);
    }
    auto mod_label_width = area.proportionOfWidth(1.0/double(num_children));
    for (auto& mod_label : modulator_labels) {
        mod_label->setBounds(bottom_bar.removeFromLeft(mod_label_width));
    }
}

void ModulatorParentComponent::mouseDown (const MouseEvent& e) {
    // TODO: lets get rid of this visible_child_attachment and just use matrix
    if (auto comp = dynamic_cast<ModulatorLabel*>(e.eventComponent)) {
        // visible_child_attachment->setValueAsPartOfGesture(mod_id_to_index[comp->getModID()]);
        matrix->set_parameter(PARAM::SELECTED_MODULATOR_ID, comp->getModID());
    }
}

// void ModulatorParentComponent::set_visible(int child_id) {
    // child_components[visible_child]->setVisible(ModulatorLabel::SELECTION::NO);
    // modulator_labels[visible_child]->setSelected(ModulatorLabel::SELECTION::NO);
    // visible_child = child_id;
    // child_components[visible_child]->setVisible(ModulatorLabel::SELECTION::YES_GLOBAL);
    // modulator_labels[visible_child]->setSelected(ModulatorLabel::SELECTION::YES_GLOBAL);
// }

void ModulatorParentComponent::parameterChanged (const juce::String &parameterID, float newValue) {
    if (parameterID == PARAMETER_NAMES[PARAM::SELECTED_MODULATOR_ID]) {
        // std::cout << "Selected Modulator Changed " << std::endl;
        if (mod_id_to_index.count(newValue) > 0) { // this class manages this mod_id
            // selected a new child within the same class
            child_components[visible_child]->setVisible(false);
            modulator_labels[visible_child]->setSelected(ModulatorLabel::SELECTION::NO);
            visible_child = mod_id_to_index[newValue];
            child_components[visible_child]->setVisible(true);
            modulator_labels[visible_child]->setSelected(ModulatorLabel::SELECTION::YES_GLOBAL);
            if (MODULATOR_TYPES[newValue] == MOD_TYPES::LFO_MOD) {
                matrix->set_parameter(PARAM::LFO_VISIBLE_ID, visible_child);
            }
            else if (MODULATOR_TYPES[newValue] == MOD_TYPES::ADSR_MOD) {
                matrix->set_parameter(PARAM::ADSR_VISIBLE_ID, visible_child);
            }
        }
        else {
            // selected a new child in a different class
            child_components[visible_child]->setVisible(true);
            modulator_labels[visible_child]->setSelected(ModulatorLabel::SELECTION::YES_LOCAL);
        }
        // std::cout << "end mod selection changed" << std::endl;
    }
}