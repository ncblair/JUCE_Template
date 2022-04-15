#include "ADSR.h"

void ADSRParent::ADSRParent(int number_envelopes) {
    num_envelopes = number_envelopes;
    for (int i = 0; i < num_envelopes; ++i) {
        envelopes.push_back(std::make_unique<ADSR>());
    }
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>> ADSRParent::get_apvts_params() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    for (int i = 0; i < num_envelopes; ++i) {
        auto params_i = envelopes[i]->get_apvts_params(i);
        // concatenate the new params
        params.insert(v.end(), params_i.begin(), params_i.end());
        // params.resize(params.size + params_i.size);
        // std::move(params_i.begin(), params_i.end(), params.begin() + params.size());
    }
    return params;
}


void ADSRParent::attach(juce::AudioProcessorValueTreeState apvts) {
    for (int i = 0; i < num_envelopes; ++i) {
        envelopes[i]->attach(apvts, i);
    }
}

// void ADSR::set(float a, float d, float s, float r) {
//     attack = a;
//     decay = d;
//     sustain = s;
//     release = r;
// }

void ADSR::get(float ms, is_released=false) {
    /*
    get position in envelope after ms milliseconds

    if the note has been released, ms is time since release
    */
    if (is_released) {
        if (ms < release.getValue());
            // RELEASE
            return cur * (1.0f - ms / release.getValue());
        else {
            // END
            return 0.0f;
        }
    }
    else {
        //ADS stages
        if (ms < attack.getValue()) {
            //ATTACK
            cur = ms / attack.getValue();
        }
        else {
            //DS sustains
            ms = ms - attack.getValue();
            if (ms < decay.getValue()) {
                //DECAY
                cur = 1.0f - (1.0f - sustain.getValue()) * ms / decay.getValue();
            }
            else {
                //SUSTAIN
                cur = sustain.getValue();
            }
        }
        return cur;
    }
}

std::vector<std::unique_ptr<juce::RangedAudioParameter>> ADSR::get_apvts_params(int index) {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //attack
    auto atk_param_range = juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.75f); // low, high, grain, exp
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ADSR_" << index << "_ATTACK",  // parameter ID
        "ADSR " << index << " Attack",  // parameter name
        atk_param_range,  // range
        0.01f,         // default value
        "", // parameter label (description?)
        juce::AudioProcessorParameter::Category::genericParameter,
        [atk_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << atk_param_range.convertTo0to1(value);
            return juce::String(ss.str());
        },
        [atk_param_range](juce::String text) {
            return atk_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
        }
    ));

    //decay
    auto dec_param_range = juce::NormalisableRange<float> (0.001f, 60.0f, 0.0f, 0.3f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ADSR_" << index << "_DECAY",  // parameter ID
        "ADSR " << index << " Decay",  // parameter name
        dec_param_range,  // range
        1.0f,         // default value
        "", // parameter label (description?)
        juce::AudioProcessorParameter::Category::genericParameter,
        [dec_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << dec_param_range.convertTo0to1(value);
            return juce::String(ss.str());
        },
        [dec_param_range](juce::String text) {
            return dec_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
        }
    ));

    //sustain
    auto sus_param_range = juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ADSR_" << index << "_SUSTAIN",  // parameter ID
        "ADSR " << index << " Sustain",  // parameter name
        sus_param_range,  // range
        1.0f,         // default value
        "", // parameter label (description?)
        juce::AudioProcessorParameter::Category::genericParameter,
        [sus_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << sus_param_range.convertTo0to1(value);
            return juce::String(ss.str());
        },
        [sus_param_range](juce::String text) {
            return sus_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
        }
    ));

    //release
    auto rel_param_range = juce::NormalisableRange<float> (0.001f, 10.0f, 0.0f, 0.75f);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ADSR_" << index << "_RELEASE",  // parameter ID
        "ADSR " << index << " Release",  // parameter name
        rel_param_range,  // range
        1.0f,         // default value
        "", // parameter label (description?)
        juce::AudioProcessorParameter::Category::genericParameter,
        [rel_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << rel_param_range.convertTo0to1(value);
            return juce::String(ss.str());
        },
        [rel_param_range](juce::String text) {
            return rel_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
        }
    ));

    return params
}

void ADSR::attach(juce::AudioProcessorValueTreeState apvts, int index) {
    attack.attachToParameter(apvts.getParameter("ADSR_" << index << "_ATTACK"));
    sustain.attachToParameter(apvts.getParameter("ADSR_" << index << "_SUSTAIN"));
    decay.attachToParameter(apvts.getParameter("ADSR_" << index << "_DECAY"));
    release.attachToParameter(apvts.getParameter("ADSR_" << index << "_RELEASE"));
    //attack.onParameterChanged = [&] { update_attack(); };
}