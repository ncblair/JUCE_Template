#pragma once

#include <JuceHeader.h>
// #include "../util/ParamAttachment.h"
#include "../util/Modulator.h"
#include "../managers/matrix/ModulatorDefines.h"
// #include "../util/Matrix.h"

// //=====================================================================
// // Parent Class for creating Multiple ADSRModulator Objects
// // --- Compare with ADSRParentComponent, which houses multiple ADSR objects in the UI
// //=====================================================================
// class ADSRParent {
//   public:
//     ADSRParent(int number_envelopes);
//     // void attach(juce::AudioProcessorValueTreeState* apvts);
//     // std::vector<std::unique_ptr<juce::RangedAudioParameter>>* get_apvts_params();
//     std::vector<Modulator*> getChildren();
//   private:
//     int num_envelopes;
//     std::vector<std::unique_ptr<ADSR>> envelopes;
//     // std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
// };


//=====================================================================
// ADSRModulator is a type of modulator for ADSR envelopes
//=====================================================================
class ADSRModulator : public Modulator{
  public: 
    // virtual ~ADSRModulator();
    ADSRModulator(const std::vector<int>* p_ids);
    float get(float ms, float release_time=std::numeric_limits<float>::max()) override;
    void update_parameters(Matrix * matrix, float ms, float release_time=std::numeric_limits<float>::max()) override;
    float get_parameter(int param_id) override;

  private:
    std::array<float, NumADSRParams> params;

    std::vector<int> param_ids;
};
