#pragma once
#include <JuceHeader.h>
#include "MOrganPercProcessor.h"
#include "LabeledKnob.h"
#include "FunctionTablePlot.h"

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

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    MOrganPercProcessor& processor;

    BasicKnob gateTimeKnob; LabeledKnob labeledGateTimeKnob;
    BasicKnob decayRateKnob; LabeledKnob labeledDecayRateKnob;

    FunctionTablePlot plot;

    Image logoImage;
    HyperlinkButton infoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganPercEditor)
};
