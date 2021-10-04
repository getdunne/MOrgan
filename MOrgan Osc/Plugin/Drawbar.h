#pragma once
#include <JuceHeader.h>

class Drawbar  : public Slider
{
public:
    Drawbar(Colour, Colour, String);
    virtual ~Drawbar() = default;

    // Component
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;

protected:
    Colour thumbColour, thumbTextColour;
    String thumbText;

    float height, width;
    Rectangle<float> thumbRect, tabRect;

    Image numberStrip;
    float dragStartValue;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Drawbar)
};
