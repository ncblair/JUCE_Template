#include "ADSRModulator.h"
#include "NoteState.h"
#include "../matrix/Matrix.h"


ADSRModulator::ADSRModulator(const std::vector<int> p_ids) {
    param_ids = p_ids;
}

void ADSRModulator::update_parameters(Matrix* matrix, const NoteState main_state) {
    for (int p_id = 0; p_id < ADSR_PARAM::NumADSRParams; ++p_id) {
        if (PARAMETER_AUTOMATABLE[param_ids[p_id]]) {
            params[p_id] = matrix->modulatedParamValue(param_ids[p_id], main_state);
        }
        else {
            params[p_id] = matrix->paramValue(param_ids[p_id]);
        }
    }
}

float ADSRModulator::get(const NoteState note_state) {
    /*
    get position in envelope after ms milliseconds

    if the note has been released, ms is time since release

    */
    // std::cout << note_state.get_time() << " " << note_state.get_release_time() << std::endl;
    // std::cout << params[ATK] << std::endl;
    auto ms = note_state.get_time();
    // std::cout << "iunside " << params[ATK] << " " << ms << std::endl;
    // std::cout << !note_state.is_released();
    if (!note_state.is_released()) {
        //ADS stages
        if (ms < params[ADSR_PARAM::ATK]) {
            //ATTACK
            // std::cout << "inside 2x" << ms << " " << params[ATK] << " " << params[ATK_CURVE] << std::endl;
            return std::pow(ms / params[ADSR_PARAM::ATK], params[ADSR_PARAM::ATK_CURVE]);
        }
        else {
            //DS sustains
            ms = ms - params[ADSR_PARAM::ATK];
            if (ms < params[ADSR_PARAM::DEC]) {
                //DECAY
                return 1.0f - (1.0f - params[ADSR_PARAM::SUS]) * std::pow(ms / params[ADSR_PARAM::DEC], params[ADSR_PARAM::DEC_CURVE]);
            }
            else {
                //SUSTAIN
                return params[ADSR_PARAM::SUS];
            }
        }
    }
    else {
        // std::cout << "RELEASED" << std::endl;
        auto release_time = note_state.get_release_time();
        auto ms_since_release = ms - release_time;
        if (ms_since_release < params[ADSR_PARAM::REL]) {
            // RELEASE
            // get the value when of the envelope when it was released recursively
            // this will prevent jumps on release during attack stage
            auto temp_ns = NoteState();
            temp_ns.set_time(release_time);
            temp_ns.mark_unreleased();
            return get(temp_ns) * (1.0f - std::pow(ms_since_release / params[ADSR_PARAM::REL], params[ADSR_PARAM::REL_CURVE]));
        }
        else {
            // END
            return 0.0f;
        }
    }
}

float ADSRModulator::get_parameter(int param_id) {
    return params[param_id];
}