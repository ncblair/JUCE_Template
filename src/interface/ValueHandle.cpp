// #include "ModulatorViewer.h"
#include "ValueHandle.h"
// #include "IconSlider.h"
#include "../matrix/Matrix.h"
// #include "../modulators/ADSRModulator.h"
// #include "../modulators/NoteState.h"

ValueHandle::ValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1, const juce::Identifier& value_id2) 
    : id1(value_id1), id2(value_id2)
{
    mouse_hovering = false;
    undo_manager = undomanager;
    value_tree = tree;
}

void ValueHandle::mouseDown (const MouseEvent& e) {
    // param_attachment->beginGesture();
    // if (has_second_parameter()) {
    //     param_attachment_2->beginGesture();
    // }
}

void ValueHandle::mouseUp (const MouseEvent& e) {
    // param_attachment->endGesture();
    // if (has_second_parameter()) {
    //     param_attachment_2->endGesture();
    // }
}

void ValueHandle::mouseEnter (const MouseEvent& e) {
    mouse_hovering = true;
    repaint();
}

void ValueHandle::mouseExit (const MouseEvent& e) {
    mouse_hovering = false;
    repaint();
}

void ValueHandle::paint (juce::Graphics& g) {
    g.setColour (juce::Colours::black);
    auto local = getLocalBounds();
    auto area = juce::Rectangle<float>(local.getX(), local.getY(), local.getWidth(), local.getHeight());
    if (mouse_hovering) {
        g.drawEllipse(area, 1);
    }
    g.fillEllipse(area * 0.3f + juce::Point<float>(area.proportionOfWidth(0.35f), area.proportionOfHeight(0.35f)));
    // g.fillEllipse(area.getX(), area.getY(), area.getWidth(), area.getHeight());
}

void ValueHandle::constrain(float x, float y, float w, float h) {
    bounds = juce::Rectangle<float>(x, y, w, h);
}


bool ValueHandle::has_second_value() {
    return id2 != null_id;
}

float ValueHandle::get_mouse_x_pos_linear(const MouseEvent& e) {
    auto parent_width = float(getParentComponent()->getWidth());
    auto x_pos = float(e.getEventRelativeTo(getParentComponent()).getPosition().getX());
    auto bounds_x_min = float(bounds.getX());
    auto bounds_x_max = float(bounds_x_min + bounds.getWidth());
    auto constrained = std::max(std::min(x_pos, bounds_x_max), bounds_x_min);
    auto normalized_value = constrained / parent_width;
    return normalized_value;
}

float ValueHandle::get_mouse_y_pos_linear(const MouseEvent& e) {
    auto parent_height = float(getParentComponent()->getHeight());
    auto y_pos = float(e.getEventRelativeTo(getParentComponent()).getPosition().getY());
    auto bounds_y_min = float(bounds.getY());
    auto bounds_y_max = float(bounds_y_min + bounds.getHeight());
    auto constrained = std::max(std::min(y_pos, bounds_y_max), bounds_y_min);
    auto normalized_value = 1.0f - constrained / parent_height;
    return normalized_value;
}

float ValueHandle::get_mouse_y_pos_curve(const MouseEvent& e) {
    auto normed_value = std::max(get_mouse_y_pos_linear(e), 0.0001f);
    normed_value = curve_range.snapToLegalValue(-1.4427f * log(normed_value));
    return normed_value;
}

VerticalValueHandle::VerticalValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1) 
    : ValueHandle(tree, undomanager, value_id1, juce::Identifier("null"))
{

}

void VerticalValueHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_y_pos_linear(e);
    value_tree.setProperty(id1, new_val, undo_manager);
}

VerticalCurveValueHandle::VerticalCurveValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1) 
    : ValueHandle(tree, undomanager, value_id1, juce::Identifier("null"))
{
    
}

void VerticalCurveValueHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_y_pos_curve(e);
    value_tree.setProperty(id1, new_val, undo_manager);
}

FreeValueHandle::FreeValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1, const juce::Identifier& value_id2) 
    : ValueHandle(tree, undomanager, value_id1, value_id2)
{
    
}

void FreeValueHandle::mouseDrag (const MouseEvent& e) {
    auto new_val_x = get_mouse_x_pos_linear(e);
    value_tree.setProperty(id1, new_val_x, undo_manager);

    if (has_second_value()) {
        auto new_val_y = get_mouse_y_pos_linear(e);
        value_tree.setProperty(id2, new_val_y, undo_manager);
    }
}

void FreeValueHandle::mouseDoubleClick (const MouseEvent& e) {
    value_tree.getParent().removeChild(value_tree, undo_manager);
}