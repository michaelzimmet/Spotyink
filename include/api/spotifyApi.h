#pragma once
#include "spotifyAuth.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "../dataObjs/song/Song.h"

class SpotifyApi {
    private:
        SpotifyAuth& auth;
        HTTPClient http;

        static constexpr const char* API_BASE_URL = "https://api.spotify.com/v1";

    public:
        SpotifyApi(SpotifyAuth& auth);

        bool getCurrentlyPlaying(Song& outSong);
        bool getCurrentlyPlayingStream(Song& outSong);
        bool getCurrentCover(Song& outSong);

        void nextSong();
        void toggleCurrentSongState(Song& currentSong);
        void restartCurrentSong();
        void previousSong();
};