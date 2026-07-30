#include "../include/components/ProgressComponent.h"

ProgressComponent::ProgressComponent(int x, int y, int width, int height, int margin,
    uint16_t color) : Component(x, y, width, height, margin),
    progress(0.0f), color(color) {}

void ProgressComponent::draw(DisplayType& display) {
    std::tuple<int, int> dimensions = getDimension();
    int rawWidth = std::get<0>(dimensions);
    int rawHeight = std::get<1>(dimensions);

    std::tuple<int, int> startPos = getStartPos();
    int startPosX = std::get<0>(startPos);
    int startPosY = std::get<1>(startPos);

    display.drawRect(startPosX, startPosY, rawWidth, rawHeight, color);
    
    int filledWidth = static_cast<int>(rawWidth * progress);
    if (filledWidth > 0) {
        int padding = 2;
        int fillHeight = rawHeight - 2 * padding;
        display.fillRect(startPosX + padding, startPosY + padding,
                          filledWidth - 2 * padding, fillHeight, color);
    }
}