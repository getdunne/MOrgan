#pragma once

/// FunctionTable represents a simple one-dimensional table of float values,
/// addressable by a normalized fractional index, [0.0, 1.0), with or without wraparound.
/// Linear interpolation is used to interpolate values between available samples.
///
/// Cyclic (wraparound) addressing is useful for creating simple oscillators. In such
/// cases, the table typically contains one or a few cycles of a periodic function.
/// See class FunctionTableOscillator.
///
/// Bounded addressing is useful for wave-shaping and fast function-approximation using
/// tabulated functions. In such applications, the table contains function values over
/// some bounded domain. See class AKWaveShaper.
struct FunctionTable
{
    float *pWaveTable;
    int nTableSize;

    static const int defaultSize = 256;
    
    FunctionTable() : pWaveTable(0), nTableSize(0), minValue(0.0f), maxValue(1.0f) {}
    ~FunctionTable() { deinit(); }

    // table value limits, and a function to compute them
    float minValue, maxValue;
    void updateMinMax();

    void init(int tableLength= defaultSize);
    void deinit();

    // Initialize the table to some basic shapes
    void linearCurve(float gain = 1.0f);
    void linearCurve(float start, float end);
    void exponentialCurve(float left, float right, float ymin, float ymax);
    void sinusoid(float amplitude = 1.0f, float offset = 0.0f);
    void triangle(float amplitude = 1.0f, float offset = 0.0f);
    void sawtooth(float amplitude = 1.0f, float offset = 0.0f);
    void square(float amplitude = 1.0f, float dutyCycle = 0.5f, float offset = 0.0f);
    
    inline float interp_cyclic(float phase)
    {
        while (phase < 0) phase += 1.0;
        while (phase >= 1.0) phase -= 1.0f;
        
        float readIndex = phase * nTableSize;
        int ri = int(readIndex);
        float f = readIndex - ri;
        int rj = ri + 1; if (rj >= nTableSize) rj -= nTableSize;
        
        float si = pWaveTable[ri];
        float sj = pWaveTable[rj];
        return (float)((1.0f - f) * si + f * sj);
    }
    
    inline float interp_bounded(float phase)
    {
        if (phase < 0.0f) return pWaveTable[0];
        if (phase >= 1.0) return pWaveTable[nTableSize-1];
        
        float readIndex = phase * nTableSize;
        int ri = int(readIndex);
        float f = readIndex - ri;
        int rj = ri + 1; if (rj >= nTableSize) rj = nTableSize - 1;
        
        float si = pWaveTable[ri];
        float sj = pWaveTable[rj];
        return (float)((1.0f - f) * si + f * sj);
    }
};
