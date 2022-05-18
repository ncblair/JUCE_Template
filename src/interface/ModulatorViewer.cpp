#include "ModulatorViewer.h"
#include "IconPropertySlider.h"
#include "../util/Modulator.h"
#include "../managers/matrix/Matrix.h"
#include "../audio/ADSR.h"


ViewerHandle::ViewerHandle(Matrix* m, int m_id, int p_id, int p_id_2) {
    mouse_hovering = false;
    matrix = m;
    mod_id = m_id;
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

bool ViewerHandle::has_second_parameter() {
    return param_id_2 != -1;
}

float ViewerHandle::get_mouse_x_pos_ms(const MouseEvent& e) {
    auto viewer_component = getParentComponent();
    auto range_start = float(bounds.getX());
    auto range = float(range_start + bounds.getWidth()) - range_start;
    auto position = e.getEventRelativeTo(viewer_component).getPosition();
    auto pos_in_range = float(position.getX()) - range_start;
    auto total_length_ms = 1000.0f * matrix->propertyValue(MODULATOR_PROPERTIES[mod_id][ZOOM]);
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
    normalized_value = std::max(std::min(normalized_value, 1.0f), 0.0f);
    auto parameter_range = PARAMETER_RANGES[param_id];
    if (has_second_parameter()) {
        parameter_range = PARAMETER_RANGES[param_id_2];
    }
    auto new_val = parameter_range.convertFrom0to1(1.0 - normalized_value);

    // std::cout << "Mouse Drag: " << position.getY() << " " << pos_in_range << " " <<  normalized_value << " " << new_val << std::endl;
    return new_val;
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

HorizontalHandle::HorizontalHandle(Matrix* m, int m_id, int p_id) : ViewerHandle(m, m_id, p_id) {
    
}

void HorizontalHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_x_pos_ms(e);
    param_attachment->setValueAsPartOfGesture(new_val);
}

VerticalCurveHandle::VerticalCurveHandle(Matrix* m, int m_id, int p_id) : ViewerHandle(m, m_id, p_id) {
    
}

void VerticalCurveHandle::mouseDrag (const MouseEvent& e) {
    auto new_val = get_mouse_y_pos_curve(e);
    param_attachment->setValueAsPartOfGesture(new_val);
}

FreeHandle::FreeHandle(Matrix* m, int m_id, int p_id, int p_id_2) : ViewerHandle(m, m_id, p_id, p_id_2) {
    
}

void FreeHandle::mouseDrag (const MouseEvent& e) {
    auto new_val_x = get_mouse_x_pos_ms(e);
    param_attachment->setValueAsPartOfGesture(new_val_x);

    if (has_second_parameter()) {
        auto new_val_y = get_mouse_y_pos_linear(e);
        param_attachment_2->setValueAsPartOfGesture(new_val_y);
    }
}

//============================================================================================

ModulatorViewer::ModulatorViewer(Matrix* m, int modulator_id) {
    matrix = m;
    mod_id = modulator_id;
    adsr = matrix->getModulator(mod_id);
    startTimerHz(FRAMERATE);

    // add handles
    handle_ids = {ATK_CURVE, ATK, DEC_CURVE, DEC, REL_CURVE, REL};
    handles.resize(NumADSRParams);

    handles[ATK_CURVE] = std::make_unique<VerticalCurveHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][ATK_CURVE]);
    handles[ATK] = std::make_unique<HorizontalHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][ATK]);
    handles[DEC_CURVE] = std::make_unique<VerticalCurveHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][DEC_CURVE]);
    // Calling the Decay/Sustain handle handles[DEC]
    handles[DEC] = std::make_unique<FreeHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][DEC], MODULATOR_PARAMS[modulator_id][SUS]);
    handles[REL_CURVE] = std::make_unique<VerticalCurveHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][REL_CURVE]);
    handles[REL] = std::make_unique<HorizontalHandle>(m, mod_id, MODULATOR_PARAMS[modulator_id][REL]);

    for (int i = 0; i < handle_ids.size(); ++i) {
        addAndMakeVisible(*(handles[handle_ids[i]]));
    }

    zoom_img = juce::ImageCache::getFromMemory(BinaryData::ZOOM_ICON_png, BinaryData::ZOOM_ICON_pngSize);
    zoom_slider = std::make_unique<IconPropertySlider>(m, MODULATOR_PROPERTIES[modulator_id][ZOOM], "ADSR Window Zoom", zoom_img);
    addAndMakeVisible(*zoom_slider);
}

ModulatorViewer::~ModulatorViewer() {

}

