#include "Drawbar.h"

Drawbar::Drawbar(Colour tc, Colour ttc, String tt)
    : thumbColour(tc)
    , thumbTextColour(ttc)
    , thumbText(tt)
{
    numberStrip = ImageCache::getFromMemory(BinaryData::NumberStrip_png, BinaryData::NumberStrip_pngSize);
}

void Drawbar::mouseDown(const MouseEvent&)
{
    dragStartValue = getValue();
}

void Drawbar::mouseDrag(const MouseEvent& mouse)
{
    setValue(dragStartValue + 100.0f * mouse.getDistanceFromDragStartY() / height);
}

void Drawbar::mouseUp(const MouseEvent&)
{
}

void Drawbar::paint (juce::Graphics& g)
{
    float value = 0.01f * float(getValue());
    jassert(isPositiveAndNotGreaterThan(value, 1.0f));

    // compute bar rectangle and height
    auto bar = getLocalBounds().toFloat();
    bar.removeFromBottom(width * 1.25f);
    bar.setHeight(bar.getHeight() * value);
    float barHeight = bar.getHeight();
    bar.reduce(width * 0.1f, 0.0f);

    // draw the bar
    g.setColour(Colours::white);
    g.fillRect(bar);
    g.setColour(Colours::black);
    g.fillRect(bar.withRight(bar.getX() + 3.0f));
    g.fillRect(bar.withLeft(bar.getRight() - 3.0f));
    g.drawImage(numberStrip, bar, RectanglePlacement::yBottom | RectanglePlacement::xMid | RectanglePlacement::doNotResize);

    // draw the thumb and tab
    auto thumb = thumbRect.withY(barHeight);
    auto tab = tabRect.withY(barHeight + width * 0.75f);
    float cornerRadius = width * 0.2f;
    g.setColour(thumbColour);
    g.fillRect(thumb);
    g.setColour(Colours::black);
    g.drawRect(thumb, 3.0f);
    g.setColour(thumbColour);
    g.fillRoundedRectangle(tab, cornerRadius);
    g.setColour(Colours::black);
    g.drawRoundedRectangle(tab.reduced(0.5f, 0.5f), cornerRadius, 3.0f);
    g.setColour(thumbTextColour);
    g.drawText(thumbText, thumb.reduced(0, 8), Justification::centredTop);
}

void Drawbar::resized()
{
    height = getHeight();
    width = getWidth();

    tabRect = Rectangle<float>(width, 0.5f * width);
    thumbRect = Rectangle<float>(width, width);

    numberStrip = ImageCache::getFromMemory(BinaryData::NumberStrip_png, BinaryData::NumberStrip_pngSize);
    float whRatio = float(numberStrip.getWidth()) / numberStrip.getHeight();
    float barHeight = height - width * 1.25f;
    float newWidth = barHeight * whRatio;
    numberStrip = numberStrip.rescaled(int(0.5f + newWidth), int(0.5f + barHeight));
}
