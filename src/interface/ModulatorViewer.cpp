#include "ModulatorViewer.h"
#include "../util/Modulator.h"
#include "../managers/matrix/Matrix.h"

ModulatorViewer::ModulatorViewer(Matrix* m, int modulator_id) {
    matrix = m;
    mod_id = modulator_id;
    adsr = matrix->getModulator(mod_id);
    startTimerHz(24);
}

void ModulatorViewer::paint (juce::Graphics& g) {
    g.setColour (juce::Colours::white);
    g.fillRect (getLocalBounds());
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds());

    
    auto w = getWidth();
    auto h = getHeight();
    auto total_length_ms = 1000.0f * matrix->propertyValue(MODULATOR_PROPERTIES[mod_id][ZOOM]);
    auto start_release_ms = adsr->get_parameter(ATK) + adsr->get_parameter(DEC);
    
    // draw the waveform
    juce::Path p;
    p.startNewSubPath(0, h);

    for (float ms = total_length_ms / PRECISION; ms < total_length_ms; ms += total_length_ms / PRECISION) {
        p.lineTo(w * ms / total_length_ms, h - 0.8f * float(h) * adsr->get(ms, start_release_ms));
    }

    g.strokePath(p, juce::PathStrokeType(2));
}

void ModulatorViewer::resized() {

}

void ModulatorViewer::timerCallback() {
    repaint();
}