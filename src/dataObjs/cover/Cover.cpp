#include "../include/dataObjs/cover/Cover.h"
#include <cmath>

Cover::Cover() : width(10), height(10), pixels(width * height, 0) {}
Cover::Cover(int w, int h, std::vector<uint8_t> p) : width(w), height(h), pixels(p) {}

void Cover::addError(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& edgeMap, int idx, int error, int numerator, int denominator) const {
    if (idx < 0 || idx >= (int)buffer.size())  {
        return;
    }

    float edgeFactor = edgeMap[idx] / 255.0f;
    float diffusionFactor = 1.0f - edgeFactor;
    int scaledError = static_cast<int>(error * diffusionFactor);

    int newVal = static_cast<int>(buffer[idx]) + (scaledError * numerator) / denominator;
    buffer[idx] = static_cast<uint8_t>(std::max(0, std::min(newVal, 255)));

}

std::vector<uint8_t> Cover::computeEdgeStrength(const std::vector<uint8_t>& source) const {
    std::vector<uint8_t> edges(source.size(), 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;

            int left = (x > 0) ? source[idx - 1] : source[idx];
            int right = (x < width - 1) ? source[idx + 1] : source[idx];
            int up = (y > 0) ? source[idx - width] : source[idx];
            int down = (y < height - 1) ? source[idx + width] : source[idx];

            int gx = right - left;
            int gy = down - up;
            int magnitude = static_cast<int>(std::sqrt(static_cast<float>(gx * gx + gy * gy)));

            edges[idx] = static_cast<uint8_t>(std::min(magnitude, 255));
        }
    }

    return edges;
}

void Cover::setBit(std::vector<uint8_t>& packed, int bytesPerRow, int x, int y) const {
    int byteIdx = y * bytesPerRow + ( x / 8 );
    int bitIdx = 7 - (x % 8);
    packed[byteIdx] |= (1 << bitIdx);
}

std::vector<uint8_t> Cover::stretchContrast(const std::vector<uint8_t>& source) const {
    uint8_t minVal = 255;
    uint8_t maxVal = 0;
    for (uint8_t p : source) {
        minVal = std::min(minVal, p);
        maxVal = std::max(maxVal, p);
    }

    if (maxVal <= minVal) {
        return source;
    }

    std::vector<uint8_t> result(source.size());
    float scale = 255.0f / (maxVal - minVal);
    for (size_t i = 0; i < source.size(); i++) {
        int stretched = static_cast<int>((source[i] - minVal) * scale);
        result[i] = static_cast<uint8_t>(std::max(0, std::min(stretched, 255)));
    }

    return result;
}

std::vector<uint8_t> Cover::applyContrastCurve(const std::vector<uint8_t>& source) const {
    const float STEEPNESS = 6.0f; // hoeher = Mitteltoene werden aggressiver Richtung Schwarz/Weiss gedrueckt
    const float CENTER = 0.5f;

    float sMin = 1.0f / (1.0f + std::exp(STEEPNESS * CENTER));
    float sMax = 1.0f / (1.0f + std::exp(-STEEPNESS * (1.0f - CENTER)));
    float range = sMax - sMin;

    std::vector<uint8_t> result(source.size());
    for (size_t i = 0; i < source.size(); i++) {
        float normalized = source[i] / 255.0f;
        float curved = 1.0f / (1.0f + std::exp(-STEEPNESS * (normalized - CENTER)));
        float scaled = (curved - sMin) / range * 255.0f;
        int value = static_cast<int>(scaled + 0.5f);
        result[i] = static_cast<uint8_t>(std::max(0, std::min(value, 255)));
    }

    return result;
}

std::vector<uint8_t> Cover::getDitheredPixels() const {
    std::vector<uint8_t> result = applyContrastCurve(stretchContrast(pixels));
    std::vector<uint8_t> edgeMap = computeEdgeStrength(result);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = y * width + x;

            uint8_t oldPixel = result[idx];
            // 128 refers to the threshold between black and white
            uint8_t newPixel = (oldPixel < 128) ? 0 : 255;
            int error = oldPixel - newPixel;
            result[idx] = newPixel;

            // Jarvis-Judice-Ninke: 5x5-Nachbarschaft, Divisor 48 - streut den Fehler breiter,
            // damit eher kleine Cluster statt isolierter Einzelpixel entstehen.
            // addError drosselt die Streuung zusaetzlich an starken Kanten (edgeMap), damit die
            // Kanten nicht durch diffundierten Fehler verwaschen werden.
            if (x + 1 < width) {
                addError(result, edgeMap, idx + 1, error, 7, 48);
            }
            if (x + 2 < width) {
                addError(result, edgeMap, idx + 2, error, 5, 48);
            }
            if (y + 1 < height) {
                if (x - 2 >= 0) {
                    addError(result, edgeMap, idx + width - 2, error, 3, 48);
                }
                if (x - 1 >= 0) {
                    addError(result, edgeMap, idx + width - 1, error, 5, 48);
                }
                addError(result, edgeMap, idx + width, error, 7, 48);
                if (x + 1 < width) {
                    addError(result, edgeMap, idx + width + 1, error, 5, 48);
                }
                if (x + 2 < width) {
                    addError(result, edgeMap, idx + width + 2, error, 3, 48);
                }
            }
            if (y + 2 < height) {
                if (x - 2 >= 0) {
                    addError(result, edgeMap, idx + 2 * width - 2, error, 1, 48);
                }
                if (x - 1 >= 0) {
                    addError(result, edgeMap, idx + 2 * width - 1, error, 3, 48);
                }
                addError(result, edgeMap, idx + 2 * width, error, 5, 48);
                if (x + 1 < width) {
                    addError(result, edgeMap, idx + 2 * width + 1, error, 3, 48);
                }
                if (x + 2 < width) {
                    addError(result, edgeMap, idx + 2 * width + 2, error, 1, 48);
                }
            }
        }
    }

    return result;
}

std::vector<uint8_t> Cover::getPackedPixels(const std::vector<uint8_t>& sourcePixels) const {
    int bytesPerRow = (width + 7) / 8;
    std::vector<uint8_t> packed(bytesPerRow * height, 0x00);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t gray = sourcePixels[y * width + x];
            if (gray < 128) {
                setBit(packed, bytesPerRow, x, y);
            }
        }
    }

    return packed;
}