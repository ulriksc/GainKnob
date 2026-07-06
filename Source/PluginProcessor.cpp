#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    auto range = juce::NormalisableRange<float> (GainAudioProcessor::minGainDb,
                                                 GainAudioProcessor::maxGainDb,
                                                 0.01f);
    range.setSkewForCentre (0.0f);

    auto stringFromValue = [] (float value, int)
    {
        if (value <= GainAudioProcessor::minGainDb + 0.05f)
            return juce::String ("-inf dB");

        return juce::String (value, 1) + " dB";
    };

    return juce::AudioProcessorValueTreeState::ParameterLayout {
        std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { "gain", 1 },
            "Gain",
            range,
            0.0f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction (stringFromValue))
    };
}
} // namespace

GainAudioProcessor::GainAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    gainDb = apvts.getRawParameterValue ("gain");
}

void GainAudioProcessor::prepareToPlay (double sampleRate, int)
{
    smoothedGain.reset (sampleRate, 0.05);
    smoothedGain.setCurrentAndTargetValue (
        juce::Decibels::decibelsToGain (gainDb->load(), minGainDb));
}

bool GainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();

    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;

    return mainOut == layouts.getMainInputChannelSet();
}

void GainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    for (auto ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());

    smoothedGain.setTargetValue (juce::Decibels::decibelsToGain (gainDb->load(), minGainDb));
    smoothedGain.applyGain (buffer, buffer.getNumSamples());
}

juce::AudioProcessorEditor* GainAudioProcessor::createEditor()
{
    return new GainAudioProcessorEditor (*this);
}

void GainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void GainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainAudioProcessor();
}
