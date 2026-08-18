#pragma once

#include <vector>
#include <cstdint>

class Cover {
    private:
        int width;
        int height;
        std::vector<uint8_t> pixels;

        static std::vector<uint8_t>* decodeTarget;  // Graustufen-Zwischenpuffer fuer TJpg_Decoder-Callback
        static int decodeTargetW;
        static int decodeTargetH;
        static int decodeSrcW;
        static int decodeSrcH;

        static bool jpegDecodeCallback(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
        static void setPackedPixel(std::vector<uint8_t>& buf, int w, int x, int y, uint8_t value);
        static uint8_t getPackedPixel(const std::vector<uint8_t>& buf, int w, int x, int y);

    public:
        Cover();
        Cover(int width, int height, std::vector<uint8_t> p);

        int getWidth() const { return width; }
        int getHeight() const { return height;}
        const std::vector<uint8_t>& getPixels() const { return pixels; }

        void decode(int sourceWidth, int sourceHeight, int targetWidth, int targetHeight);
        uint8_t getPixelAt(int x, int y) const;
};
