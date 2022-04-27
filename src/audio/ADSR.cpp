#include "ADSR.h"
#include "../managers/matrix/Matrix.h"


ADSRModulator::ADSRModulator(const std::vector<int>* p_ids) {
    param_ids = *p_ids;
}

// ADSRModulator::~ADSRModulator() {

// }

void ADSRModulator::update_parameters(Matrix * matrix, float ms, float release_time) {
    for (int p_id = 0; p_id < NumADSRParams; ++p_id) {
        params[p_id] = matrix->modulatedParamValue(param_ids[p_id], ms, release_time);
    }
}


float ADSRModulator::get(float ms, float release_time) {
    /*
    get position in envelope after ms milliseconds

    if the note has been released, ms is time since release

    */
    if (ms < release_time) {
        //ADS stages
        if (ms < params[ATK]) {
            //ATTACK
            return ms / params[ATK];
        }
        else {
            //DS sustains
            ms = ms - params[ATK];
            if (ms < params[DEC]) {
                //DECAY
                return 1.0f - (1.0f - params[SUS]) * ms / params[DEC];
            }
            else {
                //SUSTAIN
                return params[SUS];
            }
        }
    }
    else {
        auto ms_since_release = ms - release_time;
        if (ms_since_release < params[REL]) {
            // RELEASE
            // get the value when of the envelope when it was released recursively
            // this will prevent jumps on release during attack stage
            return get(release_time) * (1.0f - ms_since_release / params[REL]);
        }
        else {
            // END
            return 0.0f;
        }
    }
}
