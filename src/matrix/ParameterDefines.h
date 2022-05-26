#pragma once
#include "JuceHeader.h"
enum PARAM {
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
	LFO_1_FREQ,
	LFO_1_MODE,
	LFO_1_SYNC,
	LFO_1_TEMPO,
	LFO_1_RISE,
	LFO_1_SPLINE,
	LFO_1_ID,
	LFO_2_FREQ,
	LFO_2_MODE,
	LFO_2_SYNC,
	LFO_2_TEMPO,
	LFO_2_RISE,
	LFO_2_SPLINE,
	LFO_2_ID,
	LFO_3_FREQ,
	LFO_3_MODE,
	LFO_3_SYNC,
	LFO_3_TEMPO,
	LFO_3_RISE,
	LFO_3_SPLINE,
	LFO_3_ID,
	LFO_4_FREQ,
	LFO_4_MODE,
	LFO_4_SYNC,
	LFO_4_TEMPO,
	LFO_4_RISE,
	LFO_4_SPLINE,
	LFO_4_ID,
	ENABLE_MPE,
	ADSR_1_VIEWER_ZOOM,
	ADSR_2_VIEWER_ZOOM,
	ADSR_3_VIEWER_ZOOM,
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
	"ADSR_3_RELEASE_CURVE",
	"LFO_1_FREQ",
	"LFO_1_MODE",
	"LFO_1_SYNC",
	"LFO_1_TEMPO",
	"LFO_1_RISE",
	"LFO_1_SPLINE",
	"LFO_1_ID",
	"LFO_2_FREQ",
	"LFO_2_MODE",
	"LFO_2_SYNC",
	"LFO_2_TEMPO",
	"LFO_2_RISE",
	"LFO_2_SPLINE",
	"LFO_2_ID",
	"LFO_3_FREQ",
	"LFO_3_MODE",
	"LFO_3_SYNC",
	"LFO_3_TEMPO",
	"LFO_3_RISE",
	"LFO_3_SPLINE",
	"LFO_3_ID",
	"LFO_4_FREQ",
	"LFO_4_MODE",
	"LFO_4_SYNC",
	"LFO_4_TEMPO",
	"LFO_4_RISE",
	"LFO_4_SPLINE",
	"LFO_4_ID",
	"ENABLE_MPE",
	"ADSR_1_VIEWER_ZOOM",
	"ADSR_2_VIEWER_ZOOM",
	"ADSR_3_VIEWER_ZOOM",
};
static const juce::Array<juce::NormalisableRange<float>> PARAMETER_RANGES {
	juce::NormalisableRange<float>(-60.0f, 6.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(-24.0f, 24.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 5000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 60000.0f, 0.0f, 0.3f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 10000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 5000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 60000.0f, 0.0f, 0.3f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 10000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 5000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 60000.0f, 0.0f, 0.3f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 10000.0f, 0.0f, 0.75f),
	juce::NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.2890649616f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 2.0f),
	juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 3.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 12.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 4.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 2.0f),
	juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 3.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 12.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 4.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 2.0f),
	juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 3.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 12.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 4.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 100.0f, 0.0f, 2.0f),
	juce::NormalisableRange<float>(0.0f, 2.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 3.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 12.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 4.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.0f, 1.0f, 1.0f, 1.0f),
	juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
	juce::NormalisableRange<float>(0.3f, 60.0f, 0.0f, 1.0f),
};
static const juce::Array<float> PARAMETER_DEFAULTS {
	-12.0f,
	0.0f,
	10.0f,
	1.0f,
	1000.0f,
	1.0f,
	1.0f,
	400.0f,
	0.5f,
	10.0f,
	1.0f,
	1000.0f,
	1.0f,
	1.0f,
	400.0f,
	0.5f,
	10.0f,
	1.0f,
	1000.0f,
	1.0f,
	1.0f,
	400.0f,
	0.5f,
	2.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	2.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	2.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	2.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	3.5f,
	3.5f,
	3.5f,
};
static const juce::Array<bool> PARAMETER_AUTOMATABLE {
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	false,
	false,
	false,
};