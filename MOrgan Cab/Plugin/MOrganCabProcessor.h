#pragma once
#include <JuceHeader.h>
#include "MOrganCabParameters.h"
#include "Leslie.hpp"
#include "mdaLeslie.h"

class MOrganCabProcessor    : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
                            , public ChangeBroadcaster
{
public:
    MOrganCabProcessor();
    ~MOrganCabProcessor();
protected:
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    // How the plugin describes itself to the host
public:
    const String getName() const override { return "MOrganCab"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
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
    void prepareToPlay (double sampleRate, int maxSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    MOrganCabParameters parameters;

    Leslie leslie1;
    mdaLeslie leslie2;
    bool fast;
    int midiControlMode;    // 0 = automation only, 1 = sustain, 2 = modwheel, 3 = both

    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

private:
    int bufferSize;
    float* leslie1Buffers[2];
    float* leslie2Buffers[2];

    float directMix, les1Mix, les2Mix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganCabProcessor)
};
