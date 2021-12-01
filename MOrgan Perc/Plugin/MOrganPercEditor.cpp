#include "MOrganPercProcessor.h"
#include "MOrganPercEditor.h"

MOrganPercEditor::MOrganPercEditor (MOrganPercProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gateTimeKnob(MOrganPercParameters::gateTimeMin, MOrganPercParameters::gateTimeMax, MOrganPercParameters::gateTimeLabel)
    , labeledGateTimeKnob(MOrganPercParameters::gateTimeName, gateTimeKnob, 4)
    , bbBox(Colour(42, 38, 37).brighter(0.12f), Colour(108, 20, 21))
    , legatoTimeKnob(MOrganPercParameters::legatoTimeMin, MOrganPercParameters::legatoTimeMax, MOrganPercParameters::legatoTimeLabel)
    , labeledLegatoTimeKnob(MOrganPercParameters::legatoTimeName, legatoTimeKnob)
    , decayRateKnob(MOrganPercParameters::decayRateMin, MOrganPercParameters::decayRateMax, MOrganPercParameters::decayRateLabel)
    , labeledDecayRateKnob(MOrganPercParameters::decayRateName, decayRateKnob)
    , plot(processor.getDecayTable())
    , infoButton("More info...", URL("https://github.com/getdunne/MOrgan"))
{
    setLookAndFeel(lookAndFeel);

    addAndMakeVisible(bbBox);

    gateTimeKnob.setDoubleClickReturnValue(true, double(MOrganPercParameters::gateTimeDefault), ModifierKeys::noModifiers);
    gateTimeKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledGateTimeKnob);

    legatoTimeKnob.setDoubleClickReturnValue(true, double(MOrganPercParameters::legatoTimeDefault), ModifierKeys::noModifiers);
    legatoTimeKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledLegatoTimeKnob);

    decayRateKnob.setDoubleClickReturnValue(true, double(MOrganPercParameters::decayRateDefault), ModifierKeys::noModifiers);
    decayRateKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledDecayRateKnob);

    processor.parameters.attachControls(gateTimeKnob, legatoTimeKnob, decayRateKnob);

    addAndMakeVisible(plot);

    enableOrDisableHammondControls();

    triggerModeCombo.addItem("Polyphonic", 1);
    triggerModeCombo.addItem("Simple Mono", 2);
    triggerModeCombo.addItem("Hammond Perc", 3);
    triggerModeCombo.setSelectedItemIndex(processor.triggerMode, dontSendNotification);
    triggerModeCombo.onChange = [this]()
    {
        processor.triggerMode = triggerModeCombo.getSelectedItemIndex();
        enableOrDisableHammondControls();
    };
    addAndMakeVisible(triggerModeCombo);

    logoImage = ImageCache::getFromMemory(BinaryData::LogoM_png, BinaryData::LogoM_pngSize);
    float whRatio = float(logoImage.getWidth()) / logoImage.getHeight();
    int logoHeight = 100;
    logoImage = logoImage.rescaled(int(0.5 + logoHeight * whRatio), logoHeight);

    infoButton.setJustificationType(Justification::right);
    addAndMakeVisible(infoButton);

    processor.addChangeListener(this);

    setSize(450, 174);
}

MOrganPercEditor::~MOrganPercEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void MOrganPercEditor::enableOrDisableHammondControls()
{
    bool hammondMode = processor.triggerModeIsHammond();
    labeledLegatoTimeKnob.setVisible(hammondMode);
    labeledDecayRateKnob.setVisible(hammondMode);
    plot.setVisible(hammondMode);
}

void MOrganPercEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    // More Info button at bottom right
    auto row = area.removeFromBottom(24);
    infoButton.setBounds(row.removeFromRight(100));
    area.removeFromBottom(10);

    // Logo at right
    area.removeFromRight(logoImage.getWidth() + 30);

    // Gate Time knob
    labeledGateTimeKnob.setBounds(area.removeFromLeft(80));
    area.removeFromLeft(20);

    // Hammond perc sim controls
    bbBox.setBounds(area);
    triggerModeCombo.setBounds(area.removeFromTop(24));
    area.reduce(6, 4); //area.removeFromTop(4); area.removeFromBottom(10);
    plot.setBounds(area.removeFromRight(area.getHeight()).reduced(10));
    int width = (area.getWidth() - 10) / 2;
    labeledLegatoTimeKnob.setBounds(area.removeFromLeft(width));
    area.removeFromLeft(10);
    labeledDecayRateKnob.setBounds(area);
}

void MOrganPercEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colour(42, 38, 37));

    auto area = getLocalBounds().reduced(20);
    area.removeFromBottom(34);
    g.drawImage(logoImage, area.toFloat().removeFromRight(float(logoImage.getWidth())));
}

void MOrganPercEditor::changeListenerCallback(ChangeBroadcaster*)
{
    triggerModeCombo.setSelectedItemIndex(processor.triggerMode, dontSendNotification);
    plot.repaint();
    enableOrDisableHammondControls();
}
