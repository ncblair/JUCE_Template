#pragma once

#include "JuceHeader.h"
#include "ParameterDefines.h"

enum {
    ADSR_1,
    ADSR_2,
    ADSR_3,
    TotalNumberModulators
};

// enum {
//     ADSR_MOD, 
//     LFO_MOD
// };

static const juce::StringArray MODULATOR_NAMES {
    "ADSR_1", 
    "ADSR_2", 
    "ADSR_3"
};

static const std::vector<std::vector<int>> MODULATOR_PARAMS {
    {ADSR_1_ATTACK, ADSR_1_DECAY, ADSR_1_SUSTAIN, ADSR_1_RELEASE},
    {ADSR_2_ATTACK, ADSR_2_DECAY, ADSR_2_SUSTAIN, ADSR_2_RELEASE},
    {ADSR_3_ATTACK, ADSR_3_DECAY, ADSR_3_SUSTAIN, ADSR_3_RELEASE}
};

// static const std::vector<int> MODULATOR_TYPES {
//     ADSR_MOD, 
//     ADSR_MOD, 
//     ADSR_MOD
// };
