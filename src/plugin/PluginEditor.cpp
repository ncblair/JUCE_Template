#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    // create components at top (so they are not null ptrs when we set bounds)
    footer = std::make_unique<FooterComponent>(processorRef.apvts);
    midi_keyboard = std::make_unique<MidiKeyboardComponent>(
                        processorRef.keyboard_state, 
                        juce::KeyboardComponentBase::Orientation::horizontalKeyboard
    );
    envelopes = std::make_unique<ADSRParentComponent>(
        processorRef.apvts, 
        3 // number of envelope sub-components
    );

    gain_knob = std::make_unique<LabeledKnobComponent>(
        processorRef.apvts, // apvts
        "LEVEL", // apvts param ID
        "Gain (in decibels) of the synth voice", // Knob Tooltip
        "Level" // Knob Label
    );

    setSize (600, 400);
    //look and feel
    setLookAndFeel(&look_and_feel);

    //sliders
    // addAndMakeVisible(gain_slider);
    // gain_slider_attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, "GAIN", gain_slider);
    // gain_slider.setRotaryParameters (M_PI * 5.0f / 4.0f, M_PI * 11.0f / 4.0f, true);
    // gain_slider.setTooltip("Gain in Decibels Slider");

    // footer
    addAndMakeVisible(*footer);

    // midi keyboard
    addAndMakeVisible(*midi_keyboard);

    // envelopes
    addAndMakeVisible(*envelopes);

    // knob
    addAndMakeVisible(*gain_knob);

    // tooltip
    addAndMakeVisible(tooltip);
    tooltip.setMillisecondsBeforeTipAppears(0);

    // // labels
    // addAndMakeVisible(gain_label);
    // gain_label.setJustificationType(juce::Justification::centred);
    // // gain_label.setText ("Gain", juce::dontSendNotification);
    // gain_label.attachToComponent (&gain_slider, false); // false sets to top, true sets to left



    // resizable window
    setResizable(true, true);
    setResizeLimits(510, 340, 1005, 670);
    getConstrainer()->setFixedAspectRatio(1.5);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // auto sw = getWidth() / 600.0;
    // auto sh = getHeight() / 400.0;
    g.fillAll (juce::Colours::white);

    // g.setColour (juce::Colours::black);
    // // g.setFont (15.0f);

    // g.drawImage(bg_img, 0, 0, getWidth(), 375*sh, 0, 0, bg_img.getWidth(), bg_img.getHeight());

    // juce::Line<float> line1 (juce::Point<float> (0*sw, 375*sh),
    //                         juce::Point<float> (600*sw, 375*sh));
    
    // juce::Line<float> line2 (juce::Point<float> (200*sw, 375*sh),
    //                         juce::Point<float> (200*sw, 400*sh));
    
    // juce::Line<float> line3 (juce::Point<float> (400*sw, 375*sh),
    //                         juce::Point<float> (400*sw, 400*sh));
 
    // g.drawLine (line1, 0.5f*sh);
    // g.drawLine (line2, 0.5f*sw);
    // g.drawLine (line3, 0.5f*sw);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    //bad encapsulation but: remove tooltip area here AND in CustomLookAndFeel
    area.removeFromBottom(proportionOfHeight(look_and_feel.tooltip_height));
    auto sw = getWidth() / 600.0;
    auto sh = getHeight() / 400.0;
    gain_knob->setBounds(60.0f*sw, 90.0f*sh, 50.0f*sw, 50.0f*sh);
    midi_keyboard->setBounds(area.removeFromBottom(proportionOfHeight(0.1375f)));
    footer->setBounds(area.removeFromTop(proportionOfHeight(0.0625f)));
    envelopes->setBounds(area);
    // title_label.setBounds(0, 361, 136.5, 39);
    // created_by_label.setBounds(380.75, 361, 219.25, 39);
    look_and_feel.setFontSize(15.0f*sw);
}

