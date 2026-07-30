#include <Arduino.h>
#include <Preferences.h>

void setup() {
    Serial.begin(115200);
    delay(10000);

    Serial.println("Spotify Refresh-Token eingeben und mit Enter bestaetigen:");

    String token = "";
    while (token.isEmpty()) {
        if (Serial.available()) {
            token = Serial.readStringUntil('\n');
            token.trim();
        }
    }

    Preferences prefs;
    prefs.begin("spotify", false);
    prefs.putString("refresh_token", token);
    prefs.end();

    Serial.println("Gespeichert! Token-Laenge: " + String(token.length()));
    Serial.println("Du kannst jetzt die eigentliche Firmware wieder flashen.");
}

void loop() {}
