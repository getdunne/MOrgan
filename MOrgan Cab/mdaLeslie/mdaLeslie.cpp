#include "mdaLeslie.h"

mdaLeslie::mdaLeslie()
{
    // initialize params for "Slow" speed
    param[0] = 0.33f;
    param[1] = 0.50f;
    param[2] = 0.48f;
    param[3] = 0.70f;
    param[4] = 0.75f;
    param[5] = 0.57f;
    param[6] = 0.50f;
    param[7] = 0.50f;
    param[8] = 0.60f;

    size = 256; hpos = 0;
    hbuf = new float[size];
    memset(hbuf, 0, size * sizeof(float));
    fbuf1 = fbuf2 = 0.0f;
    twopi = 6.2831853f;

    chp = dchp = clp = dclp = shp = dshp = slp = dslp = 0.0f;

    lspd = 0.0f; hspd = 0.0f;
    lphi = 0.0f; hphi = 1.6f;

    sampleRateHz = 44100.0f;
    update();
}

mdaLeslie::~mdaLeslie()
{
    if (hbuf) delete[] hbuf;
}

void mdaLeslie::init(double sampleRate)
{
    sampleRateHz = float(sampleRate);
#ifdef PARAM_SMOOTHING
    p0.reset(sampleRate, 0.1); p0.setCurrentAndTargetValue(param[0]);
    p4.reset(sampleRate, 0.1); p4.setCurrentAndTargetValue(param[4]);
    p5.reset(sampleRate, 0.1); p5.setCurrentAndTargetValue(param[5]);
#endif
    update();
}

void mdaLeslie::slow()
{
#ifdef PARAM_SMOOTHING
    p0.setTargetValue(0.33f);
    p4.setTargetValue(0.75f);
    p5.setTargetValue(0.57f);
#else
    param[0] = 0.33f;
    param[4] = 0.75f;
    param[5] = 0.57f;
    update();
#endif
}

void mdaLeslie::fast()
{
#ifdef PARAM_SMOOTHING
    p0.setTargetValue(0.66f);
    p4.setTargetValue(0.60f);
    p5.setTargetValue(0.70f);
#else
    param[0] = 0.66f;
    param[4] = 0.60f;
    param[5] = 0.70f;
    update();
#endif
}

void mdaLeslie::setSpeed(float fraction)
{
    if (fraction > 0.5f && param[0] < 0.66f) fast();
    else if (fraction <= 0.5f && param[0] > 0.33f) slow();
}

void mdaLeslie::update()
{
    float ifs = 1.0f / sampleRateHz;
    float spd = twopi * ifs * 2.0f * param[7];

    //calcs here!
    filo = 1.f - (float)pow(10.0f, param[2] * (2.27f - 0.54f * param[2]) - 1.92f);

    if (param[0] < 0.50f)
    {
        if (param[0] < 0.1f) //stop
        {
            lset = 0.00f; hset = 0.00f;
            lmom = 0.12f; hmom = 0.10f;
        }
        else //low speed
        {
            lset = 0.49f; hset = 0.66f;
            lmom = 0.27f; hmom = 0.18f;
        }
    }
    else //high speed
    {
        lset = 5.31f; hset = 6.40f;
        lmom = 0.14f; hmom = 0.09f;
    }
    hmom = (float)pow(10.0f, -ifs / hmom);
    lmom = (float)pow(10.0f, -ifs / lmom);
    hset *= spd;
    lset *= spd;

    gain = 0.4f * (float)pow(10.0f, 2.0f * param[1] - 1.0f);
    lwid = param[6] * param[6];
    llev = gain * 0.9f * param[8] * param[8];
    hwid = param[3] * param[3];
    hdep = param[4] * param[4] * sampleRateHz / 760.0f;
    hlev = gain * 0.9f * param[5] * param[5];
}

void mdaLeslie::render(int numSamples, const float** inputs, float** outputs)
{
    const float* in1 = inputs[0];
    const float* in2 = inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];
    float a, c, d, g = gain, h, l;
    float fo = filo, fb1 = fbuf1, fb2 = fbuf2;
    float hl = hlev, hs = hspd, ht, hm = hmom, hp = hphi, hw = hwid, hd = hdep;
    float ll = llev, ls = lspd, lt, lm = lmom, lp = lphi, lw = lwid;
    float hint, k0 = 0.03125f, k1 = 32.f; //k0 = 1/k1
    int hdd, hdd2, k = 0, hps = hpos;

    ht = hset * (1.f - hm); //target speeds
    lt = lset * (1.f - lm);

    chp = (float)cos(hp); chp *= chp * chp; //set LFO values
    clp = (float)cos(lp);
    shp = (float)sin(hp);
    slp = (float)sin(lp);

    --in1;
    --in2;
    --out1;
    --out2;
    while (--numSamples >= 0)
    {
#ifdef PARAM_SMOOTHING
        bool mustUpdate = p0.isSmoothing() || p4.isSmoothing() || p5.isSmoothing();
        param[0] = p0.getNextValue();
        param[4] = p4.getNextValue();
        param[5] = p5.getNextValue();
        if (mustUpdate) update();
#endif

        a = *++in1 + *++in2; //mono input

        if (k) k--; else //linear piecewise approx to LFO waveforms
        {
            ls = (lm * ls) + lt; //tend to required speed
            hs = (hm * hs) + ht;
            lp += k1 * ls;
            hp += k1 * hs;

            dchp = (float)cos(hp + k1 * hs);
            dchp = k0 * (dchp * dchp * dchp - chp); //sin^3 level mod
            dclp = k0 * ((float)cos(lp + k1 * ls) - clp);
            dshp = k0 * ((float)sin(hp + k1 * hs) - shp);
            dslp = k0 * ((float)sin(lp + k1 * ls) - slp);

            k = (int)k1;
        }

        fb1 = fo * (fb1 - a) + a; //crossover
        fb2 = fo * (fb2 - fb1) + fb1;
        h = (g - hl * chp) * (a - fb2); //volume
        l = (g - ll * clp) * fb2;

        if (hps > 0) hps--; else hps = 200;  //delay input pos
        hint = hps + hd * (1.0f + chp); //delay output pos 
        hdd = (int)hint;
        hint = hint - hdd; //linear intrpolation
        hdd2 = hdd + 1;
        if (hdd > 199) { if (hdd > 200) hdd -= 201; hdd2 -= 201; }

        *(hbuf + hps) = h; //delay input
        a = *(hbuf + hdd);
        h += a + hint * (*(hbuf + hdd2) - a); //delay output

        c = l + h;
        d = l + h;
        h *= hw * shp;
        l *= lw * slp;
        d += l - h;
        c += h - l;

        *++out1 = c; //output
        *++out2 = d;

        chp += dchp;
        clp += dclp;
        shp += dshp;
        slp += dslp;
    }
    lspd = ls;
    hspd = hs;
    hpos = hps;
    lphi = (float)fmod(lp + (k1 - k) * ls, twopi);
    hphi = (float)fmod(hp + (k1 - k) * hs, twopi);
    if (fabs(fb1) > 1.0e-10) fbuf1 = fb1; else fbuf1 = 0.0f; //catch denormals
    if (fabs(fb2) > 1.0e-10) fbuf2 = fb2; else fbuf2 = 0.0f;
}
