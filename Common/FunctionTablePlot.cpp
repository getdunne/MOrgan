#include "FunctionTablePlot.h"

FunctionTablePlot::FunctionTablePlot(FunctionTable* tbl)
    : table(tbl)
    , fillColour(Colours::black)
    , outlineColour(Colours::white)
    , plotColour(Colour(41, 196, 159))   // same as iconColour
    , lineThickness(2.0f)
    , tableOffset(0)
{
}

void FunctionTablePlot::setTable(FunctionTable* tbl)
{
    table = tbl;
    repaint();
}

void FunctionTablePlot::setColours(Colour fill, Colour outline, Colour plot)
{
    fillColour = fill;
    outlineColour = outline;
    plotColour = plot;
}

void FunctionTablePlot::setLineThickness(float pixels)
{
    lineThickness = pixels;
}

void FunctionTablePlot::setPhaseOffset(float phase)
{
    tableOffset = int(0.5f + phase * (table->nTableSize - 1)) % table->nTableSize;
    repaint();
}

void FunctionTablePlot::paint(Graphics& g)
{
    g.fillAll(fillColour);
    auto bounds = getLocalBounds();

    if (table != nullptr && isEnabled())
    {
        g.setColour(plotColour);
        Path p;
        float y = table->pWaveTable[tableOffset];
        p.startNewSubPath(0.0f, y);
        for (int i = 1; i < table->nTableSize; i++)
        {
            y = table->pWaveTable[(tableOffset + i) % table->nTableSize];
            p.lineTo(float(i), y);
        }

        float ww = float(getWidth() - 2);
        float wh = float(getHeight() - 2);
        float sx = ww / float(table->nTableSize - 1);
        float sy = wh;
        AffineTransform at = AffineTransform().scaled(sx, -sy).translated(1.0f, 1.0f + wh);
        g.strokePath(p, PathStrokeType(lineThickness), at);
    }

    g.setColour(outlineColour);
    g.drawRect(bounds);
}
