#pragma once
#include <JuceHeader.h>
#include "Modulator.h"

// class ParamWithModList {
//   public:
//     // ParamAttachment* param;
//     std::map<juce::String, ModulatorInfo> modulators;

//     int& ParamWithModList::operator[](const int& index) {
//         return modulators[index];
//     }
//     int num_modulators() {
//         return modulators.size();
//     }
// };

struct ModulatorInfo {
    bool is_active{0};
    float depth{0.0f};
    Modulator* mod_ptr{nullptr};
};



class Matrix {
  public:
    Matrix(const juce::StringArray& param_names, juce::AudioProcessorValueTreeState* apvts_, std::map<juce::String, Modulator>* modulators) {
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
            for (Modulator& m : modulators) {
                auto m_name = m.getName();
                matrix[p_name][m_name] = ModulatorInfo();
                matrix[p_name][m_name].is_active = 0;
                matrix[p_name][m_name].depth = 0;
                matrix[p_name][m_name].mod_ptr = &m;
                // matrix[name].param = ;
            }
        }
    }

    float value_at(juce::String param_name, double ms_elapsed, double release_time=std::numeric_limits<float>::max()) {

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
        return param_range.convertFrom0to1(param_value);
    }

    // Modulator* get_modulator(juce::String modulator_name) {
    //     return mods[modulator_name];
    // }

    // void connect(juce::String param_name, juce::String mod_name)
    // void setDepth(juce::String param_name, juce::String mod_name, float depth)

    // std::vector<ModulatorInfo> find(juce::String param_name) {
    //     return matrix.find(param_name);
    // }

  private:
    std::map<juce::String, std::map<juce::String, ModulatorInfo>> matrix;
    // std::map<juce::String, Modulator*> mods;
    juce::AudioProcessorValueTreeState* apvts;
};