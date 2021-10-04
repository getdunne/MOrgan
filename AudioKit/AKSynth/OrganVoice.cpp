//
//  OrganVoice.cpp
//  AudioKit Core
//
//  Created by Shane Dunne, revision history on Github.
//  Copyright © 2018 AudioKit. All rights reserved.
//

#include "OrganVoice.hpp"
#include <stdio.h>

namespace AudioKitCore
{
    void OrganVoice::init(double sampleRate,
                            WaveStack *pOscStack,
                            OrganVoiceParameters* pTimbreParameters,
                            OrganModParameters* pModParameters)
    {
        VoiceBase::init(sampleRate, pTimbreParameters, pModParameters);

        drawBarOsc.init(sampleRate, pOscStack);
        drawBarOsc.level = pTimbreParameters->organ.drawbars;

        ampEG.init();
    }

    void OrganVoice::start(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
#ifdef SMOOTH_AMP_ADSR
        tempGain = prevTempGain = 0.0f;
#endif
        drawBarOsc.setFrequency(0.5f * freqHz);
        ampEG.start();
        VoiceBase::start(evt, noteNum, velocity, freqHz, volume);
    }

    void OrganVoice::restart(unsigned evt, unsigned noteNum, unsigned velocity, float freqHz, float volume)
    {
        ampEG.restart();
        VoiceBase::restart(evt, noteNum, velocity, freqHz, volume);
    }

    void OrganVoice::release(unsigned evt)
    {
        ampEG.release();
        VoiceBase::release(evt);
    }

    void OrganVoice::stop(unsigned evt)
    {
        VoiceBase::stop(evt);
        ampEG.reset();
    }

    bool OrganVoice::doModulation(void)
    {
        if (ampEG.isIdle()) return true;
#ifdef SMOOTH_AMP_ADSR
        prevTempGain = tempGain;
#endif

        OrganModParameters* pMod = (OrganModParameters*)pModParams;

        if (ampEG.isPreStarting())
        {
            float ampeg = ampEG.getSample();
            tempGain = pMod->masterVol * noteVol * ampeg;
            if (!ampEG.isPreStarting())
            {
                noteVol = newNoteVol;
                tempGain = pMod->masterVol * noteVol * ampeg;

                if (newNoteNumber >= 0)
                {
                    // restarting a "stolen" voice with a new note number
                    drawBarOsc.setFrequency(0.5f * noteHz);
                    noteNumber = newNoteNumber;
                }
                ampEG.start();
            }
        }
        else
            tempGain = pMod->masterVol * noteVol * ampEG.getSample();

        drawBarOsc.phaseDeltaMultiplier = pMod->phaseDeltaMul;

        return false;
    }

    bool OrganVoice::getSamples(int sampleCount, float *leftOutput, float *rightOutput)
    {
        OrganVoiceParameters* pParams = (OrganVoiceParameters*)pTimbreParams;

#ifdef SMOOTH_AMP_ADSR
        float gain = prevTempGain;
        float deltaGain = (tempGain - prevTempGain) / sampleCount;
#endif
        for (int i=0; i < sampleCount; i++)
        {
            float leftSample = 0.0f;
            float rightSample = 0.0f;
            drawBarOsc.getSamples(&leftSample, &rightSample, pParams->organ.mixLevel);
#ifdef SMOOTH_AMP_ADSR
            gain += deltaGain;
            *leftOutput++ += gain * leftSample;
            *rightOutput++ += gain * rightSample;
#else
            * leftOutput++ += tempGain * leftSample;
            *rightOutput++ += tempGain * rightSample;
#endif
        }
        return false;
    }
}
