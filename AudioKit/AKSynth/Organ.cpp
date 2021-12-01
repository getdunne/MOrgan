//
//  Organ.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "Organ.hpp"
#include "FunctionTable.hpp"
#include "OrganVoice.hpp"
#include "WaveStack.hpp"
#include "SustainPedalLogic.hpp"
#include "VoiceManager.hpp"

#include <math.h>
#include <list>

#define MAX_VOICE_COUNT 32      // number of voices
#define MIDI_NOTENUMBERS 128    // MIDI offers 128 distinct note numbers

struct Organ::InternalData
{
    /// array of voice resources, and a voice manager
    AudioKitCore::OrganVoice voice[MAX_VOICE_COUNT];
    AudioKitCore::VoiceManager voiceManager;
    
    AudioKitCore::WaveStack waveform;       // WaveStacks are shared by all voice oscillators
    FunctionTableOscillator vibratoLFO;     // one vibrato LFO shared by all voices
    
    // simple parameters
    AudioKitCore::OrganVoiceParameters voiceParameters;
    AudioKitCore::ADSREnvelopeParameters ampEGParameters;
    AudioKitCore::OrganModParameters modParameters;

    // nominal drawbar values and a nonlinear gain table
    float nominalDrawbarValues[9];
    FunctionTable drawbarGainTable;
};

Organ::Organ()
: data(new InternalData)
, pitchOffset(0.0f)
, vibratoDepth(0.0f)
{
    for (int i=0; i < MAX_VOICE_COUNT; i++)
    {
        data->voice[i].ampEG.pParameters = &data->ampEGParameters;
    }

    memset(data->voiceParameters.organ.drawbars, 0, 16 * sizeof(float));
    memset(data->nominalDrawbarValues, 0, 9 * sizeof(float));
    data->nominalDrawbarValues[2] = 1.0f;
    data->voiceParameters.organ.drawbars[1] = 1.0f; // only the 8' drawbar is out
    data->voiceParameters.organ.mixLevel = 0.25f;   // reduce mix level so adding more drawbars won't overdrive
    data->modParameters.masterVol = 0.4f;           // same thing here
    data->modParameters.phaseDeltaMul = 1.0f;       // standard init value

    data->drawbarGainTable.init(9);
    data->drawbarGainTable.pWaveTable[8] = 1.0f;
    data->drawbarGainTable.pWaveTable[7] = 0.708f;
    data->drawbarGainTable.pWaveTable[6] = 0.5f;
    data->drawbarGainTable.pWaveTable[5] = 0.355f;
    data->drawbarGainTable.pWaveTable[4] = 0.25f;
    data->drawbarGainTable.pWaveTable[3] = 0.178f;
    data->drawbarGainTable.pWaveTable[2] = 0.125f;
    data->drawbarGainTable.pWaveTable[1] = 0.0891f;
    data->drawbarGainTable.pWaveTable[0] = 0.0f;

    data->ampEGParameters.setAttackDurationSeconds(0.01f);
    data->ampEGParameters.setReleaseDurationSeconds(0.05f);

    setVelocitySensitivity(0.1f);    // organ has no velocity sensitivity; let's have just a bit
    setTuningRatio(0.5f);            // tune down 1 octave, making 16' drawbar a sub-octave
}

Organ::~Organ()
{
}

int Organ::init(double sampleRate)
{
    FunctionTable waveform;
    int length = 1 << AudioKitCore::WaveStack::maxBits;
    waveform.init(length);
    waveform.hammond(0.177f);
    data->waveform.initStack(waveform.pWaveTable);
    
    data->ampEGParameters.updateSampleRate((float)(sampleRate/AKSYNTH_CHUNKSIZE));
    
    data->vibratoLFO.waveTable.sinusoid();
    data->vibratoLFO.init(sampleRate/AKSYNTH_CHUNKSIZE, 5.0f);
    
    AudioKitCore::VoicePointerArray vpa;
    for (int i=0; i < MAX_VOICE_COUNT; i++)
    {
        data->voice[i].init(sampleRate, &data->waveform, &data->voiceParameters, &data->modParameters);
        vpa.push_back(&(data->voice[i]));
    }
    data->voiceManager.init(vpa, MAX_VOICE_COUNT, &renderPrepCallback, this);
    
    return 0;   // no error
}

