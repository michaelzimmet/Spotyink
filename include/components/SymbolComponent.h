#pragma once
#include "Component.h"

enum class SymbolType { Play, Pause };

class SymbolComponent : public Component {
    private:
        SymbolType symbol;
        uint16_t color;

        void drawPlay(DisplayType& display, int cx, int cy, int size);
        void drawPause(DisplayType& display, int cx, int cy, int size);

    public:
        SymbolComponent(int x, int y, int width, int height, int margin, uint16_t color = GxEPD_BLACK);
        void draw(DisplayType& display) override;
        void setSymbol(SymbolType s) { symbol = s; }
};