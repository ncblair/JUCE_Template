#pragma once

class NoteState;

#include <JuceHeader.h>
#include "Modulator.h"
#include "../matrix/ModulatorDefines.h"


enum ADSR_PARAM {
	ATK,
	ATK_CURVE,
	DEC,
	DEC_CURVE,
	SUS,
	REL,
	REL_CURVE,
	ZOOM,
	NumADSRParams
};

//=====================================================================
// ADSRModulator is a type of modulator for ADSR envelopes
//=====================================================================
class ADSRModulator : public Modulator{
  public: 
	// virtual ~ADSRModulator();
	ADSRModulator(const std::vector<int> p_ids);
	float get(NoteState* note_state) override;
	void update_parameters(Matrix* matrix, NoteState* main_state) override;
	float get_parameter(int param_id) override;

  private:
	std::array<double, ADSR_PARAM::NumADSRParams> params;
	std::vector<int> param_ids;
	std::unique_ptr<NoteState> temp_ns;
};