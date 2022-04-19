#include <JuceHeader.h>
#pragma once

class ParamWithModList {
  public:
    ParamAttachment* param;
    std::map<juce::String, ModulatorInfo> modulators;

    int& ParamWithModList::operator[](const int& index) {
        return modulators[index];
    }
    int num_modulators() {
        return modulators.size();
    }
};

struct ModulatorInfo {
    bool is_active{0};
    float depth{0.0f};
    Modulator* mod_ptr{nullptr};
};

class Matrix {
  public:
    void createMatrix(std::vector<Modulator*> modulators, std::vector<ParamAttachment*> params) {
        // called in processor constructor, not while audio is running though

        for (ParamAttachment* p: params) {
            auto p_name = p->getName();
            matrix.emplace(name);
            for (Modulator* m : modulators) {
                auto m_name = m->getName(m_name)
                matrix[p_name].emplace(m_name);
                matrix[p_name][m_name].is_active = 0;
                matrix[p_name][m_name].depth = 0;
                matrix[p_name][m_name].mod_ptr = m;
                matrix[name].param = p;
            }
        }
    }

    float value_at(juce::String param_name, double ms_elapsed, bool note_released) {

        float v = 0.0f;
        ParamWithModList param_and_mods = matrix.find(param_name);
        for (int i = 0; i < param_and_mods.num_modulators(); ++i) {
            auto m = param_and_mods[i];
            if (m.is_active) {
                v += m.depth * m.mod_ptr->get(ms_elapsed, note_released);
            }
        }
        v = std::clamp(v + param_and_mods.param->getNormalisedValue(), 0.0f, 1.0f);
        return param_and_mods.param->unnormalized_value(v);
    }

    void connect(juce::String param_name, juce::String mod_name)

    std::vector<ModulatorInfo> find(juce::String param_name) {
        return matrix.find(param_name);
    }

  private:
    std::map<juce::String, ParamWithModList> matrix;
};