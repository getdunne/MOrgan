#include "FunctionTable.h"
#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES
#endif
#include <math.h>

void FunctionTable::init(int tableLength)
{
    if (nTableSize == tableLength) return;
    nTableSize = tableLength;
    if (pWaveTable) delete[] pWaveTable;
    pWaveTable = new float[tableLength];
}

void FunctionTable::deinit()
{
    if (pWaveTable) delete[] pWaveTable;
    nTableSize = 0;
    pWaveTable = 0;
}

void FunctionTable::updateMinMax()
{
    minValue = pWaveTable[0];
    maxValue = pWaveTable[0];

    for (int i = 0; i < nTableSize; i++)
    {
        float tv = pWaveTable[i];
        if (tv < minValue) minValue = tv;
        if (tv > maxValue) maxValue = tv;
    }
}

void FunctionTable::linearCurve(float gain)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    for (int i = 0; i < nTableSize; i++)
        pWaveTable[i] = gain * i / float(nTableSize - 1);
    updateMinMax();
}

void FunctionTable::linearCurve(float start, float end)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    if (start != end)
    {
        float delta = (end - start) / float(nTableSize - 1);
        float v = start;
        for (int i = 0; i < nTableSize; i++)
        {
            pWaveTable[i] = v;
            v += delta;
        }
        updateMinMax();
    }
}

void FunctionTable::exponentialCurve(float left, float right, float ymin, float ymax)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    float bottom = -expf(-left);
    float top = -expf(-right);
    float vscale = 1.0f / (top - bottom);
    float yscale = ymax - ymin;

    float x = left;
    float dx = (right - left) / (nTableSize - 1);
    for (int i = 0; i < nTableSize; i++, x += dx)
        pWaveTable[i] = ymin + yscale * vscale * (-expf(-x) - bottom);
    
    updateMinMax();
}

void FunctionTable::sinusoid(float amplitude, float offset)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    for (int i = 0; i < nTableSize; i++)
        pWaveTable[i] = offset + (float)(amplitude * sin(double(i) / nTableSize * 2.0 * M_PI));
    updateMinMax();
}

void FunctionTable::triangle(float amplitude, float offset)
{
    // in case user forgot, init table to size 2
    if (pWaveTable == 0) init(2);

    if (nTableSize == 2)   // default 2 elements suffice for a triangle wave
    {
        pWaveTable[0] = -amplitude;
        pWaveTable[1] = amplitude;
    }
    else    // you would normally only do this if you plan to low-pass filter the result
    {
        for (int i = 0; i < nTableSize; i++)
            pWaveTable[i] = offset + 2.0f * amplitude * (0.25f - fabsf((float(i) / nTableSize) - 0.5f));
    }
    updateMinMax();
}

void FunctionTable::sawtooth(float amplitude, float offset)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    for (int i = 0; i < nTableSize; i++)
        pWaveTable[i] = offset + (float)(2.0 * amplitude * double(i) / nTableSize - amplitude);
}

void FunctionTable::square(float amplitude, float dutyCycle, float offset)
{
    // in case user forgot, init table to default size
    if (pWaveTable == 0) init();

    float dcOffset = (offset != 0.0f) ? offset : -amplitude * (2.0f * dutyCycle - 1.0f);
    for (int i = 0; i < nTableSize; i++)
    {
        float phi = (float)i / nTableSize;
        pWaveTable[i] = (phi < dutyCycle ? amplitude : -amplitude) + dcOffset;
    }
    updateMinMax();
}
