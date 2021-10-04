#include "MOrganPercProcessor.h"
#include "MOrganPercEditor.h"

MOrganPercEditor::MOrganPercEditor (MOrganPercProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , gateTimeKnob(MOrganPercParameters::gateTimeMin, MOrganPercParameters::gateTimeMax, MOrganPercParameters::gateTimeLabel)
    , labeledGateTimeKnob(MOrganPercParameters::gateTimeName, gateTimeKnob)
    , decayRateKnob(MOrganPercParameters::decayRateMin, MOrganPercParameters::decayRateMax, MOrganPercParameters::decayRateLabel)
    , labeledDecayRateKnob(MOrganPercParameters::decayRateName, decayRateKnob)
    , plot(processor.getDecayTable())
    , infoButton("More info...", URL("https://github.com/getdunne/morgan"))
{
    setLookAndFeel(lookAndFeel);

    gateTimeKnob.setDoubleClickReturnValue(true, double(MOrganPercParameters::gateTimeDefault), ModifierKeys::noModifiers);
    gateTimeKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledGateTimeKnob);
    decayRateKnob.setDoubleClickReturnValue(true, double(MOrganPercParameters::decayRateDefault), ModifierKeys::noModifiers);
    decayRateKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledDecayRateKnob);

    processor.parameters.attachControls(
        gateTimeKnob,
        decayRateKnob );

    addAndMakeVisible(plot);

    logoImage = ImageCache::getFromMemory(BinaryData::LogoM_png, BinaryData::LogoM_pngSize);
    float whRatio = float(logoImage.getWidth()) / logoImage.getHeight();
    int logoHeight = 100;
    logoImage = logoImage.rescaled(int(0.5 + logoHeight * whRatio), logoHeight);

    addAndMakeVisible(infoButton);
    processor.addChangeListener(this);

    setSize(460, 174);
}

MOrganPercEditor::~MOrganPercEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void MOrganPercEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    auto row = area.removeFromBottom(24);
    infoButton.setBounds(row.removeFromLeft(120));
    area.removeFromBottom(10);

    area.removeFromRight(logoImage.getWidth() + 20);
    plot.setBounds(area.removeFromRight(100));
    area.removeFromRight(20);

    int width = (area.getWidth() - 10) / 2;
    labeledGateTimeKnob.setBounds(area.removeFromLeft(width));
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
    plot.repaint();
}
