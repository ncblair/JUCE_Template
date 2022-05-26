#include "ModulatorLabel.h"
#include "../matrix/Matrix.h"

ModulatorLabel::ModulatorLabel(Matrix* matrix, int modulator_id, juce::String tooltip, juce::String label_str) {
    mod_id = modulator_id;
    selected = false;
    addAndMakeVisible(label);
    label.setText (label_str, juce::dontSendNotification);
    label.setTooltip(tooltip);
    label.setJustificationType(juce::Justification::centred);
    label.setInterceptsMouseClicks(false, false);
}

// ModulatorLabel::~ModulatorLabel(){

// }
void ModulatorLabel::paint (juce::Graphics& g) {
    g.setColour (juce::Colours::black);
    g.drawRect(getLocalBounds());
    if (selected) {
        g.setColour(juce::Colours::lightslategrey);
        g.fillRect(getLocalBounds());
    }
    else {
        g.setColour(juce::Colours::transparentWhite);
        g.fillRect(getLocalBounds());
    }
}

void ModulatorLabel::resized() {
    label.setBounds(getLocalBounds());
}

int ModulatorLabel::getModID() {
    return mod_id;
}

void ModulatorLabel::setSelected(bool is_selected) {
    selected = is_selected;
    repaint();
}

void ModulatorLabel::mouseDown (const MouseEvent& e) {
    std::cout << mod_id << " mouseDown()" << std::endl;
}

void ModulatorLabel::mouseDrag (const MouseEvent& e) {
    // std::cout << mod_id << " mouseDrag()" << getX() << " " << getY() << " " << e.x << " " << e.y << std::endl;
    juce::DragAndDropContainer* dragC = juce::DragAndDropContainer::findParentDragContainerFor(this);
    if (!dragC) {
        // std::cout << mod_id << " mouseDrag(): can't find parent drag container" << std::endl;
    } else {
        if (!dragC->isDragAndDropActive())
        {
            dragC->startDragging(mod_id, this);
        }
    }
}