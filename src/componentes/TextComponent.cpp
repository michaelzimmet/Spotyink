#include "../include/components/TextComponent.h"

TextComponent::TextComponent(int x, int y, int width, int height, int margin, 
    const String& t, const GFXfont* font, uint16_t color)
    : Component(x, y, width, height, margin), text(t), font(font), color(color) {}

void TextComponent::draw(DisplayType& display) {
    display.setFont(font);
    display.setTextColor(color);
    display.setTextWrap(false);

    std::tuple<int, int> dimensions = getDimension();
    int rawWidth = std::get<0>(dimensions);
    int rawHeight = std::get<1>(dimensions);

    int16_t boundsX, boundsY;
    uint16_t textWidth, textHeight;
    String displayText = text;
    display.getTextBounds(displayText, 0, 0, &boundsX, &boundsY, &textWidth, &textHeight);

    // if text is to long for display area remove text until it fits with "..." at the end
    if (textWidth > rawWidth) {
        String truncated = text;
        while (truncated.length() > 0) {
            truncated.remove(truncated.length() - 1);
            String candidate = truncated + "...";
            display.getTextBounds(candidate, 0, 0, &boundsX, &boundsY, &textWidth, &textHeight);
            if (textWidth <= rawWidth) {
                displayText = candidate;
                break;
            }
        }
    }

    // calc starting position of text area
    std::tuple<int, int> startPos = getStartPos();
    int xStartPos = std::get<0>(startPos);
    int yStartPos = std::get<1>(startPos);

    // calc starting position for centered text
    int cursorX = xStartPos + (rawWidth - textWidth) / 2;
    int cursorY = yStartPos + (rawHeight + textHeight) / 2;

    display.setCursor(cursorX, cursorY);
    display.print(displayText);
}