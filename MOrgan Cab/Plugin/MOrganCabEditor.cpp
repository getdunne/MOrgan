#include "MOrganCabProcessor.h"
#include "MOrganCabEditor.h"

MOrganCabEditor::MOrganCabEditor (MOrganCabProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , directKnob(MOrganCabParameters::directMin, MOrganCabParameters::directMax, MOrganCabParameters::directMin, "OFF")
    , labeledDirectKnob(MOrganCabParameters::directName, directKnob)
    , leslie1Knob(MOrganCabParameters::leslie1Min, MOrganCabParameters::leslie1Max, MOrganCabParameters::leslie1Min, "OFF")
    , labeledLeslie1Knob(MOrganCabParameters::leslie1Name, leslie1Knob)
    , leslie2Knob(MOrganCabParameters::leslie2Min, MOrganCabParameters::leslie2Max, MOrganCabParameters::leslie2Min, "OFF")
    , labeledLeslie2Knob(MOrganCabParameters::leslie2Name, leslie2Knob)
    , infoButton("More info...", URL("https://github.com/getdunne/morgan"))
{
    setLookAndFeel(lookAndFeel);

    directKnob.setDoubleClickReturnValue(true, double(MOrganCabParameters::directDefault), ModifierKeys::noModifiers);
    directKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledDirectKnob);
    leslie1Knob.setDoubleClickReturnValue(true, double(MOrganCabParameters::leslie1Default), ModifierKeys::noModifiers);
    leslie1Knob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledLeslie1Knob);
    leslie2Knob.setDoubleClickReturnValue(true, double(MOrganCabParameters::leslie2Default), ModifierKeys::noModifiers);
    leslie2Knob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledLeslie2Knob);

    processor.parameters.attachControls(
        directKnob,
        leslie1Knob,
        leslie2Knob );

    leslieModeLabel.setText("Leslie Speed", NotificationType::dontSendNotification);
    leslieModeLabel.attachToComponent(&leslieModeCombo, true);

    leslieModeCombo.addItem("Mod Wheel", 1);
    leslieModeCombo.addItem("Sustain Pedal", 2);
    leslieModeCombo.setSelectedItemIndex(processor.pedalLeslieMode ? 1 : 0);
    leslieModeCombo.onChange = [this]()
    {
        processor.pedalLeslieMode = leslieModeCombo.getSelectedItemIndex() > 0;
    };
    addAndMakeVisible(leslieModeCombo);

    logoImage = ImageCache::getFromMemory(BinaryData::LogoM_png, BinaryData::LogoM_pngSize);
    float whRatio = float(logoImage.getWidth()) / logoImage.getHeight();
    int logoHeight = 90;
    logoImage = logoImage.rescaled(int(0.5 + logoHeight * whRatio), logoHeight);

    addAndMakeVisible(infoButton);
    
    processor.addChangeListener(this);

    setSize (640, 130);
}

MOrganCabEditor::~MOrganCabEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void MOrganCabEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromRight(logoImage.getWidth() + 20);

    auto col = area.removeFromLeft(220).reduced(0, 12);
    col.removeFromTop(10);
    auto row = col.removeFromTop(24);
    row.removeFromLeft(80);
    leslieModeCombo.setBounds(row);
    row = col.removeFromBottom(24);
    row.removeFromRight(20);
    infoButton.setBounds(row);

    area.removeFromLeft(20);
    int knobWidth = area.getWidth() / 3;
    labeledDirectKnob.setBounds(area.removeFromLeft(knobWidth));
    labeledLeslie1Knob.setBounds(area.removeFromLeft(knobWidth));
    labeledLeslie2Knob.setBounds(area);
}

void MOrganCabEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colour(42, 38, 37));

    auto area = getLocalBounds().reduced(20);
    g.drawImage(logoImage, area.toFloat().removeFromRight(float(logoImage.getWidth())));
}

void MOrganCabEditor::changeListenerCallback(ChangeBroadcaster*)
{
    leslieModeCombo.setSelectedItemIndex(processor.pedalLeslieMode ? 1 : 0, dontSendNotification);
}
