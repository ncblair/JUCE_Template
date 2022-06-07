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
    void setSelected(int selected_type);

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;

    static const enum SELECTION {
        NO,
        YES_LOCAL,
        YES_GLOBAL
    };

  private:
    juce::Label label;
    int mod_id;
    int selected;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulatorLabel)
};