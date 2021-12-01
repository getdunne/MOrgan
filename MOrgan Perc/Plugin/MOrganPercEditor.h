#pragma once
#include <JuceHeader.h>
#include "MOrganPercProcessor.h"
#include "LabeledKnob.h"
#include "FunctionTablePlot.h"

class BackgroundBox : public Component
{
public:
    BackgroundBox(Colour bgc, Colour olc) : bgColour(bgc), outlineColour(olc) {}

    // Component
    void paint(Graphics& g) override
    {
        g.fillAll(bgColour);
        g.setColour(outlineColour);
        g.drawRect(getLocalBounds());
    }

protected:
    Colour bgColour, outlineColour;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BackgroundBox)
};


class MOrganPercEditor  : public AudioProcessorEditor
                        , public ChangeListener
{
public:
    MOrganPercEditor (MOrganPercProcessor&);
    virtual ~MOrganPercEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

protected:
    void enableOrDisableHammondControls();

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    MOrganPercProcessor& processor;

    BasicKnob gateTimeKnob; LabeledKnob labeledGateTimeKnob;

    BackgroundBox bbBox;
    ComboBox triggerModeCombo;
    BasicKnob legatoTimeKnob; LabeledKnob labeledLegatoTimeKnob;
    BasicKnob decayRateKnob; LabeledKnob labeledDecayRateKnob;

    FunctionTablePlot plot;

    Image logoImage;
    HyperlinkButton infoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganPercEditor)
};
