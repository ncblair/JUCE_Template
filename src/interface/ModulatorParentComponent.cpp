#include "ModulatorParentComponent.h"
#include "ModulatorLabel.h"
#include "../matrix/Matrix.h"

//==============================================================================
// ModulatorParentComponent
//==============================================================================

ModulatorParentComponent::ModulatorParentComponent(Matrix* matrix, std::vector<int>& mod_ids) {
    num_children = mod_ids.size();

    for (int i = 0; i < num_children; ++i) {
        auto m_id = mod_ids[i];
        mod_id_to_index[m_id] = i;
        
        modulator_labels.push_back(std::make_unique<ModulatorLabel>(
            matrix, 
            m_id,
            "Draggable Component for Modulator " + MODULATOR_NAMES[m_id], 
            MODULATOR_NAMES[m_id]
        ));
        addAndMakeVisible(*(modulator_labels[i]));
        modulator_labels[i]->addMouseListener(this, true);
    }
}

ModulatorParentComponent::~ModulatorParentComponent() {

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
    if (auto comp = dynamic_cast<ModulatorLabel*>(e.eventComponent)) {
        visible_child_attachment->setValueAsPartOfGesture(mod_id_to_index[comp->getModID()]);
    }
}

void ModulatorParentComponent::set_visible(int child_id) {
    child_components[visible_child]->setVisible(false);
    modulator_labels[visible_child]->setSelected(false);
    visible_child = child_id;
    child_components[visible_child]->setVisible(true);
    modulator_labels[visible_child]->setSelected(true);
}