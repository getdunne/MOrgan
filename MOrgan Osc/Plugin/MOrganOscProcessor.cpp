#include "MOrganOscProcessor.h"
#include "MOrganOscEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MOrganOscProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* MOrganOscProcessor::createEditor()
{
    return new MOrganOscEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
MOrganOscProcessor::MOrganOscProcessor()
    : AudioProcessor(BusesProperties()
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , enablePedalSustain(false)
    , enableModwheelVibrato(false)
    , valueTreeState(*this, nullptr, Identifier("MOrganOsc"), MOrganOscParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , prevMasterVolFraction(parameters.masterVolFraction)
{
    smoothed[0].setCurrentAndTargetValue(parameters.drawbar1);
    smoothed[1].setCurrentAndTargetValue(parameters.drawbar2);
    smoothed[2].setCurrentAndTargetValue(parameters.drawbar3);
    smoothed[3].setCurrentAndTargetValue(parameters.drawbar4);
    smoothed[4].setCurrentAndTargetValue(parameters.drawbar5);
    smoothed[5].setCurrentAndTargetValue(parameters.drawbar6);
    smoothed[6].setCurrentAndTargetValue(parameters.drawbar7);
    smoothed[7].setCurrentAndTargetValue(parameters.drawbar8);
    smoothed[8].setCurrentAndTargetValue(parameters.drawbar9);
}

bool MOrganOscProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() != 0) return false;
    if (layout.outputBuses.size() != 1) return false;
    if (layout.outputBuses[0] != AudioChannelSet::stereo()) return false;
    return true;
}

// Respond to parameter changes
void MOrganOscProcessor::parameterChanged(const String& paramID, float newValue)
{
    if (paramID == MOrganOscParameters::drawbar1ID)
        smoothed[0].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar2ID)
        smoothed[1].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar3ID)
        smoothed[2].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar4ID)
        smoothed[3].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar5ID)
        smoothed[4].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar6ID)
        smoothed[5].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar7ID)
        smoothed[6].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar8ID)
        smoothed[7].setTargetValue(0.01f * newValue);
    else if (paramID == MOrganOscParameters::drawbar9ID)
        smoothed[8].setTargetValue(0.01f * newValue);

    else if (paramID == MOrganOscParameters::ampAttackID)
        synth.setAmpAttackDurationSeconds(newValue);
    else if (paramID == MOrganOscParameters::ampDecayID)
        synth.setAmpDecayDurationSeconds(newValue);
    else if (paramID == MOrganOscParameters::ampSustainID)
        synth.setAmpSustainFraction(0.01f * newValue);
    else if (paramID == MOrganOscParameters::ampReleaseID)
        synth.setAmpReleaseDurationSeconds(newValue);
}

// Prepare to process audio (always called at least once before processBlock)
void MOrganOscProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    releaseResources();
    synth.init(sampleRate);
    for (int i = 0; i < 9; i++) smoothed[i].reset(sampleRate / AKSYNTH_CHUNKSIZE, 0.1);
}

// Audio processing finished; release any allocated memory
void MOrganOscProcessor::releaseResources()
{
    synth.deinit();
}

// Process one buffer ("block") of data
void MOrganOscProcessor::processBlock (AudioBuffer<float>& audioBuffer, MidiBuffer& midiBuffer)
{
    for (const MidiMessageMetadata mmd : midiBuffer)
    {
        auto msg = mmd.getMessage();
        if (msg.isNoteOn())
        {
            int nn = msg.getNoteNumber();
            int vel = msg.getVelocity();
            float freq = float(msg.getMidiNoteInHertz(nn));
            synth.playNote(nn, vel, freq);
        }
        else if (msg.isNoteOff())
        {
            int nn = msg.getNoteNumber();
            synth.stopNote(nn, false);
        }
        else if (msg.isSustainPedalOn())
        {
            if (enablePedalSustain) synth.sustainPedal(true);
        }
        else if (msg.isSustainPedalOff())
        {
            if (enablePedalSustain) synth.sustainPedal(false);
        }
        else if (msg.isPitchWheel())
        {
            float pw = (msg.getPitchWheelValue() - 8192) / 8192.0f;
            synth.setPitchOffset(2.0f * pw);
        }
        else if (msg.isControllerOfType(1))
        {
            if (enableModwheelVibrato)
            {
                float cv = msg.getControllerValue() / 127.0f;
                synth.setVibratoDepth(0.5f * cv);
            }
        }
    }
    int numSamples = audioBuffer.getNumSamples();
    midiBuffer.clear(0, numSamples);


    // render synth
    int channelCount = audioBuffer.getNumChannels();
    float* bufs[2] = { audioBuffer.getWritePointer(0), audioBuffer.getWritePointer(1) };
    memset(bufs[0], 0, numSamples * sizeof(float));
    memset(bufs[1], 0, numSamples * sizeof(float));

    int chunkSize = AKSYNTH_CHUNKSIZE;
    int samplesRemaining = numSamples;
    while (samplesRemaining)
    {
        if (samplesRemaining < chunkSize) chunkSize = samplesRemaining;
        for (int i = 0; i < 9; i++)
            if (smoothed[i].isSmoothing()) synth.setDrawBar(i, smoothed[i].getNextValue());
        synth.render(channelCount, chunkSize, bufs);
        samplesRemaining -= chunkSize;
        bufs[0] += chunkSize;
        bufs[1] += chunkSize;
    }

    audioBuffer.applyGainRamp(0, numSamples, prevMasterVolFraction, parameters.masterVolFraction);
    prevMasterVolFraction = parameters.masterVolFraction;
}

