#include "DummyProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DummyProcessor::DummyProcessor()
{

}

DummyProcessor::~DummyProcessor()
{
}

//==============================================================================
void DummyProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

}


void DummyProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{

}

juce::AudioProcessorEditor* DummyProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void DummyProcessor::getStateInformation (juce::MemoryBlock& destData)
{

}

void DummyProcessor::setStateInformation (const void* data, int sizeInBytes)
{

}

//==============================================================================
// This creates new instances of the plugin..
// juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
// {
//     return new DummyProcessor();
// }