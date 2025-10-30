#pragma once

#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // Referenzkurve laden
    void loadReferenceCurve(const juce::String& filename);

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

    // EQ Fader mit Array erzeugen
    const std::array<float, 31> eqFrequencies
    {
        20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160,
        200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600,
        2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000, 20000
    };

    // Array f�r vertikale Linien im Spektrogramm
    juce::Array<float> frequencies
    {
        20, 40, 100,
        200, 500, 1000,
        2000, 4000, 10000, 20000
    };

    // Struct f�r Referenzkurven
    struct ReferenceBand
    {
        float freq; // Frequenz in Hz
        float p10;  // Unteres 10%-Perzentil
        float median; // Median (Zentralwert)
        float p90; // Oberes 90%-Perzentil
    };

    // Container f�r Frequenzb�nder der Referenzkurven
    std::vector<ReferenceBand> referenceBands;

    // Hintergrundbild: Spektogramm
    juce::Image background;

    // EQ Bereich einf�gen
    juce::Rectangle<int> eqArea;

    // Silder erstellen
    juce::Slider eqSlider[31];

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
