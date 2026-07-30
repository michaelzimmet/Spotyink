#pragma once

#include <ArduinoJson.h>
#include <WString.h>
#include <cstdio>
#include <vector>
#include <limits.h>
#include "../cover/Cover.h"

class Song {
    private:
        String id_;
        String title_;
        std::vector<String> artists_;
        bool isPlaying_;
        unsigned long totalTimeMs_;
        unsigned long currentTimeMs_;
        Cover cover_;
        std::vector<String> coverUrls_;
        std::vector<int> coverWidths_;

    public:
        Song(String id, String t, int ttm);

        void setArtists(const std::vector<String>& s) { artists_ = s; }
        void setPlaying(bool s) { isPlaying_ = s; }
        void setCurrentTimeMs(int s) { currentTimeMs_ = s; }
        void setCover(const Cover& c) { cover_ = c; }
        void setCoverInfo(const std::vector<String>& url, const std::vector<int>& width);

        const std::vector<String>& getArtists() const { return artists_; }
        const String& getTitle() const { return title_; }
        const String& getId() const { return id_; }
        bool isPlaying() const { return isPlaying_; }
        unsigned long getTotalTimeMs() const { return totalTimeMs_; }
        unsigned long getCurrentTimeMs() const { return currentTimeMs_; }
        int getTotalTimeS() const { return totalTimeMs_ / 1000; }
        int getCurrentTimeS() const { return currentTimeMs_ / 1000; }
        const Cover& getCover() const { return cover_; }
        const std::tuple<String, int> getCoverInfo(int preferredWidth) const;

        const String formatTime(int totalSeconds);
        const String getAristsJoined(size_t numArtists, const String& separator) const;
        const String getTimeString();
        static bool fromJson(JsonObject data, Song& outSong);
};