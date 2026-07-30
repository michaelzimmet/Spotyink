#pragma once
#include <Arduino.h>
#include <HTTPClient.h>

class SpotifyAuth {
    private:
        String clientId;
        String clientSecret;
        String redirectUri;

        String accessToken;
        String refreshToken;
        unsigned long tokenExpiresAt;
        HTTPClient http;

        static constexpr const char* TOKEN_URL = "https://accounts.spotify.com/api/token";
        static constexpr const char* AUTHORIZATION_URL = "https://accounts.spotify.com/authorize";

        void saveRefreshToken();
        bool parseTokenResponse(const String& responseBody);

    public:
        SpotifyAuth(const String& clientId, const String& clientSecret, const String& redirectUri);

        void loadRefreshToken();
        bool refreshAccessToken();
        String getValidAccessToken();
        bool hasRefreshToken() const;
};