void Organ::deinit()
{
}

void Organ::playNote(unsigned noteNumber, unsigned velocity, float noteFrequency)
{
    data->voiceManager.playNote(noteNumber, velocity, noteFrequency);
}

void Organ::stopNote(unsigned noteNumber, bool immediate)
{
    data->voiceManager.stopNote(noteNumber, immediate);
}

void Organ::sustainPedal(bool down)
{
    data->voiceManager.sustainPedal(down);
}

void Organ::renderPrepCallback(void* thisPtr)
{
    Organ& self = *((Organ*)thisPtr);
    float pitchDev = self.pitchOffset + self.vibratoDepth * self.data->vibratoLFO.getSample();
    self.data->modParameters.phaseDeltaMul = powf(2.0f, pitchDev / 12.0f);
}

void Organ::render(unsigned /*channelCount*/, unsigned sampleCount, float *outBuffers[])
{
    data->voiceManager.render(sampleCount, outBuffers);
}

void Organ::setMasterVolume(float value)
{
    data->modParameters.masterVol = value;
}

float Organ::getMasterVolume()
{
    return data->modParameters.masterVol;
}

void Organ::setAmpAttackDurationSeconds(float value)
{
    data->ampEGParameters.setAttackDurationSeconds(value);
    for (int i = 0; i < MAX_VOICE_COUNT; i++) data->voice[i].updateAmpAdsrParameters();
}
float Organ::getAmpAttackDurationSeconds(void)
{
    return data->ampEGParameters.getAttackDurationSeconds();
}
void  Organ::setAmpDecayDurationSeconds(float value)
{
    data->ampEGParameters.setDecayDurationSeconds(value);
    for (int i = 0; i < MAX_VOICE_COUNT; i++) data->voice[i].updateAmpAdsrParameters();
}
float Organ::getAmpDecayDurationSeconds(void)
{
    return data->ampEGParameters.getDecayDurationSeconds();
}
void  Organ::setAmpSustainFraction(float value)
{
    data->ampEGParameters.sustainFraction = value;
    for (int i = 0; i < MAX_VOICE_COUNT; i++) data->voice[i].updateAmpAdsrParameters();
}
float Organ::getAmpSustainFraction(void)
{
    return data->ampEGParameters.sustainFraction;
}
void  Organ::setAmpReleaseDurationSeconds(float value)
{
    data->ampEGParameters.setReleaseDurationSeconds(value);
    for (int i = 0; i < MAX_VOICE_COUNT; i++) data->voice[i].updateAmpAdsrParameters();
}

float Organ::getAmpReleaseDurationSeconds(void)
{
    return data->ampEGParameters.getReleaseDurationSeconds();
}

void Organ::setDrawBar(int index, float value)
{
    if (index < 0) index = 0;
    if (index > 8) index = 8;
    data->nominalDrawbarValues[index] = value;
    index = AudioKitCore::DrawbarsOscillator::drawBarMap[index];
    value = data->drawbarGainTable.interp_bounded(value);
    data->voiceParameters.organ.drawbars[index] = value;
}

float Organ::getDrawBar(int index)
{
    if (index < 0 || index > 8) return 0.0f;
    return data->nominalDrawbarValues[index];
}

void Organ::setHarmonicLevel(int index, float value)
{
    if (index >= 0 && index < 16)
        data->voiceParameters.organ.drawbars[index] = value;
}

float Organ::getHarmonicLevel(int index)
{
    if (index < 0 || index > 15) return 0.0f;
    return data->voiceParameters.organ.drawbars[index];
}

void Organ::setVelocitySensitivity(float value)
{
    data->voiceManager.setVelocitySensitivity(value);
}

float Organ::getVelocitySensitivity()
{
    return data->voiceManager.getVelocitySensitivity();
}

void Organ::setTuningRatio(float value)
{
    data->voiceManager.setTuningRatio(value);
}

float Organ::getTuningRatio()
{
    return data->voiceManager.getTuningRatio();
}
