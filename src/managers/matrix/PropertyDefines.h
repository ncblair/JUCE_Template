#pragma once

#include "JuceHeader.h"
// based on https://github.com/jakemumu/MTEC_498_698/blob/main/Week%209/Source/Managers/Parameter%20Manager/ParameterDefines.h

enum {
    ENABLE_MPE, 
    ADSR_1_VIEWER_ZOOM,
    ADSR_2_VIEWER_ZOOM,
    ADSR_3_VIEWER_ZOOM,
    TotalNumberProperties
};

static const juce::StringArray PROPERTY_NAMES{
        "ENABLE_MPE",
        "ADSR_1_VIEWER_ZOOM",
        "ADSR_2_VIEWER_ZOOM",
        "ADSR_3_VIEWER_ZOOM"
};

//Parameter Ranges, low, high, grain, exp
static const juce::Array<juce::NormalisableRange<float>> PROPERTY_RANGES {
    // ENABLE_MPE
    juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
    // ADSR_1_VIEWER_ZOOM (in Seconds)
    juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
    // ADSR_2_VIEWER_ZOOM
    juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
    // ADSR_3_VIEWER_ZOOM
    juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
};

static const juce::Array<float> PROPERTY_DEFAULTS {
    // ENABLE_MPE
    0.0f,
    // ADSR_1_VIEWER_ZOOM
    3.5f,
    // ADSR_2_VIEWER_ZOOM
    3.5f,
    // ADSR_3_VIEWER_ZOOM
    3.5f
};