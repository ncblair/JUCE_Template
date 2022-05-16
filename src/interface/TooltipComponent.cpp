#include "TooltipComponent.h"

TooltipComponent::TooltipComponent() {
    startTimer(123);
    addAndMakeVisible(tooltip_text);
}

void TooltipComponent::resized() {
    tooltip_text.setBounds(getLocalBounds());
}

juce::String TooltipComponent::getTipFor(juce::Component& c) {
    if (auto* ttc = dynamic_cast<juce::TooltipClient*> (&c)) {
        if (! c.isCurrentlyBlockedByAnotherModalComponent()) {
            return ttc->getTooltip();
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