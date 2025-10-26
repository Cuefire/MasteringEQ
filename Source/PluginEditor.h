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

    // Topbar einfügen
    juce::Rectangle<int> topBarArea;

    // Spektrogramm Bereich einfügen
    juce::Rectangle<int> spectrogramArea;

    // EQ Bereich einfügen
    juce::Rectangle<int> eqArea;

    // Layout Konstanten
    static constexpr int topBarHeight = 40; // Höhe der Topbar für Buttons und Dropdown
    static constexpr int spectrogramOuterHeight = 430;
    static constexpr int spectrogramMargin = 10;
    static constexpr int eqHeight = 180;

    // Dropdown für Genres
    juce::ComboBox genreBox;

    // Button für Genre erkennen
    juce::TextButton genreErkennenButton;

    // Button für Reset
    juce::TextButton resetButton;

    // Hintergrundbild: Spektogramm
    juce::Image background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
