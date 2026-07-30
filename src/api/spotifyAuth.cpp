#include "../include/api/spotifyAuth.h"
#include <Preferences.h>
#include <base64.h>
#include <ArduinoJson.h>

SpotifyAuth::SpotifyAuth(const String& clientId, const String& clientSecret, const String& redirectUri)
    : clientId(clientId), clientSecret(clientSecret), redirectUri(redirectUri) {}

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

   http.addHeader("Authorization", "Basic " + base64::encode(clientId + ":" + clientSecret));
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");

   String body = "grant_type=refresh_token";
   body += "&refresh_token=" + refreshToken;

   int statusCode = http.POST(body);
   bool success = false;

   if (statusCode == 200) {
        success = parseTokenResponse(http.getString());
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