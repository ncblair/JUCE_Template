#include "PopupParameterComponent.h"
#include "CustomLookAndFeel.h"

PopupParameterComponent::PopupParameterComponent() {
    startTimer(123);
    addAndMakeVisible(parameter_text);
    parameter_text.setJustificationType(juce::Justification::centred);
}

void PopupParameterComponent::paint (juce::Graphics& g) {
    g.setColour(juce::Colours::white);
    g.fillRoundedRectangle(0.0f, 0.0f, getWidth(), getHeight(), getHeight()/4.0f);
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(0.0f, 0.0f, getWidth(), getHeight(), getHeight()/4.0f, 1.0f);
}

void PopupParameterComponent::resized() {
    parameter_text.setBounds(getLocalBounds());
}

juce::String PopupParameterComponent::getStringFor(juce::Component& c) {
    if (auto* slider = dynamic_cast<juce::Slider*> (&c)) {
        if (! c.isCurrentlyBlockedByAnotherModalComponent()) {
            return slider->getName() + ": " + slider->getTextFromValue(slider->getValue());
        }
    }
    return {};
}

void PopupParameterComponent::timerCallback() {
    // determine current tooltip
    const auto mouseSource = juce::Desktop::getInstance().getMainMouseSource();
    if (mouseSource.isDragging()) {
        setVisible(true);
        if (current_component == nullptr) {
            current_component = mouseSource.getComponentUnderMouse();
        }
        const auto newString = current_component != nullptr ? getStringFor (*current_component) : juce::String();
        parameter_text.setText(newString, juce::dontSendNotification);

        auto component_bounds = getParentComponent()->getLocalArea(nullptr, current_component->getScreenBounds());
        auto font = parameter_text.getFont();
        auto popup_width = font.getStringWidthFloat(newString);
        auto popup_height = font.getHeight();
        auto popup_x = component_bounds.getX() + (component_bounds.getWidth() / 2.0f) - (popup_width / 2.0f);
        auto popup_y = component_bounds.getY() + component_bounds.getHeight() + popup_height;

        setBounds(popup_x, popup_y, popup_width, popup_height);
    }
    else {
        current_component = nullptr;
        setVisible(false);
    }
}