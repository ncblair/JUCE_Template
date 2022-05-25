#include "PresetBrowserComponent.h"
#include "../managers/matrix/Matrix.h"


SavePresetDialog::SavePresetDialog(Matrix* m)  : save_button("Save"), cancel_button("Cancel") {
    // undo redo
    matrix = m;
    addAndMakeVisible(save_button);
    addAndMakeVisible(cancel_button);
    addAndMakeVisible(preset_name_field);
    addAndMakeVisible(preset_name_label);
    preset_name_label.setText("Save Preset Name", juce::dontSendNotification);
    reset();
    
    save_button.onClick = [this] { save(); };
    cancel_button.onClick = [this] { setVisible(false); reset();};
}

void SavePresetDialog::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 2);
    
}

void SavePresetDialog::resized() {

    preset_name_label.setBoundsRelative(0.25f, 0.2f, 0.5f, 0.1f);
    preset_name_field.setBoundsRelative(0.25f, 0.3f, 0.5f, 0.1f);
    cancel_button.setBoundsRelative(0.25f, 0.6f, 0.2f, 0.1f);
    save_button.setBoundsRelative(0.55f, 0.6f, 0.2f, 0.1f);
    preset_name_field.setFont(juce::Font(proportionOfHeight(0.1f)));
}

void SavePresetDialog::save() {
    matrix->save_preset(preset_name_field.getText());
    setVisible(false); 
    reset();
    current_preset_name.setValue (matrix->get_preset_name());
}

void SavePresetDialog::reset() {
    preset_name_field.clear();
}

PresetBrowserComponent::PresetBrowserComponent(Matrix* m, SavePresetDialog* dialog_window) : preset_tree(m->getPresetTree()) 
{
    matrix = m;
    save_dialog_window = dialog_window;
    // undo redo
    zoom_img = juce::ImageCache::getFromMemory(BinaryData::SAVE_ICON_png, BinaryData::SAVE_ICON_pngSize);
    save_preset_button.setImages(false, true, true, zoom_img, 1.0f, juce::Colours::transparentWhite, zoom_img, 0.7f, juce::Colours::transparentWhite, zoom_img, 0.7f, juce::Colours::transparentWhite);
    save_preset_button.setTooltip("Save Current Config as Preset");
    save_preset_button.onClick = [this] {prompt_save_dialog();};
    addAndMakeVisible (save_preset_button);

    preset_label.setJustificationType(juce::Justification::centred);
    preset_label.addMouseListener(this, true);
    preset_label.setText(matrix->get_preset_name(), juce::dontSendNotification);
    addAndMakeVisible (preset_label);
    // addAndMakeVisible (preset_menu);

    // matrix->add_listener_to_preset_tree(this); // update text on preset change
    // matrix->getPresetTree()->addListener(this);
    // matrix->getPresetTree().getPropertyAsValue("NAME", nullptr).addListener(this);
    // save_dialog_window->current_preset_name.addListener(this);
    preset_tree.addListener(this);
    
}

void PresetBrowserComponent::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black);
    g.fillAll(juce::Colours::skyblue);
}

void PresetBrowserComponent::resized() {
    auto area = getLocalBounds();
    save_preset_button.setBounds(area.removeFromLeft(proportionOfWidth(0.2f)));
    preset_label.setBounds(area);
    // preset_menu.setBounds(area);
}

void PresetBrowserComponent::mouseDown (const MouseEvent& e) {
    if(e.mods.isLeftButtonDown())
    {
        // // left mouse click
        // right mouse click
        preset_menu.clear();
        int i = 0;
        preset_menu.addItem(++i,"INIT",true);

        preset_files = matrix->PRESETS_DIR.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.template_preset");

        for (auto& file : preset_files) {
            preset_menu.addItem(++i, file.getFileNameWithoutExtension(), true);
        }
        
        preset_menu.showMenuAsync 
        (
            PopupMenu::Options().withTargetComponent(preset_label).withMinimumWidth(preset_label.getWidth()),
            [this] (int result)
            {
                std::cout << result << " RESULT" << std::endl;
                if (result == 0) {
                    // user dismissed the menu without picking anything
                }
                else if (result == 1) {
                    // init
                    matrix->init();
                    // matrix->getPresetTree()->addListener(this);
                }
                else if (result < preset_files.size() + 2){
                    matrix->load_preset(preset_files[result - 2].getFileNameWithoutExtension());
                }
                // preset_label.setText(matrix->get_preset_name(), juce::dontSendNotification);
            }
        );

    }
}

void PresetBrowserComponent::prompt_save_dialog() {
    // prompt user for preset name and save preset here
    save_dialog_window->setVisible(true);
}

// void LabeledKnobComponent::mouseUp(const MouseEvent& e) {
//     // value_popup_box.dismiss();
// }

void PresetBrowserComponent::valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property){
    // std::cout << "Preset Tree Changed " << std::endl;
    preset_label.setText(matrix->get_preset_name(), juce::dontSendNotification);
}