#include "MOrganCabProcessor.h"
#include "MOrganCabEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MOrganCabProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* MOrganCabProcessor::createEditor()
{
    return new MOrganCabEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
MOrganCabProcessor::MOrganCabProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", AudioChannelSet::stereo(), true)
        .withOutput("Output", AudioChannelSet::stereo(), true)
    )
    , valueTreeState(*this, nullptr, Identifier("MOrganCab"), MOrganCabParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , fast(false)
    , midiControlMode(0)
    , directMix(parameters.direct)
    , les1Mix(parameters.leslie1)
    , les2Mix(parameters.leslie2)
{
    leslie1Buffers[0] = leslie1Buffers[1] = nullptr;
    leslie2Buffers[0] = leslie2Buffers[1] = nullptr;
}

bool MOrganCabProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() != 1) return false;
    if (layout.outputBuses.size() != 1) return false;
    if (layout.inputBuses[0] != AudioChannelSet::stereo()) return false;
    if (layout.outputBuses[0] != AudioChannelSet::stereo()) return false;
    return true;
}

// Respond to parameter changes
void MOrganCabProcessor::parameterChanged(const String& whatChanged, float newValue)
{
    DBG(whatChanged + " <== " + String(newValue));
    if (whatChanged == MOrganCabParameters::speedID)
    {
        bool newFast = parameters.speed > 0.5f;
        if (newFast == fast) return;
        fast = newFast;
        if (fast)
        {
            leslie1.setSpeed(8.0f);
            leslie2.fast();
        }
        else
        {
            leslie1.setSpeed(4.0f);
            leslie2.slow();
        }
        sendChangeMessage();
    }
}

// Destructor
MOrganCabProcessor::~MOrganCabProcessor()
{
    leslie1.deinit();
    leslie2.deinit();
    if (leslie1Buffers[0]) delete leslie1Buffers[0];
    if (leslie2Buffers[0]) delete leslie2Buffers[0];
}

// Prepare to process audio (always called at least once before processBlock)
void MOrganCabProcessor::prepareToPlay(double sampleRate, int maxSamplesPerBlock)
{
    releaseResources();

    leslie1.init(sampleRate);
    leslie2.init(sampleRate);

    if (leslie1Buffers[0]) delete leslie1Buffers[0];
    if (leslie2Buffers[0]) delete leslie2Buffers[0];

    bufferSize = 2 * maxSamplesPerBlock;
    leslie1Buffers[0] = new float[bufferSize];
    leslie1Buffers[1] = leslie1Buffers[0] + maxSamplesPerBlock;
    leslie2Buffers[0] = new float[bufferSize];
    leslie2Buffers[1] = leslie2Buffers[0] + maxSamplesPerBlock;
}

// Audio processing finished; release any allocated memory
void MOrganCabProcessor::releaseResources()
{
    leslie1.deinit();
    leslie2.deinit();
}

// Process one buffer ("block") of data
void MOrganCabProcessor::processBlock(AudioBuffer<float>& audioBuffer, MidiBuffer& midiBuffer)
{
    for (const MidiMessageMetadata mmd : midiBuffer)
    {
        auto msg = mmd.getMessage();
        if (msg.isSustainPedalOn())
        {
            if (midiControlMode & 1)
            {
                valueTreeState.getParameter(MOrganCabParameters::speedID)->setValueNotifyingHost(0.8f);
                leslie1.setSpeed(8.0f);
                leslie2.fast();
            }
        }
        else if (msg.isSustainPedalOff())
        {
            if (midiControlMode & 1)
            {
                valueTreeState.getParameter(MOrganCabParameters::speedID)->setValueNotifyingHost(0.4f);
                leslie1.setSpeed(4.0f);
                leslie2.slow();
            }
        }
        else if (msg.isControllerOfType(1))
        {
            float cv = msg.getControllerValue() / 127.0f;
            if (midiControlMode & 2)
            {
                valueTreeState.getParameter(MOrganCabParameters::speedID)->setValueNotifyingHost(cv > 0.5f ? 0.8f : 0.4f);
                leslie1.setSpeed(8.0f * cv);
                leslie2.setSpeed(cv);
            }
        }
    }
    int numSamples = audioBuffer.getNumSamples();
    midiBuffer.clear(0, numSamples);

    // render Leslie effects to their buffers
    const float* inbufs[2] = { audioBuffer.getWritePointer(0), audioBuffer.getWritePointer(1) };
    leslie1.render(numSamples, inbufs, leslie1Buffers);
    leslie2.render(numSamples, inbufs, leslie2Buffers);

    // compute output mix
    float* pOutL = audioBuffer.getWritePointer(0);
    float* pOutR = audioBuffer.getWritePointer(1);
    const float* pSynthL = audioBuffer.getWritePointer(0);
    const float* pSynthR = audioBuffer.getWritePointer(1);
    const float* pLes1L = leslie1Buffers[0];
    const float* pLes1R = leslie1Buffers[1];
    const float* pLes2L = leslie2Buffers[0];
    const float* pLes2R = leslie2Buffers[1];

    float dd = (parameters.direct - directMix) / numSamples;
    float d1 = (parameters.leslie1 - les1Mix) / numSamples;
    float d2 = (parameters.leslie2 - les2Mix) / numSamples;
    for (int i = 0; i < numSamples; i++, pOutL++, pOutR++)
    {
        *pOutL = *pSynthL++ * directMix;
        *pOutR = *pSynthR++ * directMix;
        *pOutL += *pLes1L++ * les1Mix;
        *pOutR += *pLes1R++ * les1Mix;
        *pOutL += *pLes2L++ * les2Mix;
        *pOutR += *pLes2R++ * les2Mix;
        directMix += dd;
        les1Mix += d1;
        les2Mix += d2;
    }
    directMix = parameters.direct;
    les1Mix = parameters.leslie1;
    les2Mix = parameters.leslie2;
}

// Called by the host when it needs to persist the current plugin state
void MOrganCabProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    xml->setAttribute("midiControlMode", midiControlMode);
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void MOrganCabProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        if (xml->hasAttribute("pedalLeslieMode"))
            midiControlMode = xml->getIntAttribute("pedalLeslieMode");
        midiControlMode = xml->getIntAttribute("midiControlMode", false);
        valueTreeState.state = ValueTree::fromXml(*xml);
        sendChangeMessage();
    }
}
