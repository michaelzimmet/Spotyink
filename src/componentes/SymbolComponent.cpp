#include <algorithm>
#include "../include/components/SymbolComponent.h"

SymbolComponent::SymbolComponent(int x, int y, int width, int height, int margin, 
    uint16_t color) : Component(x, y, width, height, margin),
    symbol(SymbolType::Pause), color(color) {}

void SymbolComponent::draw(DisplayType& display) {
    std::tuple<int, int> startPos = getStartPos();
    int startPosX = std::get<0>(startPos);
    int startPosY = std::get<1>(startPos);

    std::tuple<int, int> dimensions = getDimension();
    int rawWidth = std::get<0>(dimensions);
    int rawHeight = std::get<1>(dimensions);

    int centerXPos = startPosX + rawWidth / 2;
    int centerYPos = startPosY + rawHeight / 2;

    int symbolSize = std::min(rawHeight, rawWidth);

    switch (symbol) {
        case SymbolType::Play:
            drawPlay(display, centerXPos, centerYPos, symbolSize);
            break;
        case SymbolType::Pause:
            drawPause(display, centerXPos, centerYPos, symbolSize);
            break;
    }
}

void SymbolComponent::drawPause(DisplayType& display, int cx, int cy, int size) {
    int halfSize = size / 2;

    display.fillTriangle(
        cx - halfSize, cy - halfSize,    // upper left
        cx - halfSize, cy + halfSize,   // lower left
        cx + halfSize, cy,              // Spike right
        color
    );
}

void SymbolComponent::drawPlay(DisplayType& display, int cx, int cy, int size) {
    int halfSize = size / 2;
    int barWidth = size / 4;
    int gap = size / 6;

    // left Bar
    display.fillRect(cx - gap / 2 - barWidth, cy - halfSize, barWidth, size, color);
    // right Bar
    display.fillRect(cx + gap / 2, cy - halfSize, barWidth, size, color);
}