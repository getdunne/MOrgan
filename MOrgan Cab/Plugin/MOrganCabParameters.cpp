#include "MOrganCabParameters.h"

// Direct Mix
const String MOrganCabParameters::speedID = "speed";
const String MOrganCabParameters::speedName = TRANS("Leslie Speed");
const String MOrganCabParameters::speedLabel = "";
const float MOrganCabParameters::speedMin = 0.0f;
const float MOrganCabParameters::speedMax = 1.0f;
const float MOrganCabParameters::speedDefault = 0.0f;
const float MOrganCabParameters::speedStep = 0.1f;
// Direct Mix
const String MOrganCabParameters::directID = "direct";
const String MOrganCabParameters::directName = TRANS("Direct Mix");
const String MOrganCabParameters::directLabel = "dB";
const float MOrganCabParameters::directMin = -48.0f;
const float MOrganCabParameters::directMax = 12.0f;
const float MOrganCabParameters::directDefault = Decibels::gainToDecibels(0.33f, directMin);
const float MOrganCabParameters::directStep = 0.25f;
// Leslie1 Mix
const String MOrganCabParameters::leslie1ID = "leslie1";
const String MOrganCabParameters::leslie1Name = TRANS("Leslie1 Mix");
const String MOrganCabParameters::leslie1Label = "dB";
const float MOrganCabParameters::leslie1Min = -48.0f;
const float MOrganCabParameters::leslie1Max = 12.0f;
const float MOrganCabParameters::leslie1Default = Decibels::gainToDecibels(0.33f, leslie1Min);
const float MOrganCabParameters::leslie1Step = 0.25f;
// Leslie2 Mix
const String MOrganCabParameters::leslie2ID = "leslie2";
const String MOrganCabParameters::leslie2Name = TRANS("Leslie2 Mix");
const String MOrganCabParameters::leslie2Label = "dB";
const float MOrganCabParameters::leslie2Min = -48.0f;
const float MOrganCabParameters::leslie2Max = 12.0f;
const float MOrganCabParameters::leslie2Default = Decibels::gainToDecibels(0.33f, leslie1Min);
const float MOrganCabParameters::leslie2Step = 0.25f;

AudioProcessorValueTreeState::ParameterLayout MOrganCabParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        speedID, speedName,
        NormalisableRange<float>(speedMin, speedMax, speedStep), speedDefault,
        speedLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value > 0.5f ? "fast" : "slow").substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        directID, directName,
        NormalisableRange<float>(directMin, directMax, directStep), directDefault,
        directLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        leslie1ID, leslie1Name,
        NormalisableRange<float>(leslie1Min, leslie1Max, leslie1Step), leslie1Default,
        leslie1Label,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        leslie2ID, leslie2Name,
        NormalisableRange<float>(leslie2Min, leslie2Max, leslie2Step), leslie2Default,
        leslie2Label,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

MOrganCabParameters::MOrganCabParameters(AudioProcessorValueTreeState& vts, AudioProcessorValueTreeState::Listener* processor)
    : speed(speedDefault)
    , direct(Decibels::decibelsToGain(directDefault, directMin))
    , leslie1(Decibels::decibelsToGain(leslie1Default, leslie1Min))
    , leslie2(Decibels::decibelsToGain(leslie2Default, leslie2Min))
    , valueTreeState(vts)
    , processorAsListener(processor)
    , speedListener(speed)
    , directListener(direct, directMin)
    , leslie1Listener(leslie1, leslie1Min)
    , leslie2Listener(leslie2, leslie2Min)
{
    valueTreeState.addParameterListener(speedID, processorAsListener);
    valueTreeState.addParameterListener(directID, processorAsListener);
    valueTreeState.addParameterListener(leslie1ID, processorAsListener);
    valueTreeState.addParameterListener(leslie2ID, processorAsListener);

    valueTreeState.addParameterListener(speedID, &speedListener);
    valueTreeState.addParameterListener(directID, &directListener);
    valueTreeState.addParameterListener(leslie1ID, &leslie1Listener);
    valueTreeState.addParameterListener(leslie2ID, &leslie2Listener);
}

MOrganCabParameters::~MOrganCabParameters()
{
    detachControls();
    
    valueTreeState.removeParameterListener(speedID, processorAsListener);
    valueTreeState.removeParameterListener(directID, processorAsListener);
    valueTreeState.removeParameterListener(leslie1ID, processorAsListener);
    valueTreeState.removeParameterListener(leslie2ID, processorAsListener);

    valueTreeState.removeParameterListener(speedID, &speedListener);
    valueTreeState.removeParameterListener(directID, &directListener);
    valueTreeState.removeParameterListener(leslie1ID, &leslie1Listener);
    valueTreeState.removeParameterListener(leslie2ID, &leslie2Listener);
}

void MOrganCabParameters::detachControls()
{
    speedAttachment.reset(nullptr);
    directAttachment.reset(nullptr);
    leslie1Attachment.reset(nullptr);
    leslie2Attachment.reset(nullptr);
}

void MOrganCabParameters::attachControls(
    Button& speedButton,
    Slider& directKnob,
    Slider& leslie1Knob,
    Slider& leslie2Knob )
{
    using BnAt = AudioProcessorValueTreeState::ButtonAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    speedAttachment.reset(new BnAt(valueTreeState, speedID, speedButton));
    directAttachment.reset(new SlAt(valueTreeState, directID, directKnob));
    leslie1Attachment.reset(new SlAt(valueTreeState, leslie1ID, leslie1Knob));
    leslie2Attachment.reset(new SlAt(valueTreeState, leslie2ID, leslie2Knob));
}
