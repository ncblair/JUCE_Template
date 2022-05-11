
#pragma once

class Matrix;
class Modulator;

#include <JuceHeader.h>

class ModulatorViewer : public juce::Component, public juce::Timer{
  public:
    ModulatorViewer(Matrix* m, int modulator_id);
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
  private:
    Matrix* matrix;
    int mod_id;
    Modulator* adsr;
    const float PRECISION = 200.0f;
    
    // std::vector<float> attack_line;
    // std::vector<float> decay_line;
    // std::vector<float> release_line;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorViewer)
};