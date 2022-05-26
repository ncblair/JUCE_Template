#include "LFOModulator.h"
#include "NoteState.h"
#include "../matrix/Matrix.h"


LFOModulator::LFOModulator(const std::vector<int> p_ids) {
    param_ids = p_ids;
}

void LFOModulator::update_parameters(Matrix* matrix, const NoteState main_state) {
    for (int p_id = 0; p_id < LFO_PARAM::NumLFOParams; ++p_id) {
        if (PARAMETER_AUTOMATABLE[param_ids[p_id]]) {
            params[p_id] = matrix->modulatedParamValue(param_ids[p_id], main_state);
        }
        else {
            params[p_id] = matrix->paramValue(param_ids[p_id]);
        }
    }
}


float LFOModulator::get(const NoteState note_state) {
    /*
    get position in envelope after ms milliseconds
    */
    return 0.0f;
    
}

float LFOModulator::get_parameter(int param_id) {
    return params[param_id];
}