#include "PluginEditor.h"
#include "../interface/FooterComponent.h"
#include "../interface/ADSRComponent.h"
#include "../interface/LabeledKnobComponent.h"
#include "../managers/matrix/Matrix.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    std::cout << "EDITOR CONSTRUCTOR" << std::endl;
    startTimerHz(5);

    // create components at top (so they are not null ptrs when we set bounds)
    footer = std::make_unique<FooterComponent>(
        processorRef.matrix.get(),
        processorRef.matrix->getUndoManager()
    );
    midi_keyboard = std::make_unique<MidiKeyboardComponent>(
                        processorRef.keyboard_state, 
                        juce::KeyboardComponentBase::Orientation::horizontalKeyboard
    );
    auto env_ids = std::vector<int>{ADSR_1, ADSR_2, ADSR_3};
    envelopes = std::make_unique<ADSRParentComponent>(
        processorRef.matrix.get(), 
        env_ids // number of envelope sub-components
    );

    gain_knob = std::make_unique<LabeledKnobComponent>(
        processorRef.matrix.get(), // apvts
        LEVEL, // apvts param ID
        "level (in decibels) of the synth voice", // Knob Tooltip
        "Level" // Knob Label
    );

    semitones_knob = std::make_unique<LabeledKnobComponent>(
        processorRef.matrix.get(),
        SEMITONES,
        "Semitones pitch bend of the synth voice",
        "Semitones"
    );

    setSize (900, 700);

    //look and feel
    setLookAndFeel(&look_and_feel);

    // footer
    addAndMakeVisible(*footer);

    // midi keyboard
    addAndMakeVisible(*midi_keyboard);

    // envelopes
    addAndMakeVisible(*envelopes);

    // knob
    addAndMakeVisible(*gain_knob);
    addAndMakeVisible(*semitones_knob);

    // tooltip
    addAndMakeVisible(tooltip);
    tooltip.setMillisecondsBeforeTipAppears(0);

    // resizable window
    setResizable(true, true);
    setResizeLimits(510, 340, 1005, 670);
    getConstrainer()->setFixedAspectRatio(1.5);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::white);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    //bad encapsulation but: remove tooltip area here AND in CustomLookAndFeel
    area.removeFromBottom(proportionOfHeight(look_and_feel.tooltip_height));
    auto sw = getWidth() / 900.0;
    auto sh = getHeight() / 700.0;
    gain_knob->setBounds(60.0f*sw, 90.0f*sh, 50.0f*sw, 50.0f*sh);
    semitones_knob->setBounds(120.0f*sw, 90.0f*sh, 50.0f*sw, 50.0f*sh);
    midi_keyboard->setBounds(area.removeFromBottom(proportionOfHeight(0.1375f)));
    footer->setBounds(area.removeFromTop(proportionOfHeight(0.0625f)));
    auto left = area.removeFromLeft(area.proportionOfWidth(0.6f));
    auto right_top = area.removeFromTop(area.proportionOfHeight(0.5f));
    envelopes->setBounds(right_top);
    look_and_feel.setFontSize(15.0f*sw);
}


void AudioPluginAudioProcessorEditor::mouseDown (const MouseEvent& e) {
    std::cout << "MOUSE DOWN EDITOR" << std::endl;
}

void AudioPluginAudioProcessorEditor::timerCallback() {
    if (!isMouseButtonDownAnywhere()) {
        processorRef.matrix->getUndoManager()->beginNewTransaction();
    }
}