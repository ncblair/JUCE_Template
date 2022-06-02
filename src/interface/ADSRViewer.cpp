#include "ADSRViewer.h"
#include "ParameterHandle.h"
#include "IconSlider.h"
#include "../matrix/Matrix.h"
#include "../modulators/ADSRModulator.h"
#include "../modulators/NoteState.h"

//============================================================================================

ADSRViewer::ADSRViewer(Matrix* m, int modulator_id) {
    matrix = m;
    mod_id = modulator_id;
    adsr = matrix->getModulator(mod_id);
    startTimerHz(FRAMERATE);

    // add handles
    handle_ids = {ADSR_PARAM::ATK_CURVE, ADSR_PARAM::ATK, ADSR_PARAM::DEC_CURVE, ADSR_PARAM::DEC, ADSR_PARAM::REL_CURVE, ADSR_PARAM::REL};
    handles.resize(ADSR_PARAM::NumADSRParams);

    handles[ADSR_PARAM::ATK_CURVE] = std::make_unique<VerticalCurveHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::ATK_CURVE]);
    handles[ADSR_PARAM::ATK] = std::make_unique<HorizontalHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::ATK]);
    handles[ADSR_PARAM::DEC_CURVE] = std::make_unique<VerticalCurveHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::DEC_CURVE]);
    // Calling the Decay/Sustain handle handles[ADSR_PARAM::DEC]
    handles[ADSR_PARAM::DEC] = std::make_unique<FreeHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::DEC], MODULATOR_PARAMS[modulator_id][ADSR_PARAM::SUS]);
    handles[ADSR_PARAM::REL_CURVE] = std::make_unique<VerticalCurveHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::REL_CURVE]);
    handles[ADSR_PARAM::REL] = std::make_unique<HorizontalHandle>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::REL]);

    for (int i = 0; i < handle_ids.size(); ++i) {
        addAndMakeVisible(*(handles[handle_ids[i]]));
    }

    zoom_img = juce::ImageCache::getFromMemory(BinaryData::ZOOM_ICON_png, BinaryData::ZOOM_ICON_pngSize);
    zoom_slider = std::make_unique<IconSlider>(m, MODULATOR_PARAMS[modulator_id][ADSR_PARAM::ZOOM], "ADSR Window Zoom", zoom_img);
    addAndMakeVisible(*zoom_slider);

    note_state = std::make_unique<NoteState>();
}

ADSRViewer::~ADSRViewer() {

}

