#include "LabeledKnobComponent.h"
// #include "ModulatorLabel.h"
#include "../managers/matrix/Matrix.h"

LabeledKnobComponent::LabeledKnobComponent(Matrix* m, int p_id, juce::String tooltip, juce::String label_str) {
    matrix = m;
    param_id = p_id;
    auto param_name = PARAMETER_NAMES[param_id];

    addAndMakeVisible(knob);
    knob.setName(param_name);
    knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    knob.setTooltip(tooltip);
    // knob.setInterceptsMouseClicks(false, false);
    knob.addMouseListener(this, true);

    knob_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*(matrix->getParamTree()), param_name, knob);
    addAndMakeVisible(label);
    label.setText (label_str, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.addMouseListener(this, true);

    // value_popup_box = std::make_unique<juce::CallOutBox>(value_popup, getScreenBounds(), nullptr);
    // addAndMakeVisible(&value_popup_box);

    // addChildComponent(value_popup);
    // value_popup.setJustificationType(juce::Justification::centred);
    // label.setInterceptsMouseClicks(false, false);
}

void LabeledKnobComponent::resized() {
    auto area = getLocalBounds();
    knob.setBounds(area.removeFromTop(proportionOfHeight(0.8f)));
    label.setBounds(area);
    // value_popup.setBounds()
}

bool LabeledKnobComponent::isInterestedInDragSource (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) {
    return true;
}

void LabeledKnobComponent::itemDropped (const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) {
    std::cout << "Connect" << PARAMETER_NAMES[param_id] << " to " << MODULATOR_NAMES[int(dragSourceDetails.description)] << std::endl;
    matrix->connect(int(dragSourceDetails.description), param_id, matrix->DEFAULT_MOD_DEPTH);
}

void LabeledKnobComponent::mouseDown (const MouseEvent& e) {
    std::cout << PARAMETER_NAMES[param_id] << "Labeled Knob mouseDown()" << std::endl;
    if(e.mods.isLeftButtonDown())
    {
        // // left mouse click
        // if (e.originalComponent == &knob) {
        //     // left click on knob
        //     value_popup->setText(
        //         PARAMETER_NAMES[param_id] + ": " + knob.getTextFromValue(knob.getValue()),
        //         juce::dontSendNotification
        //     );
        //     // value_popup.setVisible(true);
        //     // value_popup->setSize(100, 100);
        // }

    }else if(e.mods.isRightButtonDown())
    {
        // right mouse click
        right_click_menu.clear();
        right_click_menu.addItem(1,"reset",true);
        right_click_menu.addItem(2,"disconnect all modulators",true);

        juce::ValueTree modulators = matrix->getModulators(param_id);
        auto num_mods = modulators.getNumChildren();
        for (int i = 0; i < num_mods; ++i) {
            auto mod_node = modulators.getChild(i);
            int mod_id = mod_node.getProperty("MOD_ID");
            right_click_menu.addItem(i + 3, "disconnect " + MODULATOR_NAMES[mod_id],true);
        }
        
        right_click_menu.showMenuAsync (PopupMenu::Options(),
            [this, modulators] (int result)
            {
                std::cout << result << " RESULT" << std::endl;
                if (result == 0) {
                    // user dismissed the menu without picking anything
                }
                else if (result == 1) {
                    // reset
                }
                else if (result == 2) {
                    // reset all modulators
                    auto num_children = modulators.getNumChildren();
                    while (modulators.getNumChildren()) {
                        auto mod_node = modulators.getChild(0);
                        int mod_id = mod_node.getProperty("MOD_ID");
                        matrix->disconnect(mod_id, param_id);
                    }
                }
                else {
                    // auto num_children = modulators.getNumChildren();
                    auto mod_node = modulators.getChild(result - 3);
                    int mod_id = mod_node.getProperty("MOD_ID");
                    matrix->disconnect(mod_id, param_id);
                }
            }
        );
    }
}

void LabeledKnobComponent::mouseUp(const MouseEvent& e) {
    // value_popup_box.dismiss();
}