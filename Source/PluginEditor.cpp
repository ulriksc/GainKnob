#include "PluginEditor.h"

namespace colours
{
const juce::Colour background { 0xff14171c };
const juce::Colour track      { 0xff2a2f37 };
const juce::Colour accent     { 0xffff8c42 };
const juce::Colour knobTop    { 0xff3a404a };
const juce::Colour knobBottom { 0xff23272e };
const juce::Colour knobRim    { 0xff4a515c };
const juce::Colour text       { 0xffe8eaed };
const juce::Colour textDim    { 0xff9aa0a6 };
} // namespace colours

KnobLookAndFeel::KnobLookAndFeel()
{
    setColour (juce::Slider::rotarySliderFillColourId, colours::accent);
    setColour (juce::Slider::rotarySliderOutlineColourId, colours::track);
}

void KnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPos, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    auto trackWidth = juce::jmax (4.0f, radius * 0.085f);
    auto arcRadius = radius - trackWidth * 0.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                                 rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (colours::track);
    g.strokePath (backgroundArc, juce::PathStrokeType (trackWidth,
                                                       juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                            rotaryStartAngle, toAngle, true);
    g.setColour (colours::accent);
    g.strokePath (valueArc, juce::PathStrokeType (trackWidth,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));

    auto knobRadius = arcRadius - trackWidth * 1.8f;
    auto knobBounds = juce::Rectangle<float> (knobRadius * 2.0f, knobRadius * 2.0f)
                          .withCentre (centre);

    g.setGradientFill (juce::ColourGradient (colours::knobTop, centre.x, knobBounds.getY(),
                                             colours::knobBottom, centre.x, knobBounds.getBottom(),
                                             false));
    g.fillEllipse (knobBounds);
    g.setColour (colours::knobRim);
    g.drawEllipse (knobBounds, 1.5f);

    juce::Path pointer;
    auto pointerLength = knobRadius * 0.35f;
    auto pointerThickness = juce::jmax (3.0f, trackWidth * 0.7f);
    pointer.addRoundedRectangle (-pointerThickness * 0.5f,
                                 -knobRadius + knobRadius * 0.12f,
                                 pointerThickness, pointerLength,
                                 pointerThickness * 0.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (toAngle)
                                .translated (centre.x, centre.y));
    g.setColour (colours::accent);
    g.fillPath (pointer);
}

GainAudioProcessorEditor::GainAudioProcessorEditor (GainAudioProcessor& p)
    : AudioProcessorEditor (p),
      processorRef (p),
      gainAttachment (p.apvts, "gain", gainSlider)
{
    gainSlider.setLookAndFeel (&knobLookAndFeel);
    gainSlider.setDoubleClickReturnValue (true, 0.0);
    gainSlider.onValueChange = [this] { updateValueLabel(); };
    addAndMakeVisible (gainSlider);

    titleLabel.setText ("GAIN", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    titleLabel.setColour (juce::Label::textColourId, colours::textDim);
    addAndMakeVisible (titleLabel);

    valueLabel.setJustificationType (juce::Justification::centred);
    valueLabel.setFont (juce::FontOptions (22.0f));
    valueLabel.setColour (juce::Label::textColourId, colours::text);
    addAndMakeVisible (valueLabel);
    updateValueLabel();

    setSize (320, 400);
}

GainAudioProcessorEditor::~GainAudioProcessorEditor()
{
    gainSlider.setLookAndFeel (nullptr);
}

void GainAudioProcessorEditor::updateValueLabel()
{
    auto db = (float) gainSlider.getValue();

    if (db <= GainAudioProcessor::minGainDb + 0.05f)
    {
        valueLabel.setText ("-inf dB", juce::dontSendNotification);
        return;
    }

    auto text = juce::String (db, 1) + " dB";

    if (db > 0.0f)
        text = "+" + text;

    valueLabel.setText (text, juce::dontSendNotification);
}

void GainAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (colours::background);
}

void GainAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    titleLabel.setBounds (area.removeFromTop (44));
    valueLabel.setBounds (area.removeFromBottom (40));
    gainSlider.setBounds (area.reduced (4));
}
