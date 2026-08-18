#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <vector>
#include <tuple>
#include <utility>
#include <WiFi.h>
#include "../include/dataObjs/song/Song.h"
#include "../include/components/ImageComponent.h"
#include "../include/components/TextComponent.h"
#include "../include/components/SymbolComponent.h"
#include "../include/components/ProgressComponent.h"
#include "../include/api/spotifyApi.h"
#include "../include/secrets.h"

// Crowbar Test Panel
//const int EINK_BUSY = 48;
//const int EINK_RST  = 47;
//const int EINK_DC   = 46;
//const int EINK_CS   = 45;
//const int EINK_SCK  = 12;
//const int EINK_MOSI = 11;
//const int EINK_PWR = 7;

// Waveshare 5.79 G Panel with esp32-c6
const int EINK_BUSY = 2;
const int EINK_RST = 3;
const int EINK_DC = 10;
const int EINK_CS = 7;
const int EINK_SCK = 6;
const int EINK_MOSI = 5;
const int EINK_PWR = 11;

SpotifyAuth spotifyAuth(SPOTIFY_CLIENT_ID, SPOTIFY_REDIRECT_URI);
SpotifyApi spotifyApi(spotifyAuth);
DisplayType display(GxEPD2_579_GDEY0579T93(EINK_CS, EINK_DC, EINK_RST, EINK_BUSY));

String lastTrackId = "";
Song currentSong("", "", 0);

ImageComponent coverComponent(0, 0, 272, 272, 28);
TextComponent titleComponent(272, 0, 520, 60, 28, currentSong.getTitle(), &FreeMonoBold18pt7b, GxEPD_BLACK);
TextComponent artistComponent(272, titleComponent.getEndY(), 520, 30, 28, currentSong.getAristsJoined(2, ", "), &FreeMonoBold12pt7b, GxEPD_BLACK);    
SymbolComponent pausePlayButton(272, artistComponent.getEndY(), 520, 100, 28);
ProgressComponent progressBar(272, pausePlayButton.getEndY(), 520, 20, 5);  
String timeText = currentSong.formatTime(currentSong.getCurrentTimeS()) + " / " + currentSong.formatTime(currentSong.getTotalTimeS());
TextComponent timeComponent(272, progressBar.getEndY(), 520, 60, 28, timeText, &FreeMonoBold12pt7b, GxEPD_BLACK);

void displayPowerOn() {
    pinMode(EINK_PWR, OUTPUT);
    digitalWrite(EINK_PWR, HIGH);
}

void initDisplay() {
    Serial.begin(115200);
    displayPowerOn();
    display.init(0);
    display.setRotation(0);
}

void setup() {
    SPI.begin(EINK_SCK, -1, EINK_MOSI, EINK_CS);
    SPI.setHwCs(false);

    pinMode(EINK_CS, OUTPUT);
    pinMode(EINK_DC, OUTPUT);
    pinMode(EINK_RST, OUTPUT);
    pinMode(EINK_BUSY, INPUT);
 
    initDisplay();

    Serial.println("Connecting ot WLAN...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.setSleep(false);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println("Successfully connected to WLAN");

    spotifyAuth.loadRefreshToken();

    if (!spotifyAuth.hasRefreshToken()) {
        Serial.println("No Spotify-Refreshtoken available");
        return;
    }
    Serial.println("Spotify-Refreshtoken available");
}

void updateProgress(Song& currentSong) {

    timeComponent.setText(currentSong.getTimeString());

    float progress = static_cast<float>(currentSong.getCurrentTimeMs()) / currentSong.getTotalTimeMs();
    progressBar.setProgress(progress);

    pausePlayButton.setSymbol(currentSong.isPlaying() ? SymbolType::Play : SymbolType::Pause);

    int areaX = pausePlayButton.getX();
    int areaY = pausePlayButton.getY();
    int areaWidth = timeComponent.getEndX() - areaX;
    int areaHeight = timeComponent.getEndY() - areaY;

    display.setPartialWindow(areaX, areaY, areaWidth, areaHeight);
    display.firstPage();
    do {
        pausePlayButton.draw(display);
        progressBar.draw(display);
        timeComponent.draw(display);
    } while (display.nextPage());

}

void renderFullScreen(Song& currentSong) {
    coverComponent.setCover(currentSong.getCover());
    titleComponent.setText(currentSong.getTitle());
    artistComponent.setText(currentSong.getAristsJoined(2, ", "));
    timeComponent.setText(currentSong.getTimeString());
    
    display.setFullWindow();
    display.firstPage();

    do {
        display.fillScreen(GxEPD_WHITE);
        coverComponent.draw(display);
        titleComponent.draw(display);
        artistComponent.draw(display);
        pausePlayButton.draw(display);
        progressBar.draw(display);
        timeComponent.draw(display);
    } while (display.nextPage());
}

const unsigned long FETCH_INTERVAL_MS = 5000;
const unsigned long REFRESH_INTERVAL_MS = 800;
unsigned long lastFetchMs = 0;
unsigned long lastRefreshMs = 0;

void loop() {
    delay(200);
    unsigned long now = millis();
    bool fetched = false;

    // fetch every 5 seconds
    if (now - lastFetchMs >= FETCH_INTERVAL_MS) {
        lastFetchMs = now;
        Song newSong("", "", 0);
        if (spotifyApi.getCurrentlyPlaying(newSong)) {
            if (newSong.getId() != lastTrackId) {
                if (!spotifyApi.getCurrentCover(newSong)) {
                    Cover cover(216, 216, std::vector<uint8_t>(((216 + 7) / 8) * 216, 0xFF));
                    newSong.setCover(cover);
                }

                lastTrackId = newSong.getId();
                currentSong = newSong;

                renderFullScreen(currentSong);
                return;
            }

            currentSong.setPlaying(newSong.isPlaying());
            currentSong.setCurrentTimeMs(newSong.getCurrentTimeMs());
            fetched = true;
        }
    }

    // update every 0.8 seconds
    if (now - lastRefreshMs < REFRESH_INTERVAL_MS) {
        return;
    }

    if (fetched || currentSong.isPlaying()) {
        if (!fetched) {
            unsigned long updatedTimeMs = std::min(
                currentSong.getCurrentTimeMs() + REFRESH_INTERVAL_MS, currentSong.getTotalTimeMs());
            currentSong.setCurrentTimeMs(updatedTimeMs);
        }

        lastRefreshMs = millis();
        updateProgress(currentSong);
    }
}