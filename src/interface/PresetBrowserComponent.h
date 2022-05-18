#pragma once

class Matrix;

#include <JuceHeader.h>
class SavePresetDialog : public juce::Component {
  public:
    SavePresetDialog(Matrix* m);

    void paint(juce::Graphics& g) override;
    void resized() override;
    juce::Value current_preset_name;
    
  private:
    void save();
    void reset();
    
    Matrix* matrix;
    juce::Label preset_name_label;
    juce::TextEditor preset_name_field;
    juce::TextButton save_button;
    juce::TextButton cancel_button;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SavePresetDialog)
};

class PresetBrowserComponent : public  juce::Component, public juce::Value::Listener{
  public:
    PresetBrowserComponent(Matrix* m, SavePresetDialog* dialog_window);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;

    void valueChanged(Value& value) override;

  private:
    void prompt_save_dialog(); // saves a preset
    
    Matrix* matrix;
    SavePresetDialog* save_dialog_window;
    juce::ImageButton save_preset_button;
    // juce::FileBrowserComponent browser;
    juce::PopupMenu preset_menu;
    juce::Label preset_label;

    juce::Array<juce::File> preset_files;

    juce::Image zoom_img;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserComponent)
};