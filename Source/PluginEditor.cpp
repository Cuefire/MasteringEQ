#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    // Fenster Einstellungen
    //x-Breite, y-Höhe
    setSize(1000, 650);
    setResizable(false, false);

    // Testreferenzkurve laden
    loadReferenceCurve("test.json");

    // Dropdown
    genreBox.setTextWhenNothingSelected("Genre auswahlen...");
    genreBox.addItem("Pop", 1);
    genreBox.addItem("HipHop", 2);
    genreBox.addItem("Rock", 3);
    genreBox.addItem("EDM", 4);
    genreBox.addItem("Klassik", 5);

    // Dropdown: Auf Änderung reagieren
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

    // Silder konfigurieren
    for (int i = 0; i < 31; i++)
    {
        eqSlider[i].setSliderStyle(juce::Slider::LinearVertical);
        eqSlider[i].setRange(-12.0, 12.0, 0.1);
        eqSlider[i].setValue(0.0);
        eqSlider[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        eqSlider[i].setColour(juce::Slider::thumbColourId, juce::Colours::white);
        eqSlider[i].setColour(juce::Slider::trackColourId, juce::Colours::lightgrey);

        addAndMakeVisible(eqSlider[i]);
    }

    // Sichtbar machen
    addAndMakeVisible(genreBox);
    addAndMakeVisible(genreErkennenButton);
    addAndMakeVisible(resetButton);
}

