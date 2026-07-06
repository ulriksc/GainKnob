#pragma once

#include "PluginProcessor.h"

// Draws the big knob: background track arc, orange value arc,
// gradient-shaded body, and a pointer line.
class KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KnobLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;
};

class GainAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit GainAudioProcessorEditor (GainAudioProcessor&);
    ~GainAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void updateValueLabel();

    GainAudioProcessor& processorRef;

    KnobLookAndFeel knobLookAndFeel;
    juce::Slider gainSlider { juce::Slider::RotaryHorizontalVerticalDrag,
                              juce::Slider::NoTextBox };
    juce::Label titleLabel;
    juce::Label valueLabel;

    juce::AudioProcessorValueTreeState::SliderAttachment gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GainAudioProcessorEditor)
};
