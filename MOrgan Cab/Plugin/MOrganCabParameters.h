#pragma once
#include <JuceHeader.h>
#include "ParameterListeners.h"

class MOrganCabParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String speedID, speedName, speedLabel;
    static const float speedMin, speedMax, speedDefault, speedStep;
    static const String directID, directName, directLabel;
    static const float directMin, directMax, directDefault, directStep;
    static const String leslie1ID, leslie1Name, leslie1Label;
    static const float leslie1Min, leslie1Max, leslie1Default, leslie1Step;
    static const String leslie2ID, leslie2Name, leslie2Label;
    static const float leslie2Min, leslie2Max, leslie2Default, leslie2Step;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    MOrganCabParameters(AudioProcessorValueTreeState& vts,
                        AudioProcessorValueTreeState::Listener* processor);
    ~MOrganCabParameters();

    void detachControls();
    void attachControls(
        Button& speedButton,
        Slider& directKnob,
        Slider& leslie1Knob,
        Slider& leslie2Knob );

    // working parameter values
    float speed;    // fraction 0.0 to 1.0
    float direct;
    float leslie1;
    float leslie2;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> speedAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> directAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> leslie1Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> leslie2Attachment;

    // Listener objects link parameters to working variables
    FloatListener speedListener;
    FloatDecibelListener directListener;
    FloatDecibelListener leslie1Listener;
    FloatDecibelListener leslie2Listener;
};
