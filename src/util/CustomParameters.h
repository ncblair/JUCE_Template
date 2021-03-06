#pragma once

#include <JuceHeader.h>

template<typename T>
struct Property : private juce::ValueTree::Listener
                     private juce::Timer
{
    Property(const juce::Identifier& property_name)
    {
        proxy.addListener (this);
        // startTimerHz (20);
    }

    float proxy;
    std::atomic<T> value;

    void timerCallback() override { proxy = value.load(); }
    void valueTreePropertyChanged (ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override{
        if (property == property_name)
            value.store(treeWhosePropertyHasChanged.getProperty (property_name));
    }
    // void valueChanged (juce::Value& v) override { value.store (proxy.getValue()); }
};

// Custom Parameters with flag for if it's automatable

// class BoolParameter : public juce::AudioParameterBool {
//   public:
//     bool automatable;
//     BoolParameter(const String &parameterID, const String &parameterName, bool defaultValue, bool is_automatable=true)
//         : juce::AudioParameterBool(parameterID, parameterName, defaultValue){
//         // calls parent constructor
//         automatable = is_automatable;
//     }

//     bool isAutomatable() const override {
//         return automatable;
//     }
// };

// class ModulatedFloatParam {
//   public:
//     ModulatedFloatParam() {}
//     float value_at(double ms_elapsed, bool note_released) {
//         auto name = param_ptr->getName();

//         float v = 0.0f;
//         std::vector<ModulatorInfo> modulators = matrix_ptr->find(name);
//         for (int i = 0; i < modulators.size(); ++i) {
//             auto m = modulators[i];
//             if (m.is_active) {
//                 v += m.depth * m.mod_ptr->get(ms_elapsed, note_released);
//             }
//         }
//         v = std::clamp(v + param_ptr->getNormalisedValue(), 0.0f, 1.0f);
//         return param_ptr->unnormalized_value(v);
//     }
//   private:
//     ParamAttachment* param_ptr; //normal
//     Matrix* matrix_ptr;
// };

// class PrecomputedValue {
//   private:
//     std::vector<float> value_buffer;
//   public:
    
//     void getValue(int i) {
//         // return value at buffer position i;
//         return value_buffer[i];
//     }
//     virtual void computeNextValue(int num_samples) = 0;
//     virtual void prepareToPlay(double sampleRate, int samplesPerBlock) = 0;
// }

// class ADSREnv : public precomputedValue;

// // Float Param Wrapper for Modulation
// class FloatParamWrapper : public PrecomputedValue {
//   private:
//     juce::SmoothedValue<float> param;
//     std::vector<PrecomputedValue> modulators; // list of modulators (LFOs, Envelopes)
//     std::vector<float> modulator_depths; // -1 to 1
//     std::vector<bool> modulator_ons; // bool

//   public:
//     void addModulator(PrecomputedValue m) {
//         modulators.push_back(m);
//         modulator_depths.push_back(0.3);
//         modulator_ons.push_back(false);
//     }

//     void isModulatorOn(int i) {
//         // returns true if modulator[i] is on
//         return modulators_on[i];
//     }

//     void prepareToPlay(double sampleRate, int samplesPerBlock) override{
//         param.reset (sampleRate, samplesPerBlock / sampleRate); // set up so we are linearly interping
//         value_buffer.resize(2 * samplesPerBlock); //2x size for safety
//     }

//     float setNextValue(float next, int num_samples) {
//         param.setTargetValue(next); // set target value
//     }

//     float computenextValue(int num_samples) override {
//         /*
//         precomputes the next num_samples values of the parameter given the and parameter
//          - modulator values MUST be computed BEFORE this.
//         */
//         if (num_samples > value_buffer.size()) {
//             value_buffer.resize(num_samples*2)); //allocation on audio thread, (shouldn't run)
//             std::cout << "Warning: ALLOCATE ON AUDIO THREAD in CustomParameter.h line 60" << std::endl;
//         }

//         for (int i = 0; i < num_samples; ++i) {
//             value_buffer[i] = param.getNextValue();
//             for (int j = 0; j < modulators.size(); ++j) {
//                 if (modulator_ons[j] == true) {
//                     // modulator j is modulating this parameter
//                     value_buffer[i] += modulator_depths[j] * modulators[j].getValue(i);
//                 }
//             }
//         }
//     }
// };