#include <WiFiMulti.h>
#include <AceButton.h>  // by Brian T. Park (v. 1.10.1 installed)
using namespace ace_button;
#include "secrets.h"
#define USE_SERIAL Serial
#include "GameLogic.h"

WiFiMulti WiFiMulti;

GameLogic game(WiFiMulti, "192.168.0.11", 5505, "ESP32", "621639");  // represent game state

AceButton buttons[] = { AceButton(32), AceButton(33), AceButton(23), AceButton(22) };

const int buzzerPin = 25;

void goodBeep() {
  tone(buzzerPin, 988, 200);
  tone(buzzerPin, 1319, 200);
}

void badBeep() {
  tone(buzzerPin, 800, 150);
  tone(buzzerPin, 150, 150);
}

void errorBeep() {
  tone(buzzerPin, 150, 100);
  delay(200);
  tone(buzzerPin, 150, 100);
  delay(200);
  tone(buzzerPin, 150, 100);
}

void triumph() {
  tone(buzzerPin, 500, 200);
  delay(100);
  tone(buzzerPin, 900, 100);
  delay(100);
  tone(buzzerPin, 600, 100);
  tone(buzzerPin, 800, 100);
  tone(buzzerPin, 1000, 100);
}

void basicBeep() {
  tone(buzzerPin, 700, 200);
}

void handleEvent(AceButton*, uint8_t, uint8_t);

// new pins:
// Buzzer pin: Pin 25
// Top right: Pin 33
// Top left: Pin 32
// Bottom Left: Pin 22
// Bottom Right: Pin 23

void setup() {
  USE_SERIAL.begin(115200);

  // set up buttons
  pinMode(buzzerPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  for (auto& button : buttons) {
    pinMode(button.getPin(), INPUT_PULLUP);
    button.setEventHandler(handleEvent);
  }
  basicBeep();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFiMulti.addAP(SSID, WIFI_PASSWORD);

  //WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  String ip = WiFi.localIP().toString();
  USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());
  game.setOnSubmitAnswer(basicBeep);
  game.setOnError(errorBeep);
  game.setOnJoinGame(triumph);
  game.setOnCorrect(goodBeep);
  game.setOnIncorrect(badBeep);
  game.setOnJoinServer(triumph);
  // game.setOnPlaying();
  // game.setOnPaused();
  game.start();
}

void loop() {
  game.loop();
  for (auto& button : buttons) {
    button.check();
  }
}

inline uint8_t convertPinNumberToAnswerNumber(uint8_t pinNumber) {
  switch (pinNumber) {
    case 33:
      return 0;
    case 32:
      return 1;
    case 22:
      return 2;
    case 23:
      return 3;
    default:
      return 4;
  }
}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  // TODO: when button is pressed, then send an event to the server with the Answer Number.
  Serial.printf("Pin %d = %d, eventType = %d; millis=%lu\n", button->getPin(), buttonState, eventType, millis());
  switch (eventType) {
    case AceButton::kEventPressed:
      {
        digitalWrite(LED_BUILTIN, HIGH);
        uint8_t answer = convertPinNumberToAnswerNumber(button->getPin());
        game.trySubmitAnswer(answer);
      }
      break;
    case AceButton::kEventReleased:
      digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}
