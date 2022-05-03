#include "LabeledKnobComponent.h"
// #include "ModulatorLabel.h"
#include "../managers/matrix/Matrix.h"

LabeledKnobComponent::LabeledKnobComponent(Matrix* m, int p_id, juce::String tooltip, juce::String label_str) {
    matrix = m;
    param_id = p_id;
    addAndMakeVisible(knob);
    knob.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    knob.setTooltip(tooltip);
    // knob.setInterceptsMouseClicks(false, false);

    auto param_name = PARAMETER_NAMES[p_id];
    knob_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*(matrix->getParamTree()), param_name, knob);
    addAndMakeVisible(label);
    label.setText (label_str, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setInterceptsMouseClicks(false, false);
}

void LabeledKnobComponent::resized() {
    knob.setBounds(getLocalBounds());
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
}