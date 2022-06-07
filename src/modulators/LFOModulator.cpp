#include "LFOModulator.h"
#include "NoteState.h"
#include "../matrix/Matrix.h"


LFOModulator::LFOModulator(const std::vector<int> p_ids) {
    param_ids = p_ids;
    params.fill(0.0f);
    phase.resize(Matrix::NUM_VOICES + 1, 0.0);
    time.resize(Matrix::NUM_VOICES + 1, 0.0);
}

void LFOModulator::update_parameters(Matrix* matrix, NoteState* main_state) {
    for (int p_id = 0; p_id < LFO_PARAM::NumLFOParams; ++p_id) {
        if (PARAMETER_AUTOMATABLE[param_ids[p_id]]) {
            params[p_id] = matrix->modulatedParamValue(param_ids[p_id], main_state, true);
        }
        else {
            params[p_id] = matrix->paramValue(param_ids[p_id]);
        }
    }
    lfo_tree = matrix->get_read_only_lfo_tree();
}


float LFOModulator::get(NoteState* note_state) {
    /*
    TODO: only call from audio thread or use something clever like atomics
    get position in envelope after ms milliseconds
    */
    // double dt_ms = note_state->get_dt(); // assume mode is trigger
    int voice_id = note_state->get_voice_id();
    double dt_ms = note_state->get_time() - time[voice_id];
    time[voice_id] = note_state->get_time();
    // std::cout << dt_ms << " " << voice_id << std::endl;
    if (dt_ms < 0.0) {
        phase[voice_id] = 0.0;
    }
    else {
        phase[voice_id] += dt_ms * params[LFO_PARAM::FREQ] / 1000.0f;
        while (phase[voice_id] >= 1.0) {
            phase[voice_id] -= 1.0;
        }
        // phase[voice_id] = std::fmod(phase[voice_id], 1.0);
    }
    if (lfo_tree.get() == nullptr) {
        return 0.0f;
    }
    
    auto lfo_points = lfo_tree->getChildWithName(MODULATOR_IDS[params[LFO_PARAM::MOD_ID]]);
    auto height = double(lfo_points.getProperty(Matrix::LFO_HEIGHT_ID));
    auto curve_1 = double(lfo_points.getProperty(Matrix::LFO_CURVE_1_ID));
    
    float left_x = 0.0f;
    float left_y = height;
    float right_x = 1.0f;
    float right_y = height;

    auto cur_x = phase[voice_id];

    for (int i = 0; i < lfo_points.getNumChildren(); ++i) {
        auto point = lfo_points.getChild(i);
        right_x = double(point.getProperty(Matrix::LFO_POINT_X_ID));

        if (cur_x >= left_x && cur_x <= right_x) {
            right_y = double(point.getProperty(Matrix::LFO_POINT_Y_ID));
            break;
        }
        else {
            left_x = right_x;
            left_y = double(point.getProperty(Matrix::LFO_POINT_Y_ID));
        }
    }

    if (cur_x > right_x) {
        right_x = 1.0f;
        right_y = height;
    }
    auto percent = (right_x == left_x) ? 1.0 : (cur_x - left_x) / (right_x - left_x);
    // if (right_x == left_x) {
    //     // vertical line
    //     percent = 1.0;
    // }
    // else {
    //     percent = (cur_x - left_x) / (right_x - left_x);
    // }
    //TODO: curve
    // TODO: all modulators in ValueTree format
    auto cur_y = percent * right_y + (1.0f-percent) * left_y;
    // std::cout << cur_x << " " << cur_y << " " << left_x << " " << left_y << " " << right_x << " " << right_y << " " << std::endl;
    return cur_y;
}

float LFOModulator::get_parameter(int param_id) {
    return params[param_id];
}