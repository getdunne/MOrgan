#include "MOrganPercParameters.h"

// Gate Time
const String MOrganPercParameters::gateTimeID = "gateTime";
const String MOrganPercParameters::gateTimeName = TRANS("Note Length");
const String MOrganPercParameters::gateTimeLabel = "msec";
const float MOrganPercParameters::gateTimeMin = 10.0f;
const float MOrganPercParameters::gateTimeMax = 250.0f;
const float MOrganPercParameters::gateTimeDefault = 50.0f;
const float MOrganPercParameters::gateTimeStep = 1.0f;
// Legato Time
const String MOrganPercParameters::legatoTimeID = "legatoTime";
const String MOrganPercParameters::legatoTimeName = TRANS("Window");
const String MOrganPercParameters::legatoTimeLabel = "msec";
const float MOrganPercParameters::legatoTimeMin = 50.0f;
const float MOrganPercParameters::legatoTimeMax = 250.0f;
const float MOrganPercParameters::legatoTimeDefault = 150.0f;
const float MOrganPercParameters::legatoTimeStep = 1.0f;
// Decay Rate
const String MOrganPercParameters::decayRateID = "decayRate";
const String MOrganPercParameters::decayRateName = TRANS("Decay");
const String MOrganPercParameters::decayRateLabel = "";
const float MOrganPercParameters::decayRateMin = 0.05f;
const float MOrganPercParameters::decayRateMax = 10.0f;
const float MOrganPercParameters::decayRateDefault = 2.5f;
const float MOrganPercParameters::decayRateStep = 0.0f;

AudioProcessorValueTreeState::ParameterLayout MOrganPercParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        gateTimeID, gateTimeName,
        NormalisableRange<float>(gateTimeMin, gateTimeMax, gateTimeStep), gateTimeDefault,
        gateTimeLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        legatoTimeID, legatoTimeName,
        NormalisableRange<float>(legatoTimeMin, legatoTimeMax, legatoTimeStep), legatoTimeDefault,
        legatoTimeLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        decayRateID, decayRateName,
        NormalisableRange<float>(decayRateMin, decayRateMax, decayRateStep), decayRateDefault,
        decayRateLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

MOrganPercParameters::MOrganPercParameters(AudioProcessorValueTreeState& vts,
                                           AudioProcessorValueTreeState::Listener* processor)
    : gateTimeMs(gateTimeDefault)
    , legatoTimeMs(legatoTimeDefault)
    , decayRate(decayRateDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , gateTimeListener(gateTimeMs)
    , legatoTimeListener(legatoTimeMs)
    , decayRateListener(decayRate)
{
    valueTreeState.addParameterListener(gateTimeID, processorAsListener);
    valueTreeState.addParameterListener(legatoTimeID, processorAsListener);
    valueTreeState.addParameterListener(decayRateID, processorAsListener);

    valueTreeState.addParameterListener(gateTimeID, &gateTimeListener);
    valueTreeState.addParameterListener(legatoTimeID, &legatoTimeListener);
    valueTreeState.addParameterListener(decayRateID, &decayRateListener);
}

MOrganPercParameters::~MOrganPercParameters()
{
    detachControls();
    
    valueTreeState.removeParameterListener(gateTimeID, processorAsListener);
    valueTreeState.removeParameterListener(legatoTimeID, processorAsListener);
    valueTreeState.removeParameterListener(decayRateID, processorAsListener);

    valueTreeState.removeParameterListener(gateTimeID, &gateTimeListener);
    valueTreeState.removeParameterListener(legatoTimeID, &legatoTimeListener);
    valueTreeState.removeParameterListener(decayRateID, &decayRateListener);
}

void MOrganPercParameters::detachControls()
{
    gateTimeAttachment.reset(nullptr);
    legatoTimeAttachment.reset(nullptr);
    decayRateAttachment.reset(nullptr);
}

void MOrganPercParameters::attachControls(Slider& gateTimeKnob, Slider& legatoTimeKnob, Slider& decayRateKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    gateTimeAttachment.reset(new SlAt(valueTreeState, gateTimeID, gateTimeKnob));
    legatoTimeAttachment.reset(new SlAt(valueTreeState, legatoTimeID, legatoTimeKnob));
    decayRateAttachment.reset(new SlAt(valueTreeState, decayRateID, decayRateKnob));
}
