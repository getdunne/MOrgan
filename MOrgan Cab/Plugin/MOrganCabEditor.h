#pragma once
#include <JuceHeader.h>
#include "MOrganCabProcessor.h"
#include "LabeledKnob.h"
#include "DecibelKnob.h"

class MOrganCabEditor   : public AudioProcessorEditor
                        , public ChangeListener
{
public:
    MOrganCabEditor (MOrganCabProcessor&);
    virtual ~MOrganCabEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    MOrganCabProcessor& processor;

    DecibelKnob directKnob; LabeledKnob labeledDirectKnob;
    DecibelKnob leslie1Knob; LabeledKnob labeledLeslie1Knob;
    DecibelKnob leslie2Knob; LabeledKnob labeledLeslie2Knob;

    ComboBox leslieModeCombo;
    Label leslieModeLabel;

    Image logoImage;
    HyperlinkButton infoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganCabEditor)
};
