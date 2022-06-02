#pragma once

class Matrix;

#include <JuceHeader.h>


class ViewerHandle : public juce::Component {
  public: 
    ViewerHandle(Matrix* m, int p_id, int p_id_2 = -1);
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
    void paint(juce::Graphics& g) override;
    void constrain(float x, float y, float w, float h);
    void set_total_length_ms(double length_ms);
    
  protected:
    bool has_second_parameter();
    float get_mouse_x_pos_ms(const MouseEvent& e);
    float get_mouse_y_pos_linear(const MouseEvent& e);
    float get_mouse_y_pos_curve(const MouseEvent& e);
    std::unique_ptr<juce::ParameterAttachment> param_attachment; // attach to apvts parameter
    std::unique_ptr<juce::ParameterAttachment> param_attachment_2;
  
  private:
    Matrix* matrix;
    int param_id;
    int param_id_2;
    juce::Rectangle<float> bounds;
    bool mouse_hovering;
    double total_length_ms{3000.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViewerHandle)
};

class HorizontalHandle : public ViewerHandle {
  public:
    HorizontalHandle(Matrix* m, int p_id);
    void mouseDrag (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalHandle)
};

class VerticalCurveHandle : public ViewerHandle {
  public:
    VerticalCurveHandle(Matrix* m, int p_id);
    void mouseDrag (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalCurveHandle)
};

class FreeHandle : public ViewerHandle {
  public:
    FreeHandle(Matrix* m, int p_id, int p_id_2);
    void mouseDrag (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreeHandle)
};
