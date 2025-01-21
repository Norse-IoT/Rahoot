#include "secrets.h"
#define USE_SERIAL Serial

#include "GameLogic.h"
#include <AceButton.h>  // by Brian T. Park (v. 1.10.1 installed)

using namespace ace_button;

#include <WiFiMulti.h>

WiFiMulti WiFiMulti;

GameLogic game(WiFiMulti, "10.0.1.66", 5505, "ESP32", "621639");  // represent game state

AceButton buttons[] = { AceButton(32), AceButton(33), AceButton(23), AceButton(22) };

void handleEvent(AceButton*, uint8_t, uint8_t);

// new pins:
// Buzzer pin: 25
// Top right: 32
// Top left: 33
// Bottom Left - Pin 36
// Bottom Right - Pin 39

void setup() {
  USE_SERIAL.begin(115200);

  // set up buttons
  pinMode(LED_BUILTIN, OUTPUT);
  for (auto& button : buttons) {
    pinMode(button.getPin(), INPUT_PULLUP);
    button.setEventHandler(handleEvent);
  }

  while (millis() < 5000) {
    USE_SERIAL.print(".");
  }
  USE_SERIAL.println();

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
