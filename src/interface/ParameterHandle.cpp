// #include "ModulatorViewer.h"
#include "ParameterHandle.h"
// #include "IconSlider.h"
#include "../matrix/Matrix.h"
// #include "../modulators/ADSRModulator.h"
// #include "../modulators/NoteState.h"

ViewerHandle::ViewerHandle(Matrix* m, int p_id, int p_id_2) {
    mouse_hovering = false;
    matrix = m;
    param_id = p_id;
    param_attachment = std::make_unique<juce::ParameterAttachment>(
        *(matrix->getParamTree()->getParameter(PARAMETER_NAMES[param_id])), 
        std::function<void (float)>([this](float x){}), 
        matrix->getUndoManager()
    );

    param_id_2 = p_id_2;
    if (has_second_parameter()) {
        param_attachment_2 = std::make_unique<juce::ParameterAttachment>(
            *(matrix->getParamTree()->getParameter(PARAMETER_NAMES[param_id_2])), 
            std::function<void (float)>([this](float x){}),
            matrix->getUndoManager()
        );
    }
}

void ViewerHandle::mouseDown (const MouseEvent& e) {
    param_attachment->beginGesture();
    if (has_second_parameter()) {
        param_attachment_2->beginGesture();
    }
}

void ViewerHandle::mouseUp (const MouseEvent& e) {
    param_attachment->endGesture();
    if (has_second_parameter()) {
        param_attachment_2->endGesture();
    }
}

void ViewerHandle::mouseEnter (const MouseEvent& e) {
    mouse_hovering = true;
}

void ViewerHandle::mouseExit (const MouseEvent& e) {
    mouse_hovering = false;
}

void ViewerHandle::mouseDoubleClick (const MouseEvent& e) {
    param_attachment->setValueAsCompleteGesture(PARAMETER_DEFAULTS[param_id]);
    if (has_second_parameter()) {
        param_attachment_2->setValueAsCompleteGesture(PARAMETER_DEFAULTS[param_id_2]);
    }
}

void ViewerHandle::paint (juce::Graphics& g) {
    g.setColour (juce::Colours::black);
    auto local = getLocalBounds();
    auto area = juce::Rectangle<float>(local.getX(), local.getY(), local.getWidth(), local.getHeight());
    if (mouse_hovering) {
        g.drawEllipse(area, 1);
    }
    g.fillEllipse(area * 0.3f + juce::Point<float>(area.proportionOfWidth(0.35f), area.proportionOfHeight(0.35f)));
    // g.fillEllipse(area.getX(), area.getY(), area.getWidth(), area.getHeight());
}

void ViewerHandle::constrain(float x, float y, float w, float h) {
    bounds = juce::Rectangle<float>(x, y, w, h);
}

void ViewerHandle::set_total_length_ms(double length_ms) {
    total_length_ms = length_ms;
}

bool ViewerHandle::has_second_parameter() {
    return param_id_2 != -1;
}

float ViewerHandle::get_mouse_x_pos_ms(const MouseEvent& e) {
    auto viewer_component = getParentComponent();
    auto range_start = float(bounds.getX());
    auto range = float(range_start + bounds.getWidth()) - range_start;
    auto position = e.getEventRelativeTo(viewer_component).getPosition();
    auto pos_in_range = float(position.getX()) - range_start;
    // auto total_length_ms = 1000.0f * matrix->paramValue(MODULATOR_PARAMS[mod_id][ADSR_PARAM::ZOOM]);
    auto ms_per_pixel = total_length_ms / viewer_component->getWidth();
    auto new_val = PARAMETER_RANGES[param_id].snapToLegalValue(pos_in_range * ms_per_pixel);
    return new_val;
}

float ViewerHandle::get_mouse_y_pos_linear(const MouseEvent& e) {
    auto range_start = float(bounds.getY());
    auto range = float(range_start + bounds.getHeight()) - range_start;
    auto position = e.getEventRelativeTo(getParentComponent()).getPosition();
    auto pos_in_range = float(position.getY()) - range_start;
    auto normalized_value = pos_in_range / range;
    normalized_value = 1.0 - std::max(std::min(normalized_value, 1.0f), 0.0f);
    auto parameter_norm_range = PARAMETER_RANGES[param_id];
    if (has_second_parameter()) {
        parameter_norm_range = PARAMETER_RANGES[param_id_2];
    }
    auto parameter_range = parameter_norm_range.getRange();
    auto new_val = normalized_value * parameter_range.getLength() + parameter_range.getStart();
    return new_val; // TODO? snap to legal value? (i don't think we have to as long as normalized value is good)
    // auto new_val = parameter_range.convertFrom0to1(1.0 - normalized_value);

    // // std::cout << "Mouse Drag: " << position.getY() << " " << pos_in_range << " " <<  normalized_value << " " << new_val << std::endl;
    // return new_val;
}

float ViewerHandle::get_mouse_y_pos_curve(const MouseEvent& e) {
    auto range_start = float(bounds.getY());
    auto range = float(range_start + bounds.getHeight()) - range_start;
    auto position = e.getEventRelativeTo(getParentComponent()).getPosition();
    auto pos_in_range = float(position.getY()) - range_start;
    auto normalized_value = pos_in_range / range;
    normalized_value = std::max(std::min(normalized_value, 1.0f), 0.0001f);
    auto parameter_range = PARAMETER_RANGES[param_id];
    if (has_second_parameter()) {
        parameter_range = PARAMETER_RANGES[param_id_2];
    }
    // solve an inverse exponential function
    auto new_val = parameter_range.snapToLegalValue(-1.4427f * log(normalized_value));

    return new_val;
}

HorizontalHandle::HorizontalHandle(Matrix* m, int p_id) : ViewerHandle(m, p_id) {
    
}

void HorizontalHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_x_pos_ms(e);
    param_attachment->setValueAsPartOfGesture(new_val);
}

VerticalCurveHandle::VerticalCurveHandle(Matrix* m, int p_id) : ViewerHandle(m, p_id) {
    
}

void VerticalCurveHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_y_pos_curve(e);
    param_attachment->setValueAsPartOfGesture(new_val);
}

FreeHandle::FreeHandle(Matrix* m, int p_id, int p_id_2) : ViewerHandle(m, p_id, p_id_2) {
    
}

void FreeHandle::mouseDrag (const MouseEvent& e) {
    auto new_val_x = get_mouse_x_pos_ms(e);
    param_attachment->setValueAsPartOfGesture(new_val_x);

    if (has_second_parameter()) {
        auto new_val_y = get_mouse_y_pos_linear(e);
        param_attachment_2->setValueAsPartOfGesture(new_val_y);
    }
}
