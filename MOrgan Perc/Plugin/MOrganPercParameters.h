#pragma once
#include <JuceHeader.h>
#include "ParameterListeners.h"

class MOrganPercParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String gateTimeID, gateTimeName, gateTimeLabel;
    static const float gateTimeMin, gateTimeMax, gateTimeDefault, gateTimeStep;
    static const String decayRateID, decayRateName, decayRateLabel;
    static const float decayRateMin, decayRateMax, decayRateDefault, decayRateStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    MOrganPercParameters(AudioProcessorValueTreeState& vts,
                                AudioProcessorValueTreeState::Listener* processor);
    ~MOrganPercParameters();

    void detachControls();
    void attachControls(
        Slider& gateTimeKnob,
        Slider& decayRateKnob );

    // working parameter values
    float gateTimeMs;
    float decayRate;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gateTimeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayRateAttachment;

    // Listener objects link parameters to working variables
    FloatListener gateTimeListener;
    FloatListener decayRateListener;
};
