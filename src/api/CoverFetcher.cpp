#include "../include/api/CoverFetcher.h"
#include <HTTPClient.h>
#include <TJpg_Decoder.h>

namespace {
    std::vector<uint32_t>* g_sumBuffer = nullptr;
    std::vector<uint16_t>* g_countBuffer = nullptr;
    int g_nativeSize = 0;
    int g_targetSize = 0;

    bool onJpgBlock(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
        if (!g_sumBuffer || !g_countBuffer) {
            return false;
        }

        for (uint16_t by = 0; by < h; by++) {
            int srcY = y + by;
            int dstY = (srcY * g_targetSize) / g_nativeSize;
            if (dstY < 0 || dstY >= g_targetSize) {
                continue;
            }

            for (uint16_t bx = 0; bx < w; bx++) {
                int srcX = x + bx;
                int dstX = (srcX * g_targetSize) / g_nativeSize;
                if (dstX < 0 || dstX >= g_targetSize) {
                    continue;
                }

                uint16_t pixel = bitmap[by * w + bx];
                uint8_t r5 = (pixel >> 11) & 0x1F;
                uint8_t g6 = (pixel >> 5) & 0x3F;
                uint8_t b5 = pixel & 0x1F;
                uint8_t r8 = (r5 * 255) / 31;
                uint8_t g8 = (g6 * 255) / 63;
                uint8_t b8 = (b5 * 255) / 31;
                uint8_t gray = static_cast<uint8_t>(0.299f * r8 + 0.587f * g8 + 0.114f * b8);

                int dstIdx = dstY * g_targetSize + dstX;
                (*g_sumBuffer)[dstIdx] += gray;
                (*g_countBuffer)[dstIdx] += 1;
            }
        }

        return true;
    }

    bool downloadJpeg(const String& url, std::vector<uint8_t>& outData) {
        HTTPClient http;
        http.begin(url);

        int statusCode = http.GET();
        if (statusCode != 200) {
            http.end();
            return false;
        }

        String body = http.getString();
        http.end();

        if (body.length() == 0) {
            return false;
        }

        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(body.c_str());
        outData.assign(bytes, bytes + body.length());
        return true;
    }
}

bool fetchCover(const String& url, int nativeSize, int targetSize, Cover& outCover) {

    if (url.isEmpty() || nativeSize <= 0) {
        return false;
    }

    std::vector<uint8_t> jpegData;
    if (!downloadJpeg(url, jpegData)) {
        return false;
    }

    std::vector<uint32_t> sumBuffer(targetSize * targetSize, 0);
    std::vector<uint16_t> countBuffer(targetSize * targetSize, 0);
    g_sumBuffer = &sumBuffer;
    g_countBuffer = &countBuffer;
    g_nativeSize = nativeSize;
    g_targetSize = targetSize;

    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(onJpgBlock);

    JRESULT result = TJpgDec.drawJpg(0, 0, jpegData.data(), jpegData.size());

    g_sumBuffer = nullptr;
    g_countBuffer = nullptr;

    if (result != JDR_OK) {
        return false;
    }

    std::vector<uint8_t> targetPixels(targetSize * targetSize, 255);
    for (int i = 0; i < targetSize * targetSize; i++) {
        if (countBuffer[i] > 0) {
            targetPixels[i] = static_cast<uint8_t>(sumBuffer[i] / countBuffer[i]);
        }
    }

    outCover = Cover(targetSize, targetSize, targetPixels);
    return true;
}
