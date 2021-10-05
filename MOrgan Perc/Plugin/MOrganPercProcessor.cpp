#include "MOrganPercProcessor.h"
#include "MOrganPercEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MOrganPercProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* MOrganPercProcessor::createEditor()
{
    return new MOrganPercEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
MOrganPercProcessor::MOrganPercProcessor()
    : AudioProcessor()
    , valueTreeState(*this, nullptr, Identifier("MOrganPerc"), MOrganPercParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , triggered(false)
    , triggerTimeMs(0)
{
    memset(keyIsDown, 0, sizeof(keyIsDown));
    decay.exponentialCurve(0.0f, MOrganPercParameters::decayRateDefault, 1.0f, 0.0f);
}

bool MOrganPercProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() > 0) return false;
    if (layout.outputBuses.size() > 0) return false;
    return true;
}

// Respond to parameter changes
void MOrganPercProcessor::parameterChanged(const String& paramID, float)
{
    if (paramID == MOrganPercParameters::decayRateID)
    {
        const ScopedLock myScopedLock(getCallbackLock());
        decay.exponentialCurve(0.0f, parameters.decayRate, 1.0f, 0.0f);
        sendChangeMessage();
    }
}

bool MOrganPercProcessor::keyDown(int nn, float& velFactor)
{
    keyIsDown[nn] = true;

    if (triggered)
    {
        float millisSinceTrigger = float(Time::currentTimeMillis() - triggerTimeMs);
        bool trigger = millisSinceTrigger < parameters.gateTimeMs;
        if (trigger)
            velFactor = decay.interp_bounded(millisSinceTrigger / parameters.gateTimeMs);
        return trigger;
    }
    else
    {
        triggered = true;
        triggerTimeMs = Time::currentTimeMillis();
        velFactor = 1.0f;
        return true;
    }
}

void MOrganPercProcessor::keyUp(int nn)
{
    keyIsDown[nn] = false;
    bool anyKeyDown = false;
    for (int i = 0; i < 128; i++) anyKeyDown |= keyIsDown[i];
    if (triggered && !anyKeyDown) triggered = false;
}

// Process one buffer ("block") of data
void MOrganPercProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiIn)
{
    // the audio buffer in a midi effect will have zero channels!
    jassert(buffer.getNumChannels() == 0);

    MidiBuffer midiOut;
    bool allowEvent = false;

    for (const MidiMessageMetadata mmd : midiIn)
    {
        auto msg = mmd.getMessage();
        auto samplePos = mmd.samplePosition;
        if (msg.isNoteOn())
        {
            int nn = msg.getNoteNumber();
            float velFactor;
            allowEvent = keyDown(nn, velFactor);
            if (allowEvent)
            {
                noteOnSent[nn] = true;
                msg.multiplyVelocity(velFactor);
            }
        }
        else if (msg.isNoteOff())
        {
            int nn = msg.getNoteNumber();
            keyUp(nn);
            allowEvent = noteOnSent[nn];
            noteOnSent[nn] = false;
        }

        if (allowEvent) midiOut.addEvent(msg, samplePos);
    }

    midiIn.swapWith(midiOut);
}

// Called by the host when it needs to persist the current plugin state
void MOrganPercProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void MOrganPercProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
        sendChangeMessage();
    }
}
