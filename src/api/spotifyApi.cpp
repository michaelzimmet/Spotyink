
#include "../include/api/spotifyApi.h"

SpotifyApi::SpotifyApi(SpotifyAuth& auth) : auth(auth) {
    http.setReuse(true);
}

bool SpotifyApi::getCurrentlyPlaying(Song& outSong) {
    http.begin(String(API_BASE_URL) + "/me/player/currently-playing?market=DE");
    http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());

    int statusCode = http.GET();
    bool success = false;

    if (statusCode == 200) {
        JsonDocument filter;
        filter["is_playing"] = true;
        filter["progress_ms"] = true;
        filter["item"]["name"] = true;
        filter["item"]["id"] = true;
        filter["item"]["duration_ms"] = true;
        filter["item"]["artists"] = true;
        filter["item"]["album"]["images"] = true;

        JsonDocument doc;
        deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
        success = Song::fromJson(doc.as<JsonObject>(), outSong);
    }

    http.end();
    return success;
}

bool SpotifyApi::getCurrentlyPlayingStream(Song& outSong) {
    http.begin(String(API_BASE_URL) + "/me/player/currently-playing?market=DE");
    http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());
    
    int statusCode = http.GET();
    int success = false;

    if (statusCode == 200) {
        JsonDocument filter;
        filter["is_playing"] = true;
        filter["progress_ms"] = true;
        filter["item"]["name"] = true;
        filter["item"]["id"] = true;
        filter["item"]["duration_ms"] = true;
        filter["item"]["artists"] = true;
        filter["item"]["album"]["images"] = true;

        NetworkClient* stream = &http.getStream();
        int streamLength = http.getSize();

        JsonDocument doc;
        deserializeJson(doc, *stream, DeserializationOption::Filter(filter));
        success = Song::fromJson(doc.as<JsonObject>(), outSong);
    }
    http.end();
    return false;
}

bool SpotifyApi::getCurrentCover(Song& outSong) {
    return false;
}

void SpotifyApi::nextSong() {
    http.begin(String(API_BASE_URL) + "/me/player/next");
    http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());

    int statusCode = http.GET();
    http.end();
}

void SpotifyApi::previousSong() {
    http.begin(String(API_BASE_URL) + "/me/player/previous");
    http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());

    int statusCode = http.GET();
    http.end();
}

void SpotifyApi::toggleCurrentSongState(Song& currentSong) {
    if (!currentSong.isPlaying()) {
        http.begin(String(API_BASE_URL) + "/me/player/play");
        http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());
        int statusCode = http.GET();
    } else {
        http.begin(String(API_BASE_URL) + "/me/player/pause");
        http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());  
    }
    
    currentSong.setPlaying(!currentSong.isPlaying());
    http.end();

}

void SpotifyApi::restartCurrentSong() {
    http.begin(String(API_BASE_URL) + "/me/player/seek?position_ms=0");
    http.addHeader("Authorization", "Bearer " + auth.getValidAccessToken());

    int statusCode = http.GET();
    http.end();
}

