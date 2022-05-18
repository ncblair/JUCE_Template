
#pragma once

class Matrix;
class Modulator;

#include <JuceHeader.h>

class IconPropertySlider : public juce::ImageComponent {
  public: 
    IconPropertySlider(Matrix* m, int p_id, juce::String tooltip, const juce::Image& icon);
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    // void mouseEnter (const MouseEvent& e) override;
    // void mouseExit (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
  
  private:
    Matrix* matrix;
    int property_id;
    std::unique_ptr<juce::ParameterAttachment> property_attachment;

    float last_position;
    const float MOUSE_SPEED{0.003f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IconPropertySlider)
};