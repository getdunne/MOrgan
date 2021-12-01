#pragma once
#include <JuceHeader.h>
#include "ParameterListeners.h"
#include "Drawbar.h"

class MOrganOscParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String drawbar1ID, drawbar1Name, drawbar1Label;
    static const float drawbar1Min, drawbar1Max, drawbar1Default, drawbar1Step;
    static const String drawbar2ID, drawbar2Name, drawbar2Label;
    static const float drawbar2Min, drawbar2Max, drawbar2Default, drawbar2Step;
    static const String drawbar3ID, drawbar3Name, drawbar3Label;
    static const float drawbar3Min, drawbar3Max, drawbar3Default, drawbar3Step;
    static const String drawbar4ID, drawbar4Name, drawbar4Label;
    static const float drawbar4Min, drawbar4Max, drawbar4Default, drawbar4Step;
    static const String drawbar5ID, drawbar5Name, drawbar5Label;
    static const float drawbar5Min, drawbar5Max, drawbar5Default, drawbar5Step;
    static const String drawbar6ID, drawbar6Name, drawbar6Label;
    static const float drawbar6Min, drawbar6Max, drawbar6Default, drawbar6Step;
    static const String drawbar7ID, drawbar7Name, drawbar7Label;
    static const float drawbar7Min, drawbar7Max, drawbar7Default, drawbar7Step;
    static const String drawbar8ID, drawbar8Name, drawbar8Label;
    static const float drawbar8Min, drawbar8Max, drawbar8Default, drawbar8Step;
    static const String drawbar9ID, drawbar9Name, drawbar9Label;
    static const float drawbar9Min, drawbar9Max, drawbar9Default, drawbar9Step;
    static const String ampAttackID, ampAttackName, ampAttackLabel;
    static const float ampAttackMin, ampAttackMax, ampAttackDefault, ampAttackStep;
    static const String ampDecayID, ampDecayName, ampDecayLabel;
    static const float ampDecayMin, ampDecayMax, ampDecayDefault, ampDecayStep;
    static const String ampSustainID, ampSustainName, ampSustainLabel;
    static const float ampSustainMin, ampSustainMax, ampSustainDefault, ampSustainStep;
    static const String ampReleaseID, ampReleaseName, ampReleaseLabel;
    static const float ampReleaseMin, ampReleaseMax, ampReleaseDefault, ampReleaseStep;
    static const String masterVolumeID, masterVolumeName, masterVolumeLabel;
    static const float masterVolumeMin, masterVolumeMax, masterVolumeDefault, masterVolumeStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    MOrganOscParameters(AudioProcessorValueTreeState& vts,
                                AudioProcessorValueTreeState::Listener* processor);
    ~MOrganOscParameters();

    void detachControls();
    void attachControls(
        Drawbar& drawBar1,
        Drawbar& drawBar2,
        Drawbar& drawBar3,
        Drawbar& drawBar4,
        Drawbar& drawBar5,
        Drawbar& drawBar6,
        Drawbar& drawBar7,
        Drawbar& drawBar8,
        Drawbar& drawBar9,
        Slider& ampAttackKnob,
        Slider& ampDecayKnob,
        Slider& ampSustainKnob,
        Slider& ampReleaseKnob,
        Slider& masterVolumeKnob );

    // working parameter values
    float drawbar1;
    float drawbar2;
    float drawbar3;
    float drawbar4;
    float drawbar5;
    float drawbar6;
    float drawbar7;
    float drawbar8;
    float drawbar9;
    float ampAttackSec;
    float ampDecaySec;
    float ampSustainLevel;
    float ampReleaseSec;
    float masterVolFraction;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar1Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar2Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar3Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar4Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar5Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar6Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar7Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar8Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> drawbar9Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampAttackAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampDecayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampSustainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampReleaseAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    // Listener objects link parameters to working variables
    FloatListener drawbar1Listener;
    FloatListener drawbar2Listener;
    FloatListener drawbar3Listener;
    FloatListener drawbar4Listener;
    FloatListener drawbar5Listener;
    FloatListener drawbar6Listener;
    FloatListener drawbar7Listener;
    FloatListener drawbar8Listener;
    FloatListener drawbar9Listener;
    FloatListener ampAttackListener;
    FloatListener ampDecayListener;
    FloatListener ampSustainListener;
    FloatListener ampReleaseListener;
    FloatDecibelListener masterVolumeListener;
};