std::unique_ptr<XmlElement> MOrganOscProcessor::getStateXml()
{
#if 0
    // old code: put everything into XML received from APVTS::createXml()
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    xml->setAttribute("midiSustainEnable", enablePedalSustain);
    return std::move(xml);
#else
    // new code: create an XML tree that has non-APVTS state attributes, and APVTS XML as a child object
    auto xml = new XmlElement("MOrganOsc");             // I could choose any name here
    xml->setAttribute("midiSustainEnable", enablePedalSustain);
    xml->setAttribute("modwheelVibratoEnable", enableModwheelVibrato);
    auto apvtsXml = valueTreeState.state.createXml();   // note this XML also has tag name "MOrganOsc"
    // The FIRST time we do this, we might need to scrub the midiSustainEnable we added to the APVTS XML,
    // Because it was there on the XML we passed to APVTS::fromXml() with the old code, it will still be
    // there on the XML returned by APVTS::createXml(), but that's not what we want anymore.
    // This is how we would remove it.
    // apvtsXml->getChildByName("MOrganOsc")->removeAttribute("midiSustainEnable");
    xml->addChildElement(apvtsXml.release());
    return std::unique_ptr<XmlElement>(xml);
#endif
}

void MOrganOscProcessor::setStateXml(XmlElement* xml)
{
#if 0
    // old code: we added our midiSustainEnable attribute to the APVTS XML
    valueTreeState.state = ValueTree::fromXml(*xml);
    enablePedalSustain = xml->getBoolAttribute("midiSustainEnable");
#else
#endif
    // new code: xml points to a larger XML structure that has our midiSustainAttribute
    // (and any other attributes/children we feel like adding to it)
    enablePedalSustain = xml->getBoolAttribute("midiSustainEnable");
    enableModwheelVibrato = xml->getBoolAttribute("modwheelVibratoEnable");

    // xml also contains the APVTS XML as a child element, which is called "MOrganOsc" because that was the
    // name we gave it when declaring it; see line 22 above.
    auto apvtsXml = xml->getChildByName("MOrganOsc");
    if (apvtsXml)
        valueTreeState.state = ValueTree::fromXml(*apvtsXml);
    else
        valueTreeState.state = ValueTree::fromXml(*xml);

    synth.setDrawBar(0, parameters.drawbar1);
    synth.setDrawBar(1, parameters.drawbar2);
    synth.setDrawBar(2, parameters.drawbar3);
    synth.setDrawBar(3, parameters.drawbar4);
    synth.setDrawBar(4, parameters.drawbar5);
    synth.setDrawBar(5, parameters.drawbar6);
    synth.setDrawBar(6, parameters.drawbar7);
    synth.setDrawBar(7, parameters.drawbar8);
    synth.setDrawBar(8, parameters.drawbar9);

    synth.setAmpAttackDurationSeconds(parameters.ampAttackSec);
    synth.setAmpDecayDurationSeconds(parameters.ampDecaySec);
    synth.setAmpSustainFraction(parameters.ampSustainLevel);
    synth.setAmpReleaseDurationSeconds(parameters.ampReleaseSec);

    sendChangeMessage();
}

// Called by the host when it needs to persist the current plugin state
void MOrganOscProcessor::getStateInformation (MemoryBlock& destData)
{
    auto xml = getStateXml();
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void MOrganOscProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    // While migrating from old code to new code, it's helpful to print the actual XML to debug output.
    DBG(xml->toString());

    if (xml && xml->hasTagName(valueTreeState.state.getType())) setStateXml(xml.get());
}
