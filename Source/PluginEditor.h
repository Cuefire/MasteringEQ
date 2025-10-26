#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    // Topbar einf�gen
    juce::Rectangle<int> topBarArea;

    // Dropdown f�r Genres
    juce::ComboBox genreBox;

    // Button f�r Genre erkennen
    juce::TextButton genreErkennenButton;

    // Button f�r Reset
    juce::TextButton resetButton;

    // Spektrogramm + Beschriftung Bereich einf�gen
    juce::Rectangle<int> spectrogramArea;

    // Spektrogramm Display Bereich f�r Kurve
    juce::Rectangle<int> spectrumDisplayArea;

    // Hintergrundbild: Spektogramm
    juce::Image background;

    // EQ Bereich einf�gen
    juce::Rectangle<int> eqArea;

    // Testslider
    juce::Slider testSlider;

    // EQ Beschriftungsbereich
    juce::Rectangle<int> eqLabelArea;

    // Layout Konstanten
    static constexpr int topBarHeight = 40; // H�he der Topbar f�r Buttons und Dropdown
    static constexpr int spectrogramOuterHeight = 430;
    static constexpr int spectrogramMargin = 10;
    static constexpr int eqHeight = 180;
    static constexpr int spectrumHeight = 390;
    static constexpr int spectrumBottomMargin = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
