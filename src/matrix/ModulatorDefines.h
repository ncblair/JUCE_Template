#pragma once
#include "JuceHeader.h"
#include "ParameterDefines.h"
enum MOD {
	ADSR_1,
	ADSR_2,
	ADSR_3,
	LFO_1,
	LFO_2,
	LFO_3,
	LFO_4,
	TotalNumberModulators
};
enum MOD_TYPES {
	ADSR_MOD,
	LFO_MOD,
};
static const std::array<juce::Identifier, TotalNumberModulators> MODULATOR_IDS{
	"ADSR_1",
	"ADSR_2",
	"ADSR_3",
	"LFO_1",
	"LFO_2",
	"LFO_3",
	"LFO_4",
};
static const juce::StringArray MODULATOR_NAMES{
	"ADSR_1",
	"ADSR_2",
	"ADSR_3",
	"LFO_1",
	"LFO_2",
	"LFO_3",
	"LFO_4",
};
static const std::vector<int> MODULATOR_TYPES {
	MOD_TYPES::ADSR_MOD,
	MOD_TYPES::ADSR_MOD,
	MOD_TYPES::ADSR_MOD,
	MOD_TYPES::LFO_MOD,
	MOD_TYPES::LFO_MOD,
	MOD_TYPES::LFO_MOD,
	MOD_TYPES::LFO_MOD,
};
static const std::vector<std::vector<int>> MODULATOR_PARAMS {
	{PARAM::ADSR_1_ATTACK, PARAM::ADSR_1_ATTACK_CURVE, PARAM::ADSR_1_DECAY, PARAM::ADSR_1_DECAY_CURVE, PARAM::ADSR_1_SUSTAIN, PARAM::ADSR_1_RELEASE, PARAM::ADSR_1_RELEASE_CURVE, PARAM::ADSR_1_VIEWER_ZOOM, },
	{PARAM::ADSR_2_ATTACK, PARAM::ADSR_2_ATTACK_CURVE, PARAM::ADSR_2_DECAY, PARAM::ADSR_2_DECAY_CURVE, PARAM::ADSR_2_SUSTAIN, PARAM::ADSR_2_RELEASE, PARAM::ADSR_2_RELEASE_CURVE, PARAM::ADSR_2_VIEWER_ZOOM, },
	{PARAM::ADSR_3_ATTACK, PARAM::ADSR_3_ATTACK_CURVE, PARAM::ADSR_3_DECAY, PARAM::ADSR_3_DECAY_CURVE, PARAM::ADSR_3_SUSTAIN, PARAM::ADSR_3_RELEASE, PARAM::ADSR_3_RELEASE_CURVE, PARAM::ADSR_3_VIEWER_ZOOM, },
	{PARAM::LFO_1_FREQ, PARAM::LFO_1_MODE, PARAM::LFO_1_SYNC, PARAM::LFO_1_TEMPO, PARAM::LFO_1_RISE, PARAM::LFO_1_SPLINE, PARAM::LFO_1_ID, },
	{PARAM::LFO_2_FREQ, PARAM::LFO_2_MODE, PARAM::LFO_2_SYNC, PARAM::LFO_2_TEMPO, PARAM::LFO_2_RISE, PARAM::LFO_2_SPLINE, PARAM::LFO_2_ID, },
	{PARAM::LFO_3_FREQ, PARAM::LFO_3_MODE, PARAM::LFO_3_SYNC, PARAM::LFO_3_TEMPO, PARAM::LFO_3_RISE, PARAM::LFO_3_SPLINE, PARAM::LFO_3_ID, },
	{PARAM::LFO_4_FREQ, PARAM::LFO_4_MODE, PARAM::LFO_4_SYNC, PARAM::LFO_4_TEMPO, PARAM::LFO_4_RISE, PARAM::LFO_4_SPLINE, PARAM::LFO_4_ID, },
};