
#include "../include/components/ImageComponent.h"

ImageComponent::ImageComponent(int x, int y, int width, int height, int margin, const Cover& c)
    : Component(x, y, width, height, margin), cover(c) {}

void ImageComponent::draw(DisplayType& display) {
    std::vector<uint8_t> dithered = cover.getDitheredPixels();
    std::vector<uint8_t> packed = cover.getPackedPixels(dithered);

    std::tuple<int, int> startPos = getStartPos();
    std::tuple<int, int> dimensions = getDimension();
    int startX = std::get<0>(startPos);
    int startY = std::get<1>(startPos);
    int imgWidth = std::get<0>(dimensions);
    int imgHeight = std::get<1>(dimensions);

    // draw cover where cover refers to a one dimensional 1-bit vector 
    display.drawBitmap(startX, startY, packed.data(), imgWidth, imgHeight, GxEPD_BLACK);

    // draw a 2 pixel rect around the cover
    for (int i = 0; i < 2; i++) {
        display.drawRect(
            startX - i - 1,                 // upper left corner position x
            startY - i - 1,                 // upper left corner position y
            imgWidth + 2 * (i + 1),
            imgHeight + 2 * (i + 1), 
            GxEPD_BLACK);                   // color
    }
}