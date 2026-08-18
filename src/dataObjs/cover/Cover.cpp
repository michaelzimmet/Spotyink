#include "../include/dataObjs/cover/Cover.h"
#include <cmath>
#include <utility>
#include <Arduino.h>
#include <TJpg_Decoder.h>

std::vector<uint8_t>* Cover::decodeTarget = nullptr;
int Cover::decodeTargetW = 0, Cover::decodeSrcW = 0;
int Cover::decodeTargetH = 0, Cover::decodeSrcH = 0;

Cover::Cover() : width(10), height(10), pixels(width * height, 0) {}
Cover::Cover(int w, int h, std::vector<uint8_t> p) : width(w), height(h), pixels(std::move(p)) {}

bool Cover::jpegDecodeCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    const uint8_t* gray = reinterpret_cast<const uint8_t*>(bitmap);

    for (int16_t j = 0; j < h; j++) {
        for (int16_t i = 0; i < w; i++) {
            int16_t srcX = x + i, srcY = y + j;
            if (srcX >= decodeSrcW || srcY >= decodeSrcH) continue;

            int dstX = (srcX * decodeTargetW) / decodeSrcW; 
            int dstY = (srcY * decodeTargetH) / decodeSrcH;

            (*decodeTarget)[dstY * decodeTargetW + dstX] = gray[j * w + i];
        }
    }
    return true;
}

namespace {
    void addClamped(std::vector<uint8_t>& buf, int idx, int16_t error) {
        int16_t v = int16_t(buf[idx]) + error;
        if (v < 0) v = 0;
        if (v > 255) v = 255;
        buf[idx] = uint8_t(v);
    }
}

void Cover::decode(int sourceWidth, int sourceHeight, int targetWidth, int targetHeight) {
    std::vector<uint8_t> grayTarget(targetWidth * targetHeight, 255); 

    decodeTarget = &grayTarget;
    decodeSrcW = sourceWidth;
    decodeSrcH = sourceHeight;
    decodeTargetW = targetWidth;
    decodeTargetH = targetHeight;

    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(jpegDecodeCallback);
    TJpgDec.drawJpg(0, 0, pixels.data(), pixels.size()); 

    int bytesPerRow = (targetWidth + 7) / 8;
    std::vector<uint8_t> packedTarget(bytesPerRow * targetHeight, 0xFF);

    for (int y = 0; y < targetHeight; y++) {
        for (int x = 0; x < targetWidth; x++) {
            int idx = y * targetWidth + x;

            uint8_t oldVal = grayTarget[idx];
            uint8_t newVal = oldVal > 127 ? 1 : 0; // 1 = weiss, 0 = schwarz
            setPackedPixel(packedTarget, targetWidth, x, y, newVal);

            int16_t error = int16_t(oldVal) - (newVal ? 255 : 0);
            if (x + 1 < targetWidth)                          addClamped(grayTarget, idx + 1,               error * 7 / 16);
            if (y + 1 < targetHeight && x > 0)                addClamped(grayTarget, idx + targetWidth - 1, error * 3 / 16);
            if (y + 1 < targetHeight)                         addClamped(grayTarget, idx + targetWidth,     error * 5 / 16);
            if (y + 1 < targetHeight && x + 1 < targetWidth)  addClamped(grayTarget, idx + targetWidth + 1, error * 1 / 16);
        }
    }

    width = targetWidth;
    height = targetHeight;
    pixels = std::move(packedTarget);
}

void Cover::setPackedPixel(std::vector<uint8_t>& buf, int w, int x, int y, uint8_t value) {
    int bytesPerRow = (w + 7) / 8;
    int byteIndex = y * bytesPerRow + x / 8;
    uint8_t mask = 0x80 >> (x % 8);
    if (value) buf[byteIndex] |= mask;
    else buf[byteIndex] &= ~mask;
}

uint8_t Cover::getPackedPixel(const std::vector<uint8_t>& buf, int w, int x, int y) {
    int bytesPerRow = (w + 7) / 8;
    int byteIndex = y * bytesPerRow + x / 8;
    uint8_t mask = 0x80 >> (x % 8);
    return (buf[byteIndex] & mask) ? 1 : 0;
}

uint8_t Cover::getPixelAt(int x, int y) const {
    return getPackedPixel(pixels, width, x, y);
}
