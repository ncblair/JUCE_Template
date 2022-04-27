#include "Matrix.h"
#include "../../util/Modulator.h"
#include "../../plugin/PluginProcessor.h"

Matrix::Matrix(PluginProcessor* proc) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //==============================================================================
    //-> ADD PARAMS
    //==============================================================================
    for (int i = 0; i < TotalNumberParameters; ++i) {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
                PARAMETER_NAMES[i],   // parameter ID
                PARAMETER_NAMES[i],   // parameter name
                PARAMETER_RANGES[i],  // range
                PARAMETER_DEFAULTS[i],// default value
                "", // parameter label (description?)
                juce::AudioProcessorParameter::Category::genericParameter,
                [this, i](float value, int maximumStringLength) { // Float to String Precision 2 Digits
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << value;
                    return juce::String(ss.str());
                },
                [this, i](juce::String text) {
                    return text.getFloatValue(); // Convert Back to Value
                }
            )
        );
    }

    apvts_ptr.reset(new juce::AudioProcessorValueTreeState(*proc,
                                                        nullptr,
                                                        "PARAMETERS",
                                                        { params.begin(), params.end() }));

    //==============================================================================
    //-> Set Up Modulators
    //==============================================================================
    for (int m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        modulators[m_id] = std::make_unique<ADSRModulator>(
            &MODULATOR_PARAMS[m_id]
        );
    }

    //==============================================================================
    //-> Set Up Matrix
    //==============================================================================
    for (int p_id = 0; p_id < TotalNumberParameters; ++p_id) {
        matrix[p_id].reserve(TotalNumberModulators);
        // for (int m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        //     matrix[p_id][m_id] = ModulatorInfo();
        //     matrix[p_id][m_id].mod_id = m_id;
        // }
    }
}

Matrix::~Matrix(){

}

float Matrix::paramValue(int param_id) {
    // returns the parameter value of a certain ID in a thread safe way
    return apvts_ptr->getRawParameterValue(PARAMETER_NAMES[param_id])->load();
}

juce::NormalisableRange<float> Matrix::paramRange(int param_id) {
    // returns the parameter value of a certain ID in a thread safe way
    return apvts_ptr->getParameterRange(PARAMETER_NAMES[param_id]);
}

float Matrix::modulatorValue(int mod_id, double ms_elapsed, double release_time) {
    // returns the current modulator value after a ms_elapsed with release_time
    return modulators[mod_id]->get(ms_elapsed, release_time);
}

float Matrix::modulatedParamValue(int param_id, double ms_elapsed, double release_time) {
    // returns the param value modulated by currently active modulators
    float v = 0.0f;
    auto mods = matrix[param_id];
    for (auto const& mod : mods) {
        std::cout << "Mod " << MODULATOR_NAMES[mod.mod_id] << " attached to " << PARAMETER_NAMES[param_id] << " with depth " << mod.depth << std::endl;
        v += mod.depth * modulatorValue(mod.mod_id, ms_elapsed, release_time);
    }
    auto param_range = paramRange(param_id);
    auto param_value = paramValue(param_id);
    v = std::clamp(v + param_range.convertTo0to1(param_value), 0.0f, 1.0f);
    return param_range.convertFrom0to1(v);
}

void Matrix::connect(int mod_id, int param_id, float depth) {
    // TODO: Do this on the UI Thread and MAKE THREAD SAFE
    auto& param_modulators = matrix[param_id];
    bool already_connected = false;
    for (auto& mod : param_modulators) {
        if (mod.mod_id == mod_id) {
            already_connected = true;
            mod.depth = depth;
        }
    }
    if (!already_connected) {
        param_modulators.push_back(ModulatorInfo{depth, mod_id});
    }
}

void Matrix::disconnect(int mod_id, int param_id) {
    // TODO: Do this on the UI Thread and MAKE THREAD SAFE
    auto param_modulators = matrix[param_id];
    for (int i = 0; i < param_modulators.size(); ++i) {
        auto mod = param_modulators[i];
        if (mod.mod_id == mod_id) {
            param_modulators.erase(param_modulators.begin() + i);
        }
    }
}

juce::AudioProcessorValueTreeState* Matrix::getValueTree(){
    return apvts_ptr.get();
}

void Matrix::update_modulator_parameters(double ms_elapsed, double last_release_time) {
    for (int m_id = 0; m_id < TotalNumberModulators; ++m_id) {
        modulators[m_id]->update_parameters(this, ms_elapsed, last_release_time);
    }
}