#pragma once
#include <JuceHeader.h>
#include "FunctionTable.h"

class FunctionTablePlot : public Component
{
public:
    FunctionTablePlot(FunctionTable* tbl = nullptr);
    virtual ~FunctionTablePlot() = default;

    // Component
    void paint(Graphics&) override;
    void setEnabled(bool en)
    {
        Component::setEnabled(en);
        repaint();  // ensure graph appears/disappears
    }

    // FunctionTablePlot
    void setTable(FunctionTable*);
    void setColours(Colour fill, Colour outline, Colour plot);
    void setPlotColour(Colour pc) { plotColour = pc; }
    void setLineThickness(float pixels);
    void setPhaseOffset(float phase);

protected:
    FunctionTable* table;
    Colour fillColour, outlineColour, plotColour;
    float lineThickness;
    int tableOffset;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunctionTablePlot)
};
