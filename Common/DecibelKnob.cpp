#include "DecibelKnob.h"

DecibelKnob::DecibelKnob(float minDB, float maxDB, float minusInfDB, String minusInfText)
    : BasicKnob(minDB, maxDB)
    , minusInfinitydB(minusInfDB)
    , minusInfinityText(minusInfText)
{
}

double DecibelKnob::getValueFromText(const String& text)
{
    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
    return decibelText.equalsIgnoreCase(minusInfinityText) ? minusInfinitydB
        : decibelText.getDoubleValue();
}

String DecibelKnob::getTextFromValue(double value)
{
    if (value == 0.0) return "0.0 dB";
    else if (value <= minusInfinitydB) return minusInfinityText;
    return Decibels::toString(value);
}
