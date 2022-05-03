#pragma once

#include "JuceHeader.h"
// based on https://github.com/jakemumu/MTEC_498_698/blob/main/Week%209/Source/Managers/Parameter%20Manager/ParameterDefines.h

enum {
    ENABLE_MPE, 
    TotalNumberProperties
};

static const juce::StringArray PROPERTY_NAMES{
        "ENABLE_MPE"
};

//Parameter Ranges, low, high, grain, exp
static const juce::Array<juce::NormalisableRange<float>> PROPERTY_RANGES {
    // ENABLE_MPE
    juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f)
};

static const juce::Array<float> PROPERTY_DEFAULTS {
    // ENABLE_MPE
    0.0f,
};