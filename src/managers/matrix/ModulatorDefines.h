#pragma once

#include "JuceHeader.h"
#include "ParameterDefines.h"
#include "PropertyDefines.h"

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

enum {
    ATK,
    ATK_CURVE,
    DEC,
    DEC_CURVE,
    SUS,
    REL,
    REL_CURVE,
    NumADSRParams
};

static const std::vector<std::vector<int>> MODULATOR_PARAMS {
    {ADSR_1_ATTACK, ADSR_1_ATTACK_CURVE, ADSR_1_DECAY, ADSR_1_DECAY_CURVE, ADSR_1_SUSTAIN, ADSR_1_RELEASE, ADSR_1_RELEASE_CURVE},
    {ADSR_2_ATTACK, ADSR_2_ATTACK_CURVE, ADSR_2_DECAY, ADSR_2_DECAY_CURVE, ADSR_2_SUSTAIN, ADSR_2_RELEASE, ADSR_2_RELEASE_CURVE},
    {ADSR_3_ATTACK, ADSR_3_ATTACK_CURVE, ADSR_3_DECAY, ADSR_3_DECAY_CURVE, ADSR_3_SUSTAIN, ADSR_3_RELEASE, ADSR_3_RELEASE_CURVE}
};

enum {
    ZOOM,
    NumADSRProperties
};

static const std::vector<std::vector<int>> MODULATOR_PROPERTIES {
    {ADSR_1_VIEWER_ZOOM},
    {ADSR_2_VIEWER_ZOOM},
    {ADSR_3_VIEWER_ZOOM}
};

// static const std::vector<int> MODULATOR_TYPES {
//     ADSR_MOD, 
//     ADSR_MOD, 
//     ADSR_MOD
// };
