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
    static constexpr int topBarHeight = 40; // H�he der Topbar f�r Buttons und Dropdown

    // Dropdown f�r Genres
    juce::ComboBox genreBox;

    // Button f�r Genre erkennen
    juce::TextButton genreErkennenButton;

    // Button f�r Reset
    juce::TextButton resetButton;

    // Hintergrundbild: Spektogramm
    juce::Image background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
