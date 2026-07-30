// Song.cpp
#include "../include/dataObjs/song/Song.h"

Song::Song(String id, String t, int ttm) : id_(id), title_(t), totalTimeMs_(ttm), currentTimeMs_(0), 
    isPlaying_(false), cover_(), coverUrls_({}), coverWidths_({}) {}


void Song::setCoverInfo(const std::vector<String>& urls, const std::vector<int>& widths) {
    coverUrls_ = urls;
    coverWidths_ = widths;
}

const String Song::getAristsJoined(size_t numArtists, const String& separator) const {
    String result = "";
    size_t limit = std::min(numArtists, artists_.size());
    
    for (size_t i = 0; i < limit ; i++) {
        result += artists_[i];
        if (i < limit - 1) {
            result += separator;
        }
    }
    return result;
}

const std::tuple<String, int> Song::getCoverInfo(int preferredWidth) const {
    int preferredIdx = 0;
    int smallestDifference = std::numeric_limits<int>::max();
    
    for (size_t i = 0; i < coverUrls_.size(); i++) {
        int difference = preferredWidth - coverWidths_[i];
        if (difference < smallestDifference) {
            preferredIdx = i;
            smallestDifference = difference; 
        }
    }

    return std::make_tuple(coverUrls_[preferredIdx], coverWidths_[preferredIdx]);
}

const String Song::formatTime(int totalSeconds) {
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char buffer[6]; // "MM:SS" + null indicator (\0) of sprintf
    sprintf(buffer, "%02d:%02d", minutes, seconds);

    return String(buffer);
}

bool Song::fromJson(JsonObject data, Song &outSong) {
    
    // required song fields ( item contains song info, song name, song id, total duration)
    if (data["item"].isNull()) {
        return false;
    }
    JsonObject item = data["item"];

    if (item["name"].isNull()) {
        return false;
    }
    String title = item["name"].as<String>();

    if (item["id"].isNull()) {
        return false;
    }
    String id = item["id"].as<String>();

    if (item["duration_ms"].isNull()) {
        return false;
    }
    int totalTimeMs = item["duration_ms"];
    
    outSong = Song(id, title, totalTimeMs);

    // optional song fields
    if (!data["is_playing"].isNull()) {
        outSong.setPlaying(data["is_playing"]);
    }

    if (!data["progress_ms"].isNull()) {
        outSong.setCurrentTimeMs(data["progress_ms"]);
    }

    if (!item["artists"].isNull()) {
        std::vector<String> artists;
        for (JsonObject artistsObj : item["artists"].as<JsonArray>()) {
            if (!artistsObj["name"].isNull()) {
                artists.push_back(artistsObj["name"].as<String>());
            }
        }
        outSong.setArtists(artists);
    }

    if (!item["album"].isNull() && !item["album"]["images"].isNull()) {
        std::vector<String> coverUrls;
        std::vector<int> widths;

        for (JsonObject image : item["album"]["images"].as<JsonArray>()) {
            int width = image["width"] | 0;
            String url = image["url"].as<String>();
            coverUrls.push_back(url);
            widths.push_back(width); 
        }

        outSong.setCoverInfo(coverUrls, widths);
    }

    return true;
}

const String Song::getTimeString() {
    return formatTime(getCurrentTimeS()) + " / " + formatTime(getTotalTimeS());
}
