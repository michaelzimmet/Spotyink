#pragma once
#include "Component.h"

class TextComponent : public Component {
    private:
        String text;
        const GFXfont* font;
        uint16_t color;

    public:
        TextComponent(int x, int y, int width, int height, int margin, 
            const String& t, const GFXfont* font = nullptr, uint16_t color = GxEPD_BLACK);
        void draw(DisplayType& display) override;
        void setText(const String& s) { text = s; }
};