#include "TooltipComponent.h"

TooltipComponent::TooltipComponent() {
    startTimer(123);
    addAndMakeVisible(tooltip_text);
}

// void TooltipComponent::paint(juce::Graphics& g) {
//     g.fillAll(juce::Colours::green);
// }

void TooltipComponent::resized() {
    tooltip_text.setBounds(getLocalBounds());
}

juce::String TooltipComponent::getTipFor(juce::Component& c) {
    if (auto* ttc = dynamic_cast<juce::TooltipClient*> (&c)) {
        if (! c.isCurrentlyBlockedByAnotherModalComponent()) {
            auto name = c.getName();
            auto tip = ttc->getTooltip();
            if (name == "") {
                return tip;
            }
            else if (tip == "") {
                return name;
            }
            else {
                return name + ": " + tip;
            }
        }
    }
            
    return {};
}

void TooltipComponent::timerCallback() {
    // determine current tooltip
    const auto mouseSource = juce::Desktop::getInstance().getMainMouseSource();
    if (!mouseSource.isTouch()) {
        auto* newComp = mouseSource.getComponentUnderMouse();
        const auto newTip = newComp != nullptr ? getTipFor (*newComp) : juce::String();
        tooltip_text.setText(newTip, juce::dontSendNotification);
    }
}