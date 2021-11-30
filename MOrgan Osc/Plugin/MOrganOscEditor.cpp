#include "MOrganOscProcessor.h"
#include "MOrganOscEditor.h"

MOrganOscEditor::MOrganOscEditor (MOrganOscProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
    , drawBar1(Colour(108, 20, 21), Colours::white, "16'")
    , drawBar2(Colour(108, 20, 21), Colours::white, "5 1/3'")
    , drawBar3(Colour(200, 200, 200), Colours::black, "8'")
    , drawBar4(Colour(200, 200, 200), Colours::black, "4'")
    , drawBar5(Colour(40, 40, 40), Colours::white, "2 2/3'")
    , drawBar6(Colour(200, 200, 200), Colours::black, "2'")
    , drawBar7(Colour(40, 40, 40), Colours::white, "1 3/5'")
    , drawBar8(Colour(40, 40, 40), Colours::white, "1 1/3'")
    , drawBar9(Colour(200, 200, 200), Colours::black, "1'")
    , ampAttackKnob(MOrganOscParameters::ampAttackMin, MOrganOscParameters::ampAttackMax, MOrganOscParameters::ampAttackLabel)
    , labeledAmpAttackKnob(MOrganOscParameters::ampAttackName, ampAttackKnob)
    , ampDecayKnob(MOrganOscParameters::ampDecayMin, MOrganOscParameters::ampDecayMax, MOrganOscParameters::ampDecayLabel)
    , labeledAmpDecayKnob(MOrganOscParameters::ampDecayName, ampDecayKnob)
    , ampSustainKnob(MOrganOscParameters::ampSustainMin, MOrganOscParameters::ampSustainMax, MOrganOscParameters::ampSustainLabel)
    , labeledAmpSustainKnob(MOrganOscParameters::ampSustainName, ampSustainKnob)
    , ampReleaseKnob(MOrganOscParameters::ampReleaseMin, MOrganOscParameters::ampReleaseMax, MOrganOscParameters::ampReleaseLabel)
    , labeledAmpReleaseKnob(MOrganOscParameters::ampReleaseName, ampReleaseKnob)
    , infoButton("More info...", URL("https://github.com/getdunne/MOrgan"))
{
    setLookAndFeel(lookAndFeel);
    processor.addChangeListener(this);

    addAndMakeVisible(drawBar1);
    addAndMakeVisible(drawBar2);
    addAndMakeVisible(drawBar3);
    addAndMakeVisible(drawBar4);
    addAndMakeVisible(drawBar5);
    addAndMakeVisible(drawBar6);
    addAndMakeVisible(drawBar7);
    addAndMakeVisible(drawBar8);
    addAndMakeVisible(drawBar9);

    sustainToggle.setButtonText("Pedal Sustain");
    sustainToggle.setToggleState(processor.enablePedalSustain, dontSendNotification);
    sustainToggle.onStateChange = [this]()
    {
        processor.enablePedalSustain = sustainToggle.getToggleState();
    };
    addAndMakeVisible(sustainToggle);

    vibratoToggle.setButtonText("MW Vibrato");
    vibratoToggle.setToggleState(processor.enableModwheelVibrato, dontSendNotification);
    vibratoToggle.onStateChange = [this]()
    {
        processor.enableModwheelVibrato = vibratoToggle.getToggleState();
    };
    addAndMakeVisible(vibratoToggle);

    ampAttackKnob.setFillColour(Colour(108, 20, 21).darker());
    ampAttackKnob.setDoubleClickReturnValue(true, double(MOrganOscParameters::ampAttackDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAmpAttackKnob);
    ampDecayKnob.setFillColour(Colour(108, 20, 21).darker());
    ampDecayKnob.setDoubleClickReturnValue(true, double(MOrganOscParameters::ampDecayDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAmpDecayKnob);
    ampSustainKnob.setFillColour(Colour(108, 20, 21).darker());
    ampSustainKnob.setDoubleClickReturnValue(true, double(MOrganOscParameters::ampSustainDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAmpSustainKnob);
    ampReleaseKnob.setFillColour(Colour(108, 20, 21).darker());
    ampReleaseKnob.setDoubleClickReturnValue(true, double(MOrganOscParameters::ampReleaseDefault), ModifierKeys::noModifiers);
    addAndMakeVisible(labeledAmpReleaseKnob);

    processor.parameters.attachControls(
        drawBar1,
        drawBar2,
        drawBar3,
        drawBar4,
        drawBar5,
        drawBar6,
        drawBar7,
        drawBar8,
        drawBar9,
        ampAttackKnob,
        ampDecayKnob,
        ampSustainKnob,
        ampReleaseKnob );

    logoImage = ImageCache::getFromMemory(BinaryData::LogoM_png, BinaryData::LogoM_pngSize);
    float whRatio = float(logoImage.getWidth()) / logoImage.getHeight();
    int logoHeight = 80;
    logoImage = logoImage.rescaled(int(0.5 + logoHeight * whRatio), logoHeight);

    addAndMakeVisible(infoButton);

    setSize(520, 380);
}

MOrganOscEditor::~MOrganOscEditor()
{
    processor.removeChangeListener(this);
    processor.parameters.detachControls();
    setLookAndFeel(nullptr);
}

void MOrganOscEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    area.removeFromTop(40);

    auto row = area.removeFromBottom(80);
    auto col = row.removeFromLeft(110).reduced(0, 4);
    sustainToggle.setBounds(col.removeFromTop(24));
    vibratoToggle.setBounds(col.removeFromTop(24));
    infoButton.setBounds(col.removeFromBottom(24));

    row.removeFromLeft(20);
    row.removeFromRight(logoImage.getWidth() + 20);
    auto knobWidth = (row.getWidth() - 30) / 4;
    labeledAmpAttackKnob.setBounds(row.removeFromLeft(knobWidth));
    row.removeFromLeft(10);
    labeledAmpDecayKnob.setBounds(row.removeFromLeft(knobWidth));
    row.removeFromLeft(10);
    labeledAmpSustainKnob.setBounds(row.removeFromLeft(knobWidth));
    row.removeFromLeft(10);
    labeledAmpReleaseKnob.setBounds(row);

    area.removeFromBottom(20);

    int drawBarWidth = (area.getWidth() - 9 * 4) / 9;
    auto column = area.removeFromLeft(drawBarWidth);
    drawBar1.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar2.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar3.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar4.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar5.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar6.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar7.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar8.setBounds(column);
    area.removeFromLeft(4);
    column = area.removeFromLeft(drawBarWidth);
    drawBar9.setBounds(column);

}

void MOrganOscEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colour(42, 38, 37));

    auto area = getLocalBounds().reduced(20);
    auto box = area.removeFromTop(40);
    g.setColour(Colour(40, 40, 40));
    g.fillRect(box);
    g.setColour(Colours::black);
    g.drawRect(box, 3.0f);

    auto row = area.removeFromBottom(80);
    g.drawImage(logoImage, row.toFloat().removeFromRight(float(logoImage.getWidth())));
}

void MOrganOscEditor::changeListenerCallback(ChangeBroadcaster*)
{
    sustainToggle.setToggleState(processor.enablePedalSustain, dontSendNotification);
    vibratoToggle.setToggleState(processor.enableModwheelVibrato, dontSendNotification);
}
