#pragma once

// forward declarations
class Matrix;
// class PluginProcessor;

class Modulator {
  public:
    // virtual juce::String getName() = 0;
    // virtual ~Modulator() = 0;
    virtual float get(float ms, float release_time=std::numeric_limits<float>::max()) = 0;
    virtual void update_parameters(Matrix* matrix, float ms, float release_time=std::numeric_limits<float>::max()) = 0;
    // virtual void prepareToPlay(PluginProcessor* processor) = 0;
};