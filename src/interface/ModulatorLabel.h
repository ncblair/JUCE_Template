#pragma once

class Matrix;

#include <JuceHeader.h>

class ModulatorLabel : public  juce::Component {
  public:
    ModulatorLabel(Matrix* matrix, int modulator_id, const juce::String tooltip, const juce::String label_str);
    // ~ModulatorLabel();
    void paint (juce::Graphics& g) override;
    void resized() override;
    int getModID();

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

  private:
    juce::Label label;
    int mod_id;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorLabel)
};