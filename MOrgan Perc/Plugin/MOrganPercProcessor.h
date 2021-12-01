#pragma once
#include <JuceHeader.h>
#include "MOrganPercParameters.h"
#include "FunctionTable.h"

class MOrganPercProcessor   : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
                            , public ChangeBroadcaster
{
public:
    MOrganPercProcessor();
    virtual ~MOrganPercProcessor() = default;
protected:
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    // How the plugin describes itself to the host
public:
    const String getName() const override { return "MOrganPerc"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return true; }
    double getTailLengthSeconds() const override { return 0.0; }
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    // Multiple simultaneously-loaded presets aka "programs" (not used)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String&) override {}

    // Actual audio processing
    void prepareToPlay(double /*sampleRate*/, int /*maxSamplesPerBlock*/) override;
    void releaseResources() override {}
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    MOrganPercParameters parameters;

    // Non-APVTS parameters
    int triggerMode; // 0 = polyphonic, 1 = simple mono, 2 = Hammond mono
    bool triggerModeIsHammond() { return triggerMode == 2; }

    FunctionTable* getDecayTable() { return &decay; }
    
    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

protected:
    bool keyDown(int, float&);
    void keyUp(int);

private:
    struct ScheduledMidiMessage
    {
        uint64 samplePos;
        uint8 data[4];

        ScheduledMidiMessage(MidiMessage& m, uint64 pos)
        {
            samplePos = pos;
            auto p = m.getRawData();
            memcpy(data, p, 3);
        }

        String getDescription()
        {
            return MidiMessage(data, 3).getDescription() + " time " + String(samplePos);
        }
    };
    std::list<ScheduledMidiMessage> midiList;
    void scheduleMidiMessage(MidiMessage msg, double timeInSamples);

    bool keyIsDown[128];
    bool triggered;
    int64 triggerTimeMs;
    FunctionTable decay;

    uint64 blockStart;
    double samplesPerMSec;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganPercProcessor)
};
