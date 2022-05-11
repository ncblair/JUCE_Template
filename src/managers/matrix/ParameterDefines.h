#pragma once

#include "JuceHeader.h"
// based on https://github.com/jakemumu/MTEC_498_698/blob/main/Week%209/Source/Managers/Parameter%20Manager/ParameterDefines.h

enum {
    LEVEL, 
    SEMITONES,
    ADSR_1_ATTACK,
    ADSR_1_ATTACK_CURVE,
    ADSR_1_DECAY,
    ADSR_1_DECAY_CURVE,
    ADSR_1_SUSTAIN,
    ADSR_1_RELEASE,
    ADSR_1_RELEASE_CURVE,
    ADSR_2_ATTACK, 
    ADSR_2_ATTACK_CURVE, 
    ADSR_2_DECAY,
    ADSR_2_DECAY_CURVE,
    ADSR_2_SUSTAIN,
    ADSR_2_RELEASE,
    ADSR_2_RELEASE_CURVE,
    ADSR_3_ATTACK, 
    ADSR_3_ATTACK_CURVE, 
    ADSR_3_DECAY,
    ADSR_3_DECAY_CURVE,
    ADSR_3_SUSTAIN,
    ADSR_3_RELEASE,
    ADSR_3_RELEASE_CURVE,
    TotalNumberParameters
};

static const juce::StringArray PARAMETER_NAMES{
        "LEVEL",
        "SEMITONES",
        "ADSR_1_ATTACK",
        "ADSR_1_ATTACK_CURVE",
        "ADSR_1_DECAY",
        "ADSR_1_DECAY_CURVE",
        "ADSR_1_SUSTAIN",
        "ADSR_1_RELEASE",
        "ADSR_1_RELEASE_CURVE",
        "ADSR_2_ATTACK",
        "ADSR_2_ATTACK_CURVE",
        "ADSR_2_DECAY",
        "ADSR_2_DECAY_CURVE",
        "ADSR_2_SUSTAIN",
        "ADSR_2_RELEASE",
        "ADSR_2_RELEASE_CURVE",
        "ADSR_3_ATTACK",
        "ADSR_3_ATTACK_CURVE",
        "ADSR_3_DECAY",
        "ADSR_3_DECAY_CURVE",
        "ADSR_3_SUSTAIN",
        "ADSR_3_RELEASE",
        "ADSR_3_RELEASE_CURVE"
};

//Parameter Ranges, low, high, grain, exp
static const juce::Array<juce::NormalisableRange<float>> PARAMETER_RANGES {
    // LEVEL
    juce::NormalisableRange<float>(-60.0f, 6.0f, 0.0f, 1.0f),
    // SEMITONES
    juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f, 1.0f),
    // ADSR_1_ATTACK
    juce::NormalisableRange<float> (0.0f, 5000.0f, 0.0f, 0.75f),
    // ADSR_1_ATTACK_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_1_DECAY
    juce::NormalisableRange<float> (0.0f, 60000.0f, 0.0f, 0.3f),
    // ADSR_1_DECAY_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_1_SUSTAIN
    juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    // ADSR_1_RELEASE
    juce::NormalisableRange<float> (0.00f, 10000.0f, 0.0f, 0.75f),
    // ADSR_1_RELEASE_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_2_ATTACK
    juce::NormalisableRange<float> (0.0f, 5000.0f, 0.0f, 0.75f),
    // ADSR_2_ATTACK_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_2_DECAY
    juce::NormalisableRange<float> (0.0f, 60000.0f, 0.0f, 0.3f),
    // ADSR_2_DECAY_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_2_SUSTAIN
    juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    // ADSR_2_RELEASE
    juce::NormalisableRange<float> (0.00f, 10000.0f, 0.0f, 0.75f),
    // ADSR_2_RELEASE_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_3_ATTACK
    juce::NormalisableRange<float> (0.0f, 5000.0f, 0.0f, 0.75f),
    // ADSR_3_ATTACK_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_3_DECAY
    juce::NormalisableRange<float> (0.0f, 60000.0f, 0.0f, 0.3f),
    // ADSR_3_DECAY_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f),
    // ADSR_3_SUSTAIN
    juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f),
    // ADSR_3_RELEASE
    juce::NormalisableRange<float> (0.00f, 10000.0f, 0.0f, 0.75f),
    // ADSR_3_RELEASE_CURVE
    juce::NormalisableRange<float> (0.1f, 10.0f, 0.0f, 0.2890649616f)
};

static const juce::Array<float> PARAMETER_DEFAULTS {
    // LEVEL
    -12.0f,
    // SEMITONES
    0.0f,
    // ADSR_1_ATTACK
    10.0f,
    // ADSR_1_ATTACK_CURVE
    1.0f,
    // ADSR_1_DECAY
    1000.0f,
    // ADSR_1_DECAY_CURVE
    1.0f,
    // ADSR_1_SUSTAIN
    1.0f,
    // ADSR_1_RELEASE
    1000.0f,
    // ADSR_1_RELEASE_CURVE
    0.5f,
    // ADSR_2_ATTACK
    10.0f,
    // ADSR_2_ATTACK_CURVE
    1.0f,
    // ADSR_2_DECAY
    1000.0f,
    // ADSR_2_DECAY_CURVE
    1.0f,
    // ADSR_2_SUSTAIN
    1.0f,
    // ADSR_2_RELEASE
    1000.0f,
    // ADSR_2_RELEASE_CURVE
    0.5f,
    // ADSR_3_ATTACK
    10.0f,
    // ADSR_3_ATTACK_CURVE
    1.0f,
    // ADSR_3_DECAY
    1000.0f,
    // ADSR_3_DECAY_CURVE
    1.0f,
    // ADSR_3_SUSTAIN
    1.0f,
    // ADSR_3_RELEASE
    1000.0f,
    // ADSR_3_RELEASE_CURVE
    0.5f,
};