#include "IconSlider.h"
#include "../matrix/Matrix.h"

IconSlider::IconSlider(Matrix* m, int p_id, juce::String tooltip, const juce::Image& icon) {
    matrix = m;
    parameter_id = p_id;
    setImage(icon);
    setTooltip(tooltip);
    parameter_attachment = std::make_unique<juce::ParameterAttachment>(
        *(matrix->get_parameter(parameter_id)), 
        std::function<void (float)>([this](float x){}), 
        matrix->getUndoManager()
    );
}

void IconSlider::mouseDown (const MouseEvent& e) { 
    parameter_attachment->beginGesture();
    last_position = e.getPosition().getY();
}
void IconSlider::mouseUp (const MouseEvent& e) {
    parameter_attachment->endGesture();
}
void IconSlider::mouseDrag (const MouseEvent& e) {
    auto position = float(e.getPosition().getY());
    auto distance = position - last_position;
    last_position = position;
    auto normalized_distance = distance * MOUSE_SPEED;
    auto parameter_range = PARAMETER_RANGES[parameter_id];
    auto current_norm_value = parameter_range.convertTo0to1(matrix->paramValue(parameter_id));
    auto normalized_value = std::max(std::min(current_norm_value + normalized_distance, 1.0f), 0.0f);
    auto new_val = parameter_range.convertFrom0to1(normalized_value);
    parameter_attachment->setValueAsPartOfGesture(new_val);
}
// void IconSlider::mouseEnter (const MouseEvent& e) {

// }
// void IconSlider::mouseExit (const MouseEvent& e) {

// }
void IconSlider::mouseDoubleClick (const MouseEvent& e) {
    parameter_attachment->setValueAsCompleteGesture(PARAMETER_DEFAULTS[parameter_id]);
}