// Referenzkurve laden
void AudioPluginAudioProcessorEditor::loadReferenceCurve(const juce::String& filename)
{
    // Speicher für Kurve leeren
    referenceBands.clear();

    // File laden
    juce::File refFileBase = juce::File::getSpecialLocation(
        juce::File::currentApplicationFile);

        // Solange im Pfad nach oben gehen, bis "build"
        for (int i = 0; i < 8; ++i)
        {
            if (refFileBase.getFileName().equalsIgnoreCase("build"))
                break;

            refFileBase = refFileBase.getParentDirectory();
        }

        // Von der "build" Ebene aus laden
        juce::File refFile = refFileBase
        .getChildFile("ReferenceCurves")
        .getChildFile(filename);

    // Inhalt von JSON in String laden
    juce::String fileContent = refFile.loadFileAsString();

    // Analysiert Text aus fileContent und erstellt dynamisches var
    juce::var jsonData = juce::JSON::parse(fileContent);

    // Bänder aus JSON erstellen
    auto bands = jsonData["bands"];

    // Bänder erstellen
    for (auto& b : *bands.getArray())
    {
        // Neues Datenobjekt
        ReferenceBand rb;

        // Bänder durchlaufen
        rb.freq = (float)b["freq"];
        rb.p10 = (float)b["p10"];
        rb.median = (float)b["median"];
        rb.p90 = (float)b["p90"];

        // Band an Vektor anhängen
        referenceBands.push_back(rb);
    }
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

    // Layout Area für alles unter der Topbar
    auto rest = getLocalBounds().withY(topBarArea.getBottom());

    // Spektogramm Hintergrundfarbe
    g.setColour(juce::Colour::fromString("ff111111"));
    g.fillRect(rest);

    // Frequenzspektrum Bereich färben
    g.setColour(juce::Colours::orange);
    g.fillRect(spectrogramArea);

    // Display Bereich färben
    g.setColour(juce::Colours::green);
    g.fillRect(spectrumDisplayArea);

    // Vertikale Frequenzlinien im Spektrogramm zeichnen
    g.setColour(juce::Colours::white.withAlpha(0.5f));

    // Schriftgröße für Achsenbeschriftung
    g.setFont(15.0f);

    // Y-Position für Achsenbeschriftung
    float textY = (float)spectrumDisplayArea.getBottom() + 3.0f;

    for (auto f : frequencies)
    {
        // Frequenzen in 0-1 Bereich umrechnen
        float normX = juce::mapFromLog10(f, 17.0f, 23700.0f);

        // Normierten Bereich (0-1) auf grünen Bereich skalieren
        float x = spectrumDisplayArea.getX() + normX * spectrumDisplayArea.getWidth();

        // Vertikale Linie innerhalb des grünen Bereichs zeichnen
        g.drawVerticalLine(
            static_cast<int>(x),
            (float)spectrumDisplayArea.getY(),                          // obere Grenze
            (float)spectrumDisplayArea.getBottom()                      // untere Grenze
        );

        // Achsenbeschriftung einfügen
        juce::String text;
        if (f >= 1000.0f)
            text = juce::String(f / 1000.0f) + "k";
        else
            text = juce::String((int)f);

        // Achsenbeschriftung einfügen
        g.drawFittedText(
            text,
            (int)(x - 15), // x-Position: Nach links verschieben
            (int)textY, // y-Position
            30, // Textbox-Breite
            15, // Textbox-Höhe
            juce::Justification::centred, // zentrieren
            1 // max. Anzahl an Zeilen
        );
    }

    // Referenzbänder zeichnen
    if (!referenceBands.empty())
    {
        // Konstanten für die Skalierung
        const float minFreq = 20.0f;
        const float maxFreq = 20000.0f;
        const float minDb = -25.0f;
        const float maxDb = 5.0f;

        // Pfade für die Linienpunkte
        juce::Path pathP10;
        juce::Path pathP90;
        juce::Path pathMedian;

        // Überprüfen, ob aktueller Punkt der erste ist
        bool firstPoint = true;

        // Schleife für alle Bänder
        for (const auto& band : referenceBands)
        {
            // x-Position normieren
            float normX = juce::mapFromLog10(band.freq, minFreq, maxFreq);

            // x-Position auf Fenster skalieren
            float x = spectrumDisplayArea.getX() + normX * spectrumDisplayArea.getWidth();

            // y-Positionen invertieren (oben = laut)
            float yP10 = juce::jmap(band.p10, minDb, maxDb,
                (float)spectrumDisplayArea.getBottom(),
                (float)spectrumDisplayArea.getY());

            float yMedian = juce::jmap(band.median, minDb, maxDb,
                (float)spectrumDisplayArea.getBottom(),
                (float)spectrumDisplayArea.getY());

            float yP90 = juce::jmap(band.p90, minDb, maxDb,
                (float)spectrumDisplayArea.getBottom(),
                (float)spectrumDisplayArea.getY());

            // Neue Linie beim ersten Punkt der JSON Datei
            if (firstPoint)
            {
                pathP10.startNewSubPath(x, yP10);
                pathP90.startNewSubPath(x, yP90);
                pathMedian.startNewSubPath(x, yMedian);
                firstPoint = false;
            }
            // Sonst Linie verbinden
            else
            {
                pathP10.lineTo(x, yP10);
                pathP90.lineTo(x, yP90);
                pathMedian.lineTo(x, yMedian);
            }
        }

        // Zeichnen der Referenlinien
        g.setColour(juce::Colours::blue.withAlpha(0.6f)); // untere (p10)
        g.strokePath(pathP10, juce::PathStrokeType(1.5f));

        g.setColour(juce::Colours::blue.withAlpha(0.6f)); // obere (p90)
        g.strokePath(pathP90, juce::PathStrokeType(1.5f));

        g.setColour(juce::Colours::grey); // Median
        g.strokePath(pathMedian, juce::PathStrokeType(2.0f));
    }







    // EQ Bereich färben
    g.setColour(juce::Colours::blue);
    g.fillRect(eqArea);

    // EQ Beschriftungsbereich
    g.setColour(juce::Colours::red);
    g.fillRect(eqLabelArea);

    // Slider Beschriftung
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.setFont(14.0f);

    for (int i = 0; i < eqFrequencies.size(); i++)
    {
        // Positionen normieren
        float normX = juce::mapFromLog10(eqFrequencies[i], 16.0f, 25500.f);

        // Auf den EQ Bereich skalieren
        int x = eqArea.getX() + static_cast<int>(normX * eqArea.getWidth());

        // Textdarstellung anpassen
        juce::String label;
        if (eqFrequencies[i] >= 1000.0f)
        {
            float valueInK = eqFrequencies[i] / 1000.0f;
        
            if (valueInK >= 10.0f)
                label = juce::String((int)valueInK) + "k";
            else
                label = juce::String(valueInK, 1) + "k";
        }
        else
        {
            label = juce::String((int)eqFrequencies[i]);
        }

        // Text einfügen
        g.drawFittedText(
            label,
            x - 20, // x-Position: Nach links verschieben
            eqLabelArea.getY() + 5, // y-Position: Oberer Rand vom roten Bereich + kleiner Abstand
            40, // Textbox-Breite
            20, // Textbox-Höhe
            juce::Justification::centred, // zentrieren
            1 // max. Anzahl an Zeilen
        );
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Hauptbereich "Area"
    auto area = getLocalBounds();

    // Topbar abtrennen
    topBarArea = area.removeFromTop(topBarHeight);

    // Dropdown Position (x-Position, y-Position, x-Breite, y-Höhe)
    const int barDropW = 220;
    const int barDropH = 30;
    genreBox.setBounds(710, 5, 220, 30);

    // Button Position (x-Position, y-Position, x-Breite, y-Höhe)
    genreErkennenButton.setBounds(10, 5, 120, 30);
    resetButton.setBounds(940, 5, 50, 30);

    // Restbereich unter der Topbar
    auto rest = area;

    // Äußerer Bereich vom Spektrogramm
    auto spectroOuter = rest.removeFromTop(spectrogramOuterHeight);

    // Innerer Bereich vom Spektrogramm
    spectrogramArea = spectroOuter.reduced(spectrogramMargin);

    // Spektrogramm Display Bereich
    spectrumDisplayArea = spectrogramArea.removeFromTop(spectrumHeight);

    // EQ Bereich
    eqArea = rest.removeFromTop(eqHeight);

    // Silder
    for (int i = 0; i < 31; ++i)
    {
        // Frequenz in log-Skala umrechnen
        float normX = juce::mapFromLog10(eqFrequencies[i], 16.0f, 25500.0f);

        // Auf den Bereich vom EQ skalieren
        int x = eqArea.getX() + static_cast<int>(normX * eqArea.getWidth());

        int sliderWidth = 16;
        int sliderHeight = eqArea.getHeight() - 40;

        eqSlider[i].setBounds(
            x - sliderWidth / 2,
            eqArea.getY() + 10,
            sliderWidth,
            sliderHeight
        );
    }
    
    // EQ Beschriftung
    eqLabelArea = eqArea.removeFromBottom(30);

    
}
