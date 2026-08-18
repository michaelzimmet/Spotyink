#pragma once

#include <cstdint>
#include <tuple>
#include "../DisplayConfig.h"

class Component {
    protected:
        int x, y;
        int width, height;
        int margin;

public:
    Component(int x, int y, int width, int height, int margin)
        : x(x), y(y), width(width), height(height), margin(margin) {}

    virtual void draw(DisplayType& display) = 0;
    virtual ~Component() = default;

    int getX() const { return x; }
    int getY() const { return y; }
    int getEndX() const { return x + width; }
    int getEndY() const { return y + height; }
    std::tuple<int, int> getDimension() const { return std::make_tuple(width - 2 * margin, height - 2 * margin); }
    std::tuple<int, int> getStartPos() const { return std::make_tuple(x + margin, y + margin); } 
};