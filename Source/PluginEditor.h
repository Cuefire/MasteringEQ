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
    static constexpr int topBarHeight = 40; // Höhe der Topbar für Buttons und Dropdown

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
