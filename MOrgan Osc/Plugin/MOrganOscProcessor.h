#pragma once
#include <JuceHeader.h>
#include "MOrganOscParameters.h"
#include "Organ.hpp"

class MOrganOscProcessor    : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
                            , public ChangeBroadcaster
{
public:
    MOrganOscProcessor();
    virtual ~MOrganOscProcessor() = default;
protected:
    bool isBusesLayoutSupported(const BusesLayout&) const override;

    // How the plugin describes itself to the host
public:
    const String getName() const override { return "MOrganOsc"; }
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

    // GUI accessible members
    Organ synth;
    bool enablePedalSustain;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    MOrganOscParameters parameters;
    
    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

private:
    std::unique_ptr<XmlElement> getStateXml();
    void setStateXml(XmlElement*);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOrganOscProcessor)
};
