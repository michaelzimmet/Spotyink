#pragma once

#include <vector>
#include <cstdint>

class Cover {
    private:
        int width;
        int height;
        std::vector<uint8_t> pixels;

        void addError(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& edgeMap, int idx, int error, int numerator, int denominator) const;
        void setBit(std::vector<uint8_t>& packed, int bytesPerRow, int x, int y) const;
        std::vector<uint8_t> stretchContrast(const std::vector<uint8_t>& source) const;
        std::vector<uint8_t> applyContrastCurve(const std::vector<uint8_t>& source) const;
        std::vector<uint8_t> computeEdgeStrength(const std::vector<uint8_t>& source) const;
    
    public:
        Cover();
        Cover(int width, int height, std::vector<uint8_t> p);

        int getWidth() const { return width; }
        int getHeight() const { return height;}
        const std::vector<uint8_t>& getPixels() const { return pixels; }
        std::vector<uint8_t> getDitheredPixels() const;
        std::vector<uint8_t> getPackedPixels(const std::vector<uint8_t>& sourcePixels) const;
};