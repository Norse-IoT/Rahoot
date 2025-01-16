#include <WiFi.h>
#include <AceButton.h>  // by Brian T. Park (v. 1.10.1 installed)
#include "secrets.h"

// new pins:
// Buzzer pin: 25
// Top right: 32
// Top left: 33
// Bottom Left - Pin 36
// Bottom Right - Pin 39

using namespace ace_button;

const int BUTTON_PIN = 2;

AceButton buttons[] = { AceButton(32), AceButton(33), AceButton(23), AceButton(22) };

void handleEvent(AceButton*, uint8_t, uint8_t);

void setup() {
  Serial.begin(115200);
  Serial.println("on");

  // set up buttons
  pinMode(LED_BUILTIN, OUTPUT);
  for (auto& button : buttons) {
    pinMode(button.getPin(), INPUT_PULLUP);
    button.setEventHandler(handleEvent);
  }

  // set up wifi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
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
      digitalWrite(LED_BUILTIN, HIGH);
      break;
    case AceButton::kEventReleased:
      digitalWrite(LED_BUILTIN, LOW);
      break;
  }
}


/**
 * Pins:

 * Top Left - Pin 23
 * Top Right - Pin 22

 *
 * Buzzer - 34 - PWM 
 */



// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   WiFi.begin(ssid, password);
//   Serial.println("\nConnecting");

//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(100);
//   }

//   Serial.println("\nConnected to the WiFi network");
//   Serial.print("Local ESP32 IP: ");
//   Serial.println(WiFi.localIP());
// }

// void loop() {}