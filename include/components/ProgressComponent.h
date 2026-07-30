#pragma once
#include "Component.h"

class ProgressComponent : public Component {
    private:
        float progress;
        uint16_t color;

    public:
        ProgressComponent(int x, int y, int width, int height, int margin, 
            uint16_t color = GxEPD_BLACK);
        void draw(DisplayType& display) override;

        void setProgress(float p) { progress = std::max(0.0f, std::min(1.0f, p)); }
};