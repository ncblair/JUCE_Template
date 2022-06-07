#include "LabeledKnobComponent.h"
// #include "ModulatorLabel.h"
#include "../matrix/Matrix.h"

LabeledKnobComponent::LabeledKnobComponent(Matrix* m, int p_id, juce::String tooltip, juce::String label_str) {
    matrix = m;
    param_id = p_id;
    tooltip_text = tooltip;
    auto param_name = PARAMETER_NAMES[param_id];

    addAndMakeVisible(knob);
    knob.setName(param_name);
    knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    // knob.setTooltip(param_name + ": " + tooltip);
    // knob.setInterceptsMouseClicks(false, false);
    knob.addMouseListener(this, true);
    knob.onValueChange = [this](){repaint();};
    
    
    knob_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*(matrix->getParamTree()), param_name, knob);

    addAndMakeVisible(label);
    label.setText (label_str, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.addMouseListener(this, true);

    addAndMakeVisible(mod_knob);
    mod_knob.setName(param_name + " modulation depth knob (No Modulators)"); 
    mod_knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    // mod_knob.addListener(this);
    mod_knob.setRange (-1, 1, 0.01);
    mod_knob.setDoubleClickReturnValue(true, 0.3);
    mod_knob.addMouseListener(this, true);

    // value_popup_box = std::make_unique<juce::CallOutBox>(value_popup, getScreenBounds(), nullptr);
    // addAndMakeVisible(&value_popup_box);

    // addChildComponent(value_popup);
    // value_popup.setJustificationType(juce::Justification::centred);
    // label.setInterceptsMouseClicks(false, false);
    matrix->addParameterListener(PARAM::SELECTED_MODULATOR_ID, this);
    matrix_tree = matrix->getMatrix();
    matrix_tree.addListener(this);
    num_modulators_changed();
    // std::cout << "end knob constructor " << std::endl;
}

LabeledKnobComponent::~LabeledKnobComponent() {
    matrix->removeParameterListener(PARAM::SELECTED_MODULATOR_ID, this);
}

void LabeledKnobComponent::paint(juce::Graphics& g) {
    if (mod_knob.isEnabled()) {
        juce::Path p;
        g.setColour (juce::Colours::blue);
        auto normalized_knob_value = knob.valueToProportionOfLength(knob.getValue())*2.0 - 1.0;
        auto min_radians = 5 * M_PI / 4.0;
        auto max_radians = 11 * M_PI / 4.0;
        auto start_radians = 2.0 * M_PI + normalized_knob_value * 3.0 * M_PI / 4.0;
        start_radians = std::max(std::min(start_radians, max_radians), min_radians);
        auto end_radians = start_radians + 6.0 * M_PI * mod_knob.getValue() / 4.0;
        end_radians = std::max(std::min(end_radians, max_radians), min_radians);

        auto w = knob.getWidth();
        auto h = knob.getHeight();
        auto r = std::min(w, h) * 0.43;
        p.addCentredArc(knob.getX() + w/2.0, knob.getY() + h/2.0, r, r, 0.0f, start_radians, end_radians, true);
        g.strokePath(p, juce::PathStrokeType(2));
    }
}

void LabeledKnobComponent::resized() {
    auto area = getLocalBounds();
    mod_knob.setBoundsRelative(0, 0, 0.342f, 0.342f);
    knob.setBounds(area.removeFromTop(proportionOfHeight(0.8f)).reduced(proportionOfHeight(0.08f)));
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
            int mod_id = mod_node.getProperty(Matrix::MOD_ID_ID);
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
                    matrix->reset_parameter(param_id);
                }
                else if (result == 2) {
                    // reset all modulators
                    matrix->disconnect_all(param_id);
                    // auto num_children = modulators.getNumChildren();
                    // while (modulators.getNumChildren()) {
                    //     auto mod_node = modulators.getChild(0);
                    //     int mod_id = mod_node.getProperty("MOD_ID");
                    //     matrix->disconnect(mod_id, param_id);
                    // }
                }
                else {
                    // auto num_children = modulators.getNumChildren();
                    auto mod_node = modulators.getChild(result - 3);
                    int mod_id = mod_node.getProperty(Matrix::MOD_ID_ID);
                    matrix->disconnect(mod_id, param_id);
                }
            }
        );
    }
}

