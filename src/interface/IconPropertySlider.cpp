#include "IconPropertySlider.h"
#include "../managers/matrix/Matrix.h"

IconPropertySlider::IconPropertySlider(Matrix* m, int p_id, juce::String tooltip, const juce::Image& icon) {
    matrix = m;
    property_id = p_id;
    // auto param_name = PROPERTY_NAMES[param_id];
    setImage(icon);
    setTooltip(tooltip);
    property_attachment = std::make_unique<juce::ParameterAttachment>(
        *(matrix->getPropertyTree()->getParameter(PROPERTY_NAMES[property_id])), 
        std::function<void (float)>([this](float x){}), 
        matrix->getUndoManager()
    );
}

void IconPropertySlider::mouseDown (const MouseEvent& e) { 
    property_attachment->beginGesture();
    last_position = e.getPosition().getY();
}
void IconPropertySlider::mouseUp (const MouseEvent& e) {
    property_attachment->endGesture();
    // prev_value = matrix->propertyValue(property_id);
}
void IconPropertySlider::mouseDrag (const MouseEvent& e) {
    // auto screen_height = float(getScreenBounds().getHeight());
    // auto distance = float(e.getDistanceFromDragStartY());
    auto position = float(e.getPosition().getY());
    auto distance = position - last_position;
    last_position = position;
    auto normalized_distance = distance * MOUSE_SPEED;
    auto property_range = PROPERTY_RANGES[property_id];
    auto current_norm_value = property_range.convertTo0to1(matrix->propertyValue(property_id));
    auto normalized_value = std::max(std::min(current_norm_value + normalized_distance, 1.0f), 0.0f);
    auto new_val = property_range.convertFrom0to1(normalized_value);
    property_attachment->setValueAsPartOfGesture(new_val);
}
// void IconPropertySlider::mouseEnter (const MouseEvent& e) {

// }
// void IconPropertySlider::mouseExit (const MouseEvent& e) {

// }
void IconPropertySlider::mouseDoubleClick (const MouseEvent& e) {
    property_attachment->setValueAsCompleteGesture(PROPERTY_DEFAULTS[property_id]);
}