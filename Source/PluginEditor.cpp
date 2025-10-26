#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.


    // Fenster Einstellungen
    //x-Breite, y-H�he
    setSize(1000, 650);
    setResizable(false, false);

    // Dropdown
    genreBox.setTextWhenNothingSelected("Genre auswahlen...");
    genreBox.addItem("Pop", 1);
    genreBox.addItem("HipHop", 2);
    genreBox.addItem("Rock", 3);
    genreBox.addItem("EDM", 4);
    genreBox.addItem("Klassik", 5);

    // Dropdown: Auf �nderung reagieren
    genreBox.onChange = [this]
        {
            const int id = genreBox.getSelectedId();
            repaint();
        };

    // Button
    genreErkennenButton.setButtonText("Genre erkennen");
    genreErkennenButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

    resetButton.setButtonText("Reset");
    resetButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);

    // Sichtbar machen
    addAndMakeVisible(genreBox);
    addAndMakeVisible(genreErkennenButton);
    addAndMakeVisible(resetButton);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Topbar Farbe
    g.setColour(juce::Colour::fromString("ff2c2f33"));
    g.fillRect(topBarArea);

    // Layout in der Topbar, Abstand Links/Rechts, Oben/Unten
    auto top = topBarArea.reduced(12, 8);

    // Layout Area f�r alles unter der Topbar
    auto rest = getLocalBounds().withY(topBarArea.getBottom());

    // Spektogramm Hintergrundfarbe
    g.setColour(juce::Colour::fromString("ff111111"));
    g.fillRect(rest);

    // Frequenzspektrum Bereich f�rben
    g.setColour(juce::Colours::orange);
    g.fillRect(spectrogramArea);

    // Array f�r vertikale Linien im Spektrogramm
        juce::Array<float> frequencies
    {
        40, 100,
        200, 500, 1000,
        2000, 4000, 10000,
    };

    // Display Bereich f�rben
    g.setColour(juce::Colours::green);
    g.fillRect(spectrumDisplayArea);

    // Vertikale Frequenzlinien im Spektrogramm zeichnen
    g.setColour(juce::Colours::white.withAlpha(0.5f));

    // Schriftgr��e f�r Achsenbeschriftung
    g.setFont(15.0f);

    // Y-Position f�r Achsenbeschriftung
    float textY = (float)spectrumDisplayArea.getBottom() + 3.0f;

    for (auto f : frequencies)
    {
        // Frequenzen in 0-1 Bereich umrechnen
        float normX = juce::mapFromLog10(f, 20.0f, 20000.0f);

        // Normierten Bereich (0-1) auf gr�nen Bereich skalieren
        float x = spectrumDisplayArea.getX() + normX * spectrumDisplayArea.getWidth();

        // Vertikale Linie innerhalb des gr�nen Bereichs zeichnen
        g.drawVerticalLine(
            static_cast<int>(x),
            (float)spectrumDisplayArea.getY(),                          // obere Grenze
            (float)spectrumDisplayArea.getBottom()                      // untere Grenze
        );

        // Achsenbeschriftung einf�gen
        juce::String text;
        if (f >= 1000.0f)
            text = juce::String(f / 1000.0f) + "k";
        else
            text = juce::String((int)f);

        // Achsenbeschriftung einf�gen
        g.drawFittedText(
            text,
            (int)(x - 15), // x-Position: Nach links verschieben
            (int)textY, // y-Position
            30, // Textbox-Breite
            15, // Textbox-H�he
            juce::Justification::centred, // zentrieren
            1 // max. Anzahl an Zeilen
        );
    }

    // EQ Bereich f�rben
    g.setColour(juce::Colours::blue);
    g.fillRect(eqArea);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Hauptbereich "Area"
    auto area = getLocalBounds();

    // Topbar abtrennen
    topBarArea = area.removeFromTop(topBarHeight);

    // Dropdown Position (x-Position, y-Position, x-Breite, y-H�he)
    const int barDropW = 220;
    const int barDropH = 30;
    genreBox.setBounds(710, 5, 220, 30);

    // Button Position (x-Position, y-Position, x-Breite, y-H�he)
    genreErkennenButton.setBounds(10, 5, 120, 30);
    resetButton.setBounds(940, 5, 50, 30);

    // Restbereich unter der Topbar
    auto rest = area;

    // �u�erer Bereich vom Spektrogramm
    auto spectroOuter = rest.removeFromTop(spectrogramOuterHeight);

    // Innerer Bereich vom Spektrogramm
    spectrogramArea = spectroOuter.reduced(spectrogramMargin);

    // Spektrogramm Display Bereich
    spectrumDisplayArea = spectrogramArea.removeFromTop(spectrumHeight);

    // EQ Bereich
    eqArea = rest.removeFromTop(eqHeight);

    
}
