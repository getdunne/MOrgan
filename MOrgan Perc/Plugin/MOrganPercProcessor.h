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
    ~MOrganPercProcessor();

    // How the plugin describes itself to the host
    const String getName() const override { return "MOrganPerc"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
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
    MOrganPercParameters parameters;

    FunctionTable* getDecayTable() { return &decay; }
    
    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

protected:
    bool keyDown(int, float&);
    void keyUp(int);

private:
    bool keyIsDown[128];
    bool noteOnSent[128];
    bool triggered;
    int64 triggerTimeMs;
    FunctionTable decay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganPercProcessor)
};
