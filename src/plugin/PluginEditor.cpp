#include "PluginEditor.h"
#include "../interface/FooterComponent.h"
#include "../interface/ModulatorParentComponent.h"
// #include "../interface/LFOComponent.h"
#include "../interface/LabeledKnobComponent.h"
#include "../interface/TooltipComponent.h"
#include "../interface/PopupParameterComponent.h"
#include "../interface/PresetBrowserComponent.h"
#include "../interface/SoundfileComponent.h"
#include "../interface/CLIComponent.h"
#include "../matrix/Matrix.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    std::cout << "EDITOR CONSTRUCTOR" << std::endl;
    startTimerHz(1);

    // create components at top (so they are not null ptrs when we set bounds)
    footer = std::make_unique<FooterComponent>(
        processorRef.matrix.get()
    );
    midi_keyboard = std::make_unique<MidiKeyboardComponent>(
                        processorRef.keyboard_state, 
                        juce::KeyboardComponentBase::Orientation::horizontalKeyboard
    );
    auto env_ids = std::vector<int>{MOD::ADSR_1, MOD::ADSR_2, MOD::ADSR_3};
    envelopes = std::make_unique<ModulatorParentComponent>(
        processorRef.matrix.get(), 
        env_ids // number of envelope sub-components
    );

    auto lfo_ids = std::vector<int>{MOD::LFO_1, MOD::LFO_2, MOD::LFO_3, MOD::LFO_4};
    lfos = std::make_unique<ModulatorParentComponent>(
        processorRef.matrix.get(),
        lfo_ids
    );

    gain_knob = std::make_unique<LabeledKnobComponent>(
        processorRef.matrix.get(), // apvts
        PARAM::LEVEL, // apvts param ID
        "level (in decibels) of the synth voice", // Knob Tooltip
        PARAMETER_NAMES[PARAM::LEVEL] // Knob Label
    );

    semitones_knob = std::make_unique<LabeledKnobComponent>(
        processorRef.matrix.get(),
        PARAM::SEMITONES,
        "Semitones pitch bend of the synth voice",
        PARAMETER_NAMES[PARAM::SEMITONES]
    );

    knob_popup = std::make_unique<PopupParameterComponent>();

    tooltip = std::make_unique<TooltipComponent>();

    save_preset_popup = std::make_unique<SavePresetDialog>(processorRef.matrix.get());
    preset_browser = std::make_unique<PresetBrowserComponent>(
        processorRef.matrix.get(),
        save_preset_popup.get()
    );

    // getChild(0) #hardcode for the first (and only in this case) child
    soundfile_component_1 = std::make_unique<SoundfileComponent>(
        processorRef.matrix.get(),
        processorRef.matrix->getAudioTree()
    );

    command_line = std::make_unique<CLIComponent>(processorRef.matrix.get());
    

    setSize (900, 700);

    //look and feel
    setLookAndFeel(&look_and_feel);

    // footer
    addAndMakeVisible(*footer);

    // midi keyboard
    addAndMakeVisible(*midi_keyboard);

    // envelopes
    addAndMakeVisible(*envelopes);

    // lfos
    addAndMakeVisible(*lfos);

    // knob
    addAndMakeVisible(*gain_knob);
    addAndMakeVisible(*semitones_knob);

    // knob popup
    addAndMakeVisible(*knob_popup);

    // tooltip
    addAndMakeVisible(*tooltip);

    // preset browser
    addAndMakeVisible(*preset_browser);
    addChildComponent(*save_preset_popup);

    // soundfile player
    addAndMakeVisible(*soundfile_component_1);

    // CLI
    addAndMakeVisible(*command_line);

    
    // tooltip.setMillisecondsBeforeTipAppears(0);

    // resizable window
    setResizable(true, true);
    setResizeLimits(450, 350, 1350, 1050);
    getConstrainer()->setFixedAspectRatio(1.2857142857);
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
    
    auto sw = getWidth() / 900.0;
    auto sh = getHeight() / 700.0;
    command_line->setBounds(area.removeFromBottom(proportionOfHeight(0.05f)));
    tooltip->setBounds(area.removeFromBottom(proportionOfHeight(0.05)));
    // knob_popup->setBounds(area.removeFromBottom(proportionOfHeight(0.05)));
    gain_knob->setBounds(60.0f*sw, 90.0f*sh, 50.0f*sw, 50.0f*sh);
    semitones_knob->setBounds(120.0f*sw, 90.0f*sh, 50.0f*sw, 50.0f*sh);
    midi_keyboard->setBounds(area.removeFromBottom(proportionOfHeight(0.1375f)));
    footer->setBounds(area.removeFromTop(proportionOfHeight(0.0625f)));
    auto left = area.removeFromLeft(area.proportionOfWidth(0.6f));
    auto right_top = area.removeFromTop(area.proportionOfHeight(0.5f));
    envelopes->setBounds(right_top);
    lfos->setBounds(area);
    auto left_top = left.removeFromTop(proportionOfHeight(0.0625f));
    auto preset_browser_area = left_top.removeFromRight(left_top.proportionOfWidth(0.5f));
    preset_browser->setBounds(preset_browser_area);
    save_preset_popup->setBounds(getLocalBounds()*0.8f + (getLocalBounds()*0.2f).getCentre());
    auto left_under_top = left.removeFromTop(proportionOfHeight(0.0625f));
    soundfile_component_1->setBounds(left_under_top.removeFromRight(left_under_top.proportionOfWidth(0.5f)));
    // look_and_feel.setFontSize(15.0f*sw);
}


void AudioPluginAudioProcessorEditor::mouseDown (const MouseEvent& e) {
    std::cout << "MOUSE DOWN EDITOR" << std::endl;
}

void AudioPluginAudioProcessorEditor::timerCallback() {
    if (!isMouseButtonDownAnywhere()) {
        // std::cout << "begin transaction" << std::endl;
        processorRef.matrix->getUndoManager()->beginNewTransaction();
    }
}