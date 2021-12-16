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

// Constructor: we must give this at least an output bus, or we'll get zero-length buffers in processBlock()
MOrganPercProcessor::MOrganPercProcessor()
    : AudioProcessor(BusesProperties().withOutput("output", AudioChannelSet::stereo()))
    , valueTreeState(*this, nullptr, Identifier("MOrganPerc"), MOrganPercParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , triggerMode(0)
    , triggered(false)
    , triggerTimeMs(0)
    , blockStart(0)
    , samplesPerMSec(44.1)
{
    memset(keyIsDown, 0, sizeof(keyIsDown));
    decay.exponentialCurve(0.0f, MOrganPercParameters::decayRateDefault, 1.0f, 0.0f);
}

bool MOrganPercProcessor::isBusesLayoutSupported(const BusesLayout& layout) const
{
    if (layout.inputBuses.size() > 0) return false;
    //if (layout.outputBuses.size() > 0) return false;
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

    if (triggerMode == 0)
    {
        // Poly triggering: always allow
        return true;
    }
    else if (triggerMode == 1)
    {
        // Simple Mono triggering
        if (triggered) return false;
        else
        {
            triggered = true;
            return true;
        }
    }
    else
    {
        // Hammond Perc triggering: use exponential decay circuit simulation
        if (triggered)
        {
            float millisSinceTrigger = float(Time::currentTimeMillis() - triggerTimeMs);
            bool trigger = millisSinceTrigger < parameters.legatoTimeMs;
            if (trigger)
                velFactor = decay.interp_bounded(millisSinceTrigger / parameters.legatoTimeMs);
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
}

void MOrganPercProcessor::keyUp(int nn)
{
    keyIsDown[nn] = false;
    bool anyKeyDown = false;
    for (int i = 0; i < 128; i++) anyKeyDown |= keyIsDown[i];
    if (triggered && !anyKeyDown) triggered = false;
}

void MOrganPercProcessor::scheduleMidiMessage(MidiMessage msg, double timeInSamples)
{
    ScheduledMidiMessage smsg(msg, uint64(timeInSamples));

    auto it = midiList.begin();
    for (; it != midiList.end(); it++)
    {
        if (it->samplePos > smsg.samplePos)
        {
            midiList.insert(it, smsg);
            return;
        }
    }
    if (it == midiList.end()) midiList.push_back(smsg);
}

void MOrganPercProcessor::prepareToPlay(double samplesPerSecond, int)
{
    samplesPerMSec = 0.001 * samplesPerSecond;
}

// Process one buffer ("block") of data
void MOrganPercProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiIn)
{
    MidiBuffer midiOut;
    bool allowEvent = false;

    for (const MidiMessageMetadata mmd : midiIn)
    {
        auto msg = mmd.getMessage();
        auto samplePos = mmd.samplePosition;

        if (msg.isNoteOn())
        {
            int ch = msg.getChannel();
            int nn = msg.getNoteNumber();
            float velFactor;
            allowEvent = keyDown(nn, velFactor);
            if (allowEvent)
            {
                if (triggerMode == 2) msg.multiplyVelocity(velFactor);
                midiOut.addEvent(msg, samplePos);
                double endSamp = blockStart + samplePos + parameters.gateTimeMs * samplesPerMSec;
                scheduleMidiMessage(MidiMessage::noteOff(ch, nn), endSamp);
            }
        }
        else if (msg.isNoteOff())
        {
            int nn = msg.getNoteNumber();
            keyUp(nn);
        }
    }

    uint64 blockLength = buffer.getNumSamples();
    uint64 blockEnd = blockStart + blockLength;
    while (!midiList.empty())
    {
        auto& smsg = midiList.front();
        if (smsg.samplePos < blockStart)
        {
            DBG("late " + smsg.getDescription());
            midiList.pop_front();
        }
        if (smsg.samplePos >= blockStart && smsg.samplePos < blockEnd)
        {
            MidiMessage outMsg = MidiMessage(smsg.data, 3);
            midiOut.addEvent(outMsg, int(smsg.samplePos - blockStart));
            midiList.pop_front();
        }
        else break;
    }
    blockStart = blockEnd;

    midiIn.swapWith(midiOut);
}

// Called by the host when it needs to persist the current plugin state
void MOrganPercProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    xml->setAttribute("triggerMode", triggerMode);
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void MOrganPercProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        triggerMode = xml->getIntAttribute("triggerMode");
        xml->removeAttribute("triggerMode");
        valueTreeState.state = ValueTree::fromXml(*xml);
        sendChangeMessage();
    }
}