void ModulatorViewer::paint (juce::Graphics& g) {
    auto r = proportionOfWidth(HANDLE_RADIUS);
    auto m = proportionOfWidth(MARGIN);
    auto w = float(getWidth()) - 2.0f * m;
    auto h = float(getHeight()) - 2.0f * m;

    g.setColour (juce::Colours::white);
    g.fillRect (getLocalBounds());
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 2);
    g.fillRect(0, getHeight() - m, getWidth(), getHeight());

    auto total_length_ms = 1000.0f * matrix->propertyValue(MODULATOR_PROPERTIES[mod_id][ZOOM]);
    
    auto release_ms = adsr->get_parameter(ATK) + adsr->get_parameter(DEC);

    // position handles
    auto atk_x = float(w) * adsr->get_parameter(ATK) / total_length_ms + m;
    auto atk_y = m;
    auto atk_curve_x = (atk_x + m) * 0.5f;
    auto atk_curve_y = float(h) * (1.0f - adsr->get(adsr->get_parameter(ATK) * 0.5f, release_ms)) + m;
    auto dec_x = atk_x + float(w) * adsr->get_parameter(DEC) / total_length_ms;
    auto dec_y = float(h) * (1.0f - adsr->get_parameter(SUS)) + m;
    auto dec_curve_x = (dec_x + atk_x)*0.5f;
    auto dec_curve_y = float(h) * (1.0f - adsr->get((adsr->get_parameter(DEC) + 2.0 * adsr->get_parameter(ATK)) * 0.5f, release_ms)) + m;
    // if (dec_curve_y == 0.0f) {
    //     dec_curve_y = (dec_y + h + m) * 0.5f;
    // }
    auto rel_x = dec_x + float(w) * adsr->get_parameter(REL) / total_length_ms;
    auto rel_y = h + m;
    auto rel_curve_x = (rel_x + dec_x)*0.5f;
    auto rel_curve_y = float(h) * (1.0f - adsr->get((adsr->get_parameter(REL) + 2.0 * release_ms) * 0.5f, release_ms)) + m;

    auto max_attack_ms = float(PARAMETER_RANGES[ATK].getRange().getEnd());
    auto max_decay_ms = float(PARAMETER_RANGES[DEC].getRange().getEnd());
    auto max_release_ms = float(PARAMETER_RANGES[REL].getRange().getEnd());
    auto max_attack_x = w * (max_attack_ms / total_length_ms);
    auto max_decay_x = w * (max_decay_ms / total_length_ms);
    auto max_release_x = w * (max_release_ms / total_length_ms);

    // set handle position (top left)
    handles[ATK_CURVE]->setCentrePosition(atk_curve_x, atk_curve_y);
    handles[ATK]->setCentrePosition(atk_x, atk_y);
    handles[DEC_CURVE]->setCentrePosition(dec_curve_x, dec_curve_y );
    handles[DEC]->setCentrePosition(dec_x, dec_y);
    handles[REL_CURVE]->setCentrePosition(rel_curve_x, rel_curve_y);
    handles[REL]->setCentrePosition(rel_x, rel_y);

    for (int i = 0; i < handle_ids.size() - 1; ++i) {
        auto curve_handle = dynamic_cast<VerticalCurveHandle*>(handles[handle_ids[i]].get());
        if (curve_handle != nullptr && 
            (curve_handle->getX() == handles[handle_ids[i + 1]]->getX() ||
            curve_handle->getY() == handles[handle_ids[i + 1]]->getY())) {
                curve_handle->setVisible(false);
        }
        else if (handles[handle_ids[i]]->getX() > w + m) {
            handles[handle_ids[i]]->setVisible(false);
        }
        else {
            handles[handle_ids[i]]->setVisible(true);
        }
    }

    // constraints on mouse drag position
    handles[ATK_CURVE]->constrain(atk_curve_x, h + m, 0, -h);
    handles[ATK]->constrain(m, m, max_attack_x, 0);
    handles[DEC_CURVE]->constrain(dec_curve_x, m, 0, dec_y - m);
    handles[DEC]->constrain(atk_x, m, max_decay_x, h);
    handles[REL_CURVE]->constrain(rel_curve_x, dec_y, 0, h + m - dec_y);
    handles[REL]->constrain(dec_x, h + m, max_release_x, 0);

    // draw the waveform
    juce::Path p;
    g.setColour (juce::Colours::black);
    p.startNewSubPath(m, h + m);

    int h_id = 0;
    for (float ms = 0; ms < total_length_ms; ms += total_length_ms / PRECISION) {
        auto x_pos = w * ms / total_length_ms;
        while (h_id < handle_ids.size() && 
                handles[handle_ids[h_id]]->isVisible() && 
                x_pos + m >= handles[handle_ids[h_id]]->getX() + r) {
            p.lineTo(handles[handle_ids[h_id]]->getX() + r, handles[handle_ids[h_id]]->getY() + r);
            h_id++;
        }
        if (h_id >= handle_ids.size()) {
            break;
        }
        else {
            p.lineTo(x_pos + m, m + h - float(h) * adsr->get(ms, release_ms));
        }
    }

    g.strokePath(p, juce::PathStrokeType(2));
}

void ModulatorViewer::resized() {
    auto diameter = 2.0f * proportionOfWidth(HANDLE_RADIUS);
    for (int i = 0; i < handle_ids.size(); ++i) {
        handles[handle_ids[i]]->setSize(diameter, diameter);
    }
    auto area = getLocalBounds();
    auto top = area.removeFromTop(proportionOfHeight(0.2));
    auto topright = top.removeFromRight(proportionOfHeight(0.2));
    zoom_slider->setBounds(topright);
}

void ModulatorViewer::timerCallback() {
    repaint();
}
