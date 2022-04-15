#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
{
    synth.clearVoices();
    for (auto i = 0; i < NUM_VOICES; ++i) {
        synth.addVoice(new SynthVoice());
    }
    synth.setVoiceStealingEnabled(true);

    synth.enableLegacyMode();
    synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);

    // Attach Parameters to callback function
    gain_param.attachToParameter(apvts.getParameter("GAIN_PARAM"));
    gain_param.onParameterChanged = [&] { update_gain_param(); };
    mpe_param.attachToParameter(apvts.getParameter("MPE_PARAM"));
    mpe_param.onParameterChanged = [&] { update_mpe_param(); };
    envelopes.attach(apvts);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))) {
            //update voice parameters from value tree
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
    update_gain_param();
    update_mpe_param();
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Process MIDI events along with Virtual Keyboard Input
    keyboard_state.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    // for a synthesiser, clear the buffer
    buffer.clear();
    // render next block
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // // copy to fill up all channels with same info
    auto read_pointer = buffer.getReadPointer(0, 0);
    for (auto c = 1; c < totalNumOutputChannels; ++c) {
        buffer.copyFrom(c, 0, read_pointer, buffer.getNumSamples());
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

// Add parameters to apvts
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameters() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // define range outside to use in the string conversion
    auto gain_param_range = juce::NormalisableRange<float>(-60.0f, 6.0f); //, 0.01f, 1.0f); // low, high, grain, exp
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "GAIN_PARAM",  // parameter ID
        "GAIN Parameter",  // parameter name
        gain_param_range,  // range
        -12.0f,         // default value
        "", // parameter label (description?)
        juce::AudioProcessorParameter::Category::genericParameter,
        [gain_param_range](float value, int maximumStringLength) { // Float to String Precision 2 Digits
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << gain_param_range.convertTo0to1(value);
            return juce::String(ss.str());
        },
        [gain_param_range](juce::String text) {
            return gain_param_range.convertFrom0to1(text.getFloatValue()); // Convert Back to Value
        }
    ));

    params.push_back(std::make_unique<BoolParameter>("MPE_PARAM", "MPE Parameter", false, false));

    // get and add apvts parameters from each of the adsrs
    auto adsr_params = envelopes.get_apvts_params();
    params.insert(params.end(), adsr_params.begin(), adsr_params.end());

    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::update_gain_param() {
    for (int i = 0; i < synth.getNumVoices(); ++i) {
        if (auto voice = dynamic_cast<juce::MPESynthesiserVoice*>(synth.getVoice(i))) {
            //update voice parameters from value tree
            auto synthvoice = dynamic_cast<SynthVoice*>(voice);
            synthvoice->update_gain_param(gain_param.getValue());
        }
    }
}

void AudioPluginAudioProcessor::update_mpe_param() {
    if (bool(mpe_param.getValue())) {
        zone_layout.clearAllZones();
        zone_layout.setLowerZone(15, 48, 2);
        synth.setZoneLayout(zone_layout);
        synth.setPitchbendTrackingMode(MPEInstrument::lastNotePlayedOnChannel);
    }
    else {
        synth.enableLegacyMode();
        synth.setPitchbendTrackingMode(juce::MPEInstrument::allNotesOnChannel);
    }
}