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

    speedButton.setClickingTogglesState(true);
    speedButton.setToggleState(processor.fast, dontSendNotification);
    speedButton.setButtonText(processor.fast ? "Spin DOWN" : "Spin UP");
    addAndMakeVisible(speedButton);

    directKnob.setDoubleClickReturnValue(true, double(MOrganCabParameters::directDefault), ModifierKeys::noModifiers);
    directKnob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledDirectKnob);
    leslie1Knob.setDoubleClickReturnValue(true, double(MOrganCabParameters::leslie1Default), ModifierKeys::noModifiers);
    leslie1Knob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledLeslie1Knob);
    leslie2Knob.setDoubleClickReturnValue(true, double(MOrganCabParameters::leslie2Default), ModifierKeys::noModifiers);
    leslie2Knob.setFillColour(Colour(108, 20, 21).darker());
    addAndMakeVisible(labeledLeslie2Knob);

    processor.parameters.attachControls(speedButton, directKnob, leslie1Knob, leslie2Knob );

    midiModeLabel.setText("MIDI Control", NotificationType::dontSendNotification);
    midiModeLabel.attachToComponent(&midiModeCombo, true);

    int id = 0;
    midiModeCombo.addItem("None", ++id);
    midiModeCombo.addItem("Sustain Pedal", ++id);
    midiModeCombo.addItem("Mod Wheel", ++id);
    midiModeCombo.addItem("Sus + MW", ++id);
    midiModeCombo.addItem("Sus Toggle", ++id);
    midiModeCombo.addItem("MW Toggle", ++id);
    midiModeCombo.addItem("Sus+MW Toggle", ++id);

    midiModeCombo.setSelectedItemIndex(processor.midiControlMode);
    midiModeCombo.onChange = [this]()
    {
        processor.midiControlMode = MOrganCabProcessor::MidiControlMode(midiModeCombo.getSelectedItemIndex());
    };
    addAndMakeVisible(midiModeCombo);

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

    auto col = area.removeFromLeft(220).reduced(0, 4);
    speedButton.setBounds(col.removeFromTop(24));
    col.removeFromTop(10);
    auto row = col.removeFromTop(24);
    row.removeFromLeft(80);
    midiModeCombo.setBounds(row);
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
    midiModeCombo.setSelectedItemIndex(processor.midiControlMode, dontSendNotification);
    speedButton.setButtonText(processor.fast ? "Spin DOWN" : "Spin UP");
}
