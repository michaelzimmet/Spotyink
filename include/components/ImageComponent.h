
#pragma once
#include "Component.h"
#include "../dataObjs/cover/Cover.h"

class ImageComponent : public Component {
    private:
        Cover cover;
    
    public:
        ImageComponent(int x, int y, int width, int height, int margin, const Cover& c = 
            Cover(216, 216, std::vector<uint8_t>(216 * 216)));
        void draw(DisplayType& display) override;
        void setCover(const Cover& newCover) { cover = newCover; }
};