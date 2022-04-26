#include "Matrix.h"
#include "Modulator.h"

Matrix::Matrix(const juce::StringArray& param_names, juce::AudioProcessorValueTreeState* apvts_, std::map<juce::String, Modulator*>* modulators) {
    // called in processor constructor, not while audio is running though
    // TODO change modulator to a map
    apvts = apvts_;
    // mods = modulators;

    // create modulator dictionary
    // for (Modulator m : modulators) {
    //     mods[m.getName()] = m;
    // }
    // create matrix
    for (juce::String p_name : param_names) {
        // auto p_name = p->getName();
        // matrix.emplace(p_name);
        matrix[p_name] = std::map<juce::String, ModulatorInfo>();
        for (auto const& [m_name, m] : *modulators) {
            matrix[p_name][m_name] = ModulatorInfo();
            matrix[p_name][m_name].mod_ptr = m;
            // matrix[name].param = ;
        }
    }
}

float Matrix::value_at(juce::String param_name, double ms_elapsed, double release_time) {

    // get all modulators and add up there total value to v
    float v = 0.0f;
    auto modulators = matrix[param_name];
    for (auto const& [m_name, m] : modulators) {
        if (m.is_active) {
            v += m.depth * m.mod_ptr->get(ms_elapsed, release_time);
        }
    }
    // add v to the parameter value and clamp
    auto param_range = apvts->getParameterRange(param_name);
    auto param_value = apvts->getRawParameterValue(param_name)->load();
    v = std::clamp(v + param_range.convertTo0to1(param_value), 0.0f, 1.0f);
    return param_range.convertFrom0to1(v);
}