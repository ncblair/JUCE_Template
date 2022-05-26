#pragma once

class NoteState;

#include <JuceHeader.h>
#include "Modulator.h"
#include "../matrix/ModulatorDefines.h"


enum LFO_PARAM {
    FREQ,
    MODE,
    SYNC,
    TEMPO,
    RISE,
    SPLINE_MODE,
    MOD_ID,
    NumLFOParams
};

//=====================================================================
// LFOModulator is a type of modulator for LFO envelopes
//=====================================================================
class LFOModulator : public Modulator{
  public: 
    // virtual ~LFOModulator();
    LFOModulator(const std::vector<int> p_ids);
    float get(const NoteState note_state) override;
    void update_parameters(Matrix* matrix, const NoteState main_state) override;
    float get_parameter(int param_id) override;

  private:
    std::array<float, NumLFOParams> params;
    std::vector<int> param_ids;
};