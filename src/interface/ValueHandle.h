#pragma once

class Matrix;

#include <JuceHeader.h>


class ValueHandle : public juce::Component {
  public: 
    ValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1, const juce::Identifier& value_id2);
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void paint(juce::Graphics& g) override;
    void constrain(float x, float y, float w, float h);    
  protected:
    bool has_second_value();
    float get_mouse_x_pos_linear(const MouseEvent& e);
    float get_mouse_y_pos_linear(const MouseEvent& e);
    float get_mouse_y_pos_curve(const MouseEvent& e);

    juce::UndoManager* undo_manager;
    juce::ValueTree value_tree;
    const juce::Identifier id1;
    const juce::Identifier id2;
    const juce::Identifier null_id = juce::Identifier("null");
  
  private:
    juce::Rectangle<float> bounds;
    bool mouse_hovering;
    const juce::NormalisableRange<float> curve_range{0.1f, 10.0f, 0.0f, 0.2890649616f};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueHandle)
};

class VerticalValueHandle : public ValueHandle {
  public:
    VerticalValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1);
    void mouseDrag (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalValueHandle)
};

class VerticalCurveValueHandle : public ValueHandle {
  public:
    VerticalCurveValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1);
    void mouseDrag (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalCurveValueHandle)
};

class FreeValueHandle : public ValueHandle {
  public:
    FreeValueHandle(juce::ValueTree tree, juce::UndoManager* undomanager, const juce::Identifier& value_id1, const juce::Identifier& value_id2);
    void mouseDrag (const MouseEvent& e) override;
    void mouseDoubleClick (const MouseEvent& e) override;
  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreeValueHandle)
};
