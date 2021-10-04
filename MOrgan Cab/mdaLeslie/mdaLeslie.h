#pragma once
#include <JuceHeader.h>

/*  Simple rotary-speaker emulation adapted from the open-source mda-vst plug-ins by
    Paul Kellett. (http://mda.smartelectronix.com/)
*/

// SD: I added smoothing of parameters 0, 4, and 5, to avoid clicking artifacts
#define PARAM_SMOOTHING

class mdaLeslie
{
public:
    mdaLeslie();
    ~mdaLeslie();

    void init(double sampleRate);
    void deinit() {};
    void render(int numSamples, const float** inputs, float** outputs);

    void slow();
    void fast();
    void setSpeed(float fraction);

    float param[9]; // externally accessible parameters
    void update();  // call after parameter change

protected:
    float sampleRateHz;

    float filo; //crossover filter coeff
    float fbuf1, fbuf2; //filter buffers
    float twopi; //speed, target, momentum, phase, width, ampmod, freqmod...
    float hspd, hset, hmom, hphi, hwid, hlev, hdep;
    float lspd, lset, lmom, lphi, lwid, llev, gain;
    float* hbuf;  //HF delay buffer
    int size, hpos; //buffer length & pointer
    float chp, dchp, clp, dclp, shp, dshp, slp, dslp;

#ifdef PARAM_SMOOTHING
    using Smf = LinearSmoothedValue<float>;
    Smf p0, p4, p5;
#endif
};