void ADSRViewer::paint (juce::Graphics& g) {
    auto r = proportionOfWidth(HANDLE_RADIUS);
    auto m = proportionOfWidth(MARGIN);
    auto w = float(getWidth()) - 2.0f * m;
    auto h = float(getHeight()) - 2.0f * m;

    g.setColour (juce::Colours::white);
    g.fillRect (getLocalBounds());
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 2);
    g.fillRect(0, getHeight() - m, getWidth(), getHeight());

    auto total_length_ms = 1000.0f * adsr->get_parameter(ADSR_PARAM::ZOOM);

    // auto note_state = NoteState();
    auto release_ms = adsr->get_parameter(ADSR_PARAM::ATK) + adsr->get_parameter(ADSR_PARAM::DEC);
    note_state->set_release_time(release_ms);

    

    // position handles
    auto atk_x = float(w) * adsr->get_parameter(ADSR_PARAM::ATK) / total_length_ms + m;
    auto atk_y = m;
    auto atk_curve_x = (atk_x + m) * 0.5f;
    note_state->set_time(adsr->get_parameter(ADSR_PARAM::ATK) * 0.5f);
    // note_state.set_release_time(release_ms);
    auto atk_curve_y = float(h) * (1.0f - adsr->get(note_state.get())) + m;
    auto dec_x = atk_x + float(w) * adsr->get_parameter(ADSR_PARAM::DEC) / total_length_ms;
    auto dec_y = float(h) * (1.0f - adsr->get_parameter(ADSR_PARAM::SUS)) + m;
    auto dec_curve_x = (dec_x + atk_x)*0.5f;
    note_state->set_time((adsr->get_parameter(ADSR_PARAM::DEC) + 2.0 * adsr->get_parameter(ADSR_PARAM::ATK)) * 0.5f);
    // note_state.set_release_time(release_ms);
    auto dec_curve_y = float(h) * (1.0f - adsr->get(note_state.get())) + m;
    auto rel_x = dec_x + float(w) * adsr->get_parameter(ADSR_PARAM::REL) / total_length_ms;
    auto rel_y = h + m;
    auto rel_curve_x = (rel_x + dec_x)*0.5f;
    note_state->set_time((adsr->get_parameter(ADSR_PARAM::REL) + 2.0 * release_ms) * 0.5f);
    // note_state.set_release_time(release_ms);
    auto rel_curve_y = float(h) * (1.0f - adsr->get(note_state.get())) + m;

    auto max_attack_ms = float(PARAMETER_RANGES[MODULATOR_PARAMS[mod_id][ADSR_PARAM::ATK]].getRange().getEnd());
    auto max_decay_ms = float(PARAMETER_RANGES[MODULATOR_PARAMS[mod_id][ADSR_PARAM::DEC]].getRange().getEnd());
    auto max_release_ms = float(PARAMETER_RANGES[MODULATOR_PARAMS[mod_id][ADSR_PARAM::REL]].getRange().getEnd());
    auto max_attack_x = w * (max_attack_ms / total_length_ms);
    auto max_decay_x = w * (max_decay_ms / total_length_ms);
    auto max_release_x = w * (max_release_ms / total_length_ms);

    // set handle position (top left)
    handles[ADSR_PARAM::ATK_CURVE]->setCentrePosition(atk_curve_x, atk_curve_y);
    handles[ADSR_PARAM::ATK]->setCentrePosition(atk_x, atk_y);
    handles[ADSR_PARAM::DEC_CURVE]->setCentrePosition(dec_curve_x, dec_curve_y );
    handles[ADSR_PARAM::DEC]->setCentrePosition(dec_x, dec_y);
    handles[ADSR_PARAM::REL_CURVE]->setCentrePosition(rel_curve_x, rel_curve_y);
    handles[ADSR_PARAM::REL]->setCentrePosition(rel_x, rel_y);

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
    handles[ADSR_PARAM::ATK_CURVE]->constrain(atk_curve_x, h + m, 0, -h);
    handles[ADSR_PARAM::ATK]->constrain(m, m, max_attack_x, 0);
    handles[ADSR_PARAM::DEC_CURVE]->constrain(dec_curve_x, m, 0, dec_y - m);
    handles[ADSR_PARAM::DEC]->constrain(atk_x, m, max_decay_x, h);
    handles[ADSR_PARAM::REL_CURVE]->constrain(rel_curve_x, dec_y, 0, h + m - dec_y);
    handles[ADSR_PARAM::REL]->constrain(dec_x, h + m, max_release_x, 0);
    handles[ADSR_PARAM::ATK]->set_total_length_ms(total_length_ms);
    handles[ADSR_PARAM::DEC]->set_total_length_ms(total_length_ms);
    handles[ADSR_PARAM::REL]->set_total_length_ms(total_length_ms);

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
            note_state->set_time(ms);
            // note_state.set_release_time(release_ms);
            p.lineTo(x_pos + m, m + h - float(h) * adsr->get(note_state.get()));
        }
    }

    g.strokePath(p, juce::PathStrokeType(2));
}

void ADSRViewer::resized() {
    auto diameter = 2.0f * proportionOfWidth(HANDLE_RADIUS);
    for (int i = 0; i < handle_ids.size(); ++i) {
        handles[handle_ids[i]]->setSize(diameter, diameter);
    }
    auto area = getLocalBounds();
    auto top = area.removeFromTop(proportionOfHeight(0.2));
    auto topright = top.removeFromRight(proportionOfHeight(0.2));
    zoom_slider->setBounds(topright);
}

void ADSRViewer::timerCallback() {
    repaint();
}
