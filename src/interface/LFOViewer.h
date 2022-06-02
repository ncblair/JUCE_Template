#pragma once

class Matrix;
class Modulator;
class ValueHandle;

#include <JuceHeader.h>

class LFOViewer : public juce::Component, public juce::ValueTree::Listener {
  public:
    LFOViewer(Matrix* m, int modulator_id);
    ~LFOViewer();
    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDoubleClick (const MouseEvent& e) override;

    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    void valueTreeChildAdded (ValueTree &parentTree, ValueTree &childWhichHasBeenAdded) override;
    void valueTreeChildRemoved (ValueTree &parentTree, ValueTree &childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;

  private:
    void recreate_handles();

    Matrix* matrix;
    juce::ValueTree lfo_tree;
    int mod_id;
    Modulator* lfo;
    std::vector<std::unique_ptr<ValueHandle>> handles;
    // std::unique_ptr<NoteState> note_state;
    
    const float PRECISION = 300.0f;
    // const int FRAMERATE = 30;
    const float HANDLE_RADIUS = 0.04f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFOViewer)
};