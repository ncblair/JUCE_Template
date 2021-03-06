#pragma once

class Matrix;
class Modulator;
class IconSlider;
class NoteState;
class ViewerHandle;

#include <JuceHeader.h>

// class ViewerHandle : public juce::Component {
//   public: 
//     ViewerHandle(Matrix* m, int m_id, int p_id, int p_id_2 = -1);
//     void mouseDown (const MouseEvent& e) override;
//     void mouseUp (const MouseEvent& e) override;
//     void mouseEnter (const MouseEvent& e) override;
//     void mouseExit (const MouseEvent& e) override;
//     void mouseDoubleClick (const MouseEvent& e) override;
//     void paint(juce::Graphics& g) override;
//     void constrain(float x, float y, float w, float h);
    
//   protected:
//     bool has_second_parameter();
//     float get_mouse_x_pos_ms(const MouseEvent& e);
//     float get_mouse_y_pos_linear(const MouseEvent& e);
//     float get_mouse_y_pos_curve(const MouseEvent& e);
//     std::unique_ptr<juce::ParameterAttachment> param_attachment; // attach to apvts parameter
//     std::unique_ptr<juce::ParameterAttachment> param_attachment_2;
  
//   private:
//     Matrix* matrix;
//     int param_id;
//     int param_id_2;
//     int mod_id;
//     juce::Rectangle<float> bounds;
//     bool mouse_hovering;

//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ViewerHandle)
// };

// class HorizontalHandle : public ViewerHandle {
//   public:
//     HorizontalHandle(Matrix* m, int m_id, int p_id);
//     void mouseDrag (const MouseEvent& e) override;
//   private:
//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HorizontalHandle)
// };

// class VerticalCurveHandle : public ViewerHandle {
//   public:
//     VerticalCurveHandle(Matrix* m, int m_id, int p_id);
//     void mouseDrag (const MouseEvent& e) override;
//   private:
//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalCurveHandle)
// };

// class FreeHandle : public ViewerHandle {
//   public:
//     FreeHandle(Matrix* m, int m_id, int p_id, int p_id_2);
//     void mouseDrag (const MouseEvent& e) override;
//   private:
//     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreeHandle)
// };

class ADSRViewer : public juce::Component, public juce::Timer {
  public:
    ADSRViewer(Matrix* m, int modulator_id);
    ~ADSRViewer();
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
  private:
    Matrix* matrix;
    int mod_id;
    Modulator* adsr;
    std::vector<std::unique_ptr<ViewerHandle>> handles;
    std::unique_ptr<NoteState> note_state;
    

    const float PRECISION = 300.0f;
    const int FRAMERATE = 30;
    const float HANDLE_RADIUS = 0.04f;
    const float MARGIN = 0.01f;

    std::vector<int> handle_ids;

    std::unique_ptr<IconSlider> zoom_slider;
    juce::Image zoom_img;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRViewer)
};