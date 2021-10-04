#pragma once
#include <JuceHeader.h>
#include "MOrganOscProcessor.h"
#include "Drawbar.h"
#include "LabeledKnob.h"

class MOrganOscEditor   : public AudioProcessorEditor
                        , public ChangeListener
{
public:
    MOrganOscEditor (MOrganOscProcessor&);
    virtual ~MOrganOscEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    MOrganOscProcessor& processor;

    Drawbar drawBar1, drawBar2, drawBar3, drawBar4, drawBar5, drawBar6, drawBar7, drawBar8, drawBar9;
    ToggleButton sustainToggle;
    BasicKnob ampAttackKnob; LabeledKnob labeledAmpAttackKnob;
    BasicKnob ampDecayKnob; LabeledKnob labeledAmpDecayKnob;
    BasicKnob ampSustainKnob; LabeledKnob labeledAmpSustainKnob;
    BasicKnob ampReleaseKnob; LabeledKnob labeledAmpReleaseKnob;

    Image logoImage;
    HyperlinkButton infoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganOscEditor)
};
