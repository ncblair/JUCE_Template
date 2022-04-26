#include "ADSR.h"
#include "../plugin/PluginProcessor.h"


ADSRModulator::ADSRModulator(juce::String attack, juce::String decay, juce::String sustain, juce::String release) {
    atk_str = attack;
    dec_str = decay;
    sus_str = sustain;
    rel_str = release;
}

void ADSRModulator::prepareToPlay(PluginProcessor* processor) {
    params = &(processor->processor_parameters);
}
// void ADSRModulator::set(float a, float d, float s, float r) {
//     attack = a;
//     decay = d;
//     sustain = s;
//     release = r;
// }

// ADSRParent::ADSRParent(int number_envelopes) {
//     num_envelopes = number_envelopes;
//     for (int i = 0; i < num_envelopes; ++i) {
//         envelopes.push_back(std::make_unique<ADSR>("ADSR_" + std::to_string(i)));
//     }
// }

// // std::vector<std::unique_ptr<juce::RangedAudioParameter>>* ADSRParent::get_apvts_params() {
// //     params.clear();
// //     for (int i = 0; i < num_envelopes; ++i) {
// //         auto params_i = envelopes[i]->get_apvts_params(i);
// //         // concatenate the new params
// //         // param_layout.add(params_i.begin(), params_i.end());
// //         params.insert(params.end(), params_i->begin(), params_i->end());
// //         // params.resize(params.size + params_i.size);
// //         // std::move(params_i.begin(), params_i.end(), params.begin() + params.size());
// //     }
// //     return &params;
// // }


// // void ADSRParent::attach(juce::AudioProcessorValueTreeState* apvts) {
// //     for (int i = 0; i < num_envelopes; ++i) {
// //         envelopes[i]->attach(apvts, i);
// //     }
// // }

// std::vector<Modulator*> ADSRParent::getChildren() {
//     return envelopes
// }


float ADSRModulator::get(float ms, float release_time) {
    /*
    get position in envelope after ms milliseconds

    if the note has been released, ms is time since release

    */
    if (ms < release_time) {
        //ADS stages
        if (ms < (*params)[atk_str]) {
            //ATTACK
            return ms / (*params)[atk_str];
        }
        else {
            //DS sustains
            ms = ms - (*params)[atk_str];
            if (ms < (*params)[dec_str]) {
                //DECAY
                return 1.0f - (1.0f - (*params)[sus_str]) * ms / (*params)[dec_str];
            }
            else {
                //SUSTAIN
                return (*params)[sus_str];
            }
        }
    }
    else {
        auto ms_since_release = ms - release_time;
        if (ms_since_release < (*params)[rel_str]) {
            // RELEASE
            // get the value when of the envelope when it was released recursively
            // this will prevent jumps on release during attack stage
            return get(release_time) * (1.0f - ms_since_release / (*params)[rel_str]);
        }
        else {
            // END
            return 0.0f;
        }
    }
}

// std::vector<std::unique_ptr<juce::RangedAudioParameter>>* ADSR::get_apvts_params(int index) {
//     params.clear();

//     //attack
//     auto atk_param_range = juce::NormalisableRange<float> (0.001f, 5.0f, 0.0f, 0.75f); // low, high, grain, exp
//     params.push_back(std::make_unique<juce::AudioParameterFloat>(
//         "ADSR_" << index << "_ATTACK",  // parameter ID
//         "ADSR " << index << " Attack",  // parameter name
//         atk_param_range,  // range
//         0.01f,         // default value
//         "", // parameter label (description?)
//         juce::AudioProcessorParameter::Category::genericParameter,
//         [atk_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
//             std::stringstream ss;
//             ss << std::fixed << std::setprecision(2) << atk_param_range.convertTo0to1(value);
//             return juce::String(ss.str());
//         },
//         [atk_param_range](juce::String text) {
//             return atk_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
//         }
//     ));

//     //decay
//     auto dec_param_range = juce::NormalisableRange<float> (0.001f, 60.0f, 0.0f, 0.3f);
//     params.push_back(std::make_unique<juce::AudioParameterFloat>(
//         "ADSR_" << index << "_DECAY",  // parameter ID
//         "ADSR " << index << " Decay",  // parameter name
//         dec_param_range,  // range
//         1.0f,         // default value
//         "", // parameter label (description?)
//         juce::AudioProcessorParameter::Category::genericParameter,
//         [dec_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
//             std::stringstream ss;
//             ss << std::fixed << std::setprecision(2) << dec_param_range.convertTo0to1(value);
//             return juce::String(ss.str());
//         },
//         [dec_param_range](juce::String text) {
//             return dec_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
//         }
//     ));

//     //sustain
//     auto sus_param_range = juce::NormalisableRange<float> (0.0f, 1.0f, 0.0f, 1.0f);
//     params.push_back(std::make_unique<juce::AudioParameterFloat>(
//         "ADSR_" << index << "_SUSTAIN",  // parameter ID
//         "ADSR " << index << " Sustain",  // parameter name
//         sus_param_range,  // range
//         1.0f,         // default value
//         "", // parameter label (description?)
//         juce::AudioProcessorParameter::Category::genericParameter,
//         [sus_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
//             std::stringstream ss;
//             ss << std::fixed << std::setprecision(2) << sus_param_range.convertTo0to1(value);
//             return juce::String(ss.str());
//         },
//         [sus_param_range](juce::String text) {
//             return sus_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
//         }
//     ));

//     //release
//     auto rel_param_range = juce::NormalisableRange<float> (0.001f, 10.0f, 0.0f, 0.75f);
//     params.push_back(std::make_unique<juce::AudioParameterFloat>(
//         "ADSR_" << index << "_RELEASE",  // parameter ID
//         "ADSR " << index << " Release",  // parameter name
//         rel_param_range,  // range
//         1.0f,         // default value
//         "", // parameter label (description?)
//         juce::AudioProcessorParameter::Category::genericParameter,
//         [rel_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
//             std::stringstream ss;
//             ss << std::fixed << std::setprecision(2) << rel_param_range.convertTo0to1(value);
//             return juce::String(ss.str());
//         },
//         [rel_param_range](juce::String text) {
//             return rel_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
//         }
//     ));

//     return &params
// }

// void ADSR::attach(juce::AudioProcessorValueTreeState* apvts, int index) {
//     attack.attachToParameter(apvts->getParameter("ADSR_" << index << "_ATTACK"));
//     sustain.attachToParameter(apvts->getParameter("ADSR_" << index << "_SUSTAIN"));
//     decay.attachToParameter(apvts->getParameter("ADSR_" << index << "_DECAY"));
//     release.attachToParameter(apvts->getParameter("ADSR_" << index << "_RELEASE"));
//     //attack.onParameterChanged = [&] { update_attack(); };
// }

// juce::String ADSRModulator::getName() {
//     return modulator_name;
// }