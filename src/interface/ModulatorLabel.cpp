#include "ModulatorLabel.h"
#include "../matrix/Matrix.h"

ModulatorLabel::ModulatorLabel(Matrix* matrix, int modulator_id, juce::String tooltip, juce::String label_str) {
    mod_id = modulator_id;
    selected = SELECTION::NO;
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
    if (selected == SELECTION::YES_GLOBAL) {
        g.setColour(juce::Colours::lightslategrey);
    }
    else if (selected == SELECTION::YES_LOCAL) {
        g.setColour(juce::Colours::lightgrey);
    }
    else {
        g.setColour(juce::Colours::transparentWhite);
    }
    g.fillRect(getLocalBounds());
}

void ModulatorLabel::resized() {
    label.setBounds(getLocalBounds());
}

int ModulatorLabel::getModID() {
    return mod_id;
}

void ModulatorLabel::setSelected(int selected_type) {
    selected = selected_type;
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