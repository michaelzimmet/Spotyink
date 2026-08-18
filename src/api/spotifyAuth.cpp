#include "../include/api/spotifyAuth.h"
#include <Preferences.h>
#include <base64.h>
#include <ArduinoJson.h>

SpotifyAuth::SpotifyAuth(const String& clientId, const String& redirectUri)
    : clientId(clientId), redirectUri(redirectUri) {}

String SpotifyAuth::getValidAccessToken() {
    if (millis() >= tokenExpiresAt) {
        refreshAccessToken();
    }
    return accessToken;
}

void SpotifyAuth::saveRefreshToken() {
    Preferences prefs;
    prefs.begin("spotify", false);
    prefs.putString("refresh_token", refreshToken);
    prefs.end();   
}

void SpotifyAuth::loadRefreshToken() {
    Preferences prefs;
    prefs.begin("spotify", true); // true = read-only
    refreshToken = prefs.getString("refresh_token", "");
    prefs.end();  
}

bool SpotifyAuth::hasRefreshToken() const {
    return !refreshToken.isEmpty();
}

bool SpotifyAuth::refreshAccessToken() {
   http.begin(TOKEN_URL);

   http.addHeader("Content-Type", "application/x-www-form-urlencoded");

   String body = "grant_type=refresh_token";
   body += "&refresh_token=" + refreshToken;
   body += "&client_id=" + clientId;

   int statusCode = http.POST(body);
   bool success = false;
   String responseBody = http.getString();

   if (statusCode == 200) {
        success = parseTokenResponse(responseBody);
   } else {
        Serial.printf("Token refresh failed, status %d: %s\n", statusCode, responseBody.c_str());
   }

   http.end();
   return success;
}

bool SpotifyAuth::parseTokenResponse(const String& responseBody) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, responseBody);

    if (error) {
        return false;
    }

    accessToken = doc["access_token"].as<String>();
    int expiresIn = doc["expires_in"];

    // (runtime_in_ms) + (expiresIn_in_sec converted to ms) - (one minute as buffer)
    tokenExpiresAt = millis() + (expiresIn * 1000UL) - 60000UL;

    if (!doc["refresh_token"].isNull()) {
        refreshToken = doc["refresh_token"].as<String>();
        saveRefreshToken();
    }

    return true;
}