void LabeledKnobComponent::mouseUp(const MouseEvent& e) {
    // value_popup_box.dismiss();
}

// void LabeledKnobComponent::sliderValueChanged (Slider* slider) {
//     if (slider == &mod_knob) {
//         // mod knob changed
//         // std::cout << "MOD KNOB CHANGE" << std::endl;

//     }
// }

void LabeledKnobComponent::parameterChanged (const juce::String &parameterID, float newValue) {
    // std::cout << "parameter changed " << parameterID << " " << newValue << std::endl;
    if (parameterID == PARAMETER_NAMES[PARAM::SELECTED_MODULATOR_ID]) {
        redirect_mod_knob(newValue);
        repaint();
    }
}

void LabeledKnobComponent::valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) {
    if (property == Matrix::MOD_DEPTH_ID && treeWhosePropertyHasChanged.getRoot() == matrix_tree && treeWhosePropertyHasChanged.getParent().hasType(PARAMETER_IDS[param_id])) {
        repaint();
    }
}
void LabeledKnobComponent::valueTreeChildAdded (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenAdded) {
    if (parentTree.getRoot() == matrix_tree && (parentTree.hasType(PARAMETER_IDS[param_id]) || childWhichHasBeenAdded.hasType(PARAMETER_IDS[param_id]))) {
        num_modulators_changed();
    }
}
void LabeledKnobComponent::valueTreeChildRemoved (juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) {
    if (parentTree.getRoot() == matrix_tree && (parentTree.hasType(PARAMETER_IDS[param_id]) || childWhichHasBeenRemoved.hasType(PARAMETER_IDS[param_id]))) {
        num_modulators_changed();
    }
}

void LabeledKnobComponent::redirect_mod_knob(int mod_id) {
    // TODO: called on Load, add/remove connection, select different modulator (not on propertyChange)
    // std::cout << "mod knob redirected: " << MODULATOR_NAMES[mod_id] << " " << PARAMETER_NAMES[param_id] << std::endl;
    if (matrix->is_connected(mod_id, param_id)) {
        mod_knob.setEnabled(true);
        mod_knob.setName(PARAMETER_NAMES[param_id] + " " + MODULATOR_NAMES[mod_id] + " DEPTH");
        auto depth_value = matrix->get_mod_depth(mod_id, param_id); // Get Depth Value from Matrix
        mod_knob.getValueObject().referTo(depth_value);
    }
    else {
        mod_knob.setEnabled(false);
        mod_knob.getValueObject().referTo(juce::Value(juce::var(0.0f)));
        mod_knob.setName(PARAMETER_NAMES[param_id] + " modulation depth knob (No Modulators)");  
    }
}

void LabeledKnobComponent::reset_tooltip() {
    auto tooltip_string = tooltip_text;
    auto modulators = matrix->getModulators(param_id);
    auto num_mods = modulators.getNumChildren();
    if (num_mods > 0) {
       tooltip_string += " | modulated by: ";
    }
    for (int i = 0; i < num_mods - 1; ++i) {
        auto mod_node = modulators.getChild(i);
        int mod_id = mod_node.getProperty(Matrix::MOD_ID_ID);
        tooltip_string += MODULATOR_NAMES[mod_id] + ", ";
    }
    tooltip_string += MODULATOR_NAMES[modulators.getChild(num_mods - 1).getProperty(Matrix::MOD_ID_ID)];
    knob.setTooltip(tooltip_string);
    // mod_knob.setTooltip(tooltip_string);
}

void LabeledKnobComponent::num_modulators_changed() {
    redirect_mod_knob(matrix->paramValue(PARAM::SELECTED_MODULATOR_ID));
    reset_tooltip();
    repaint();
}