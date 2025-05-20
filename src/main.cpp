#include <Arduino.h>
#include "LEDHandler.h"
#include "ClickHandler.h"

const int buttonPin = 0; // ESP32 built-in BOOT button
const int ledPin = 2;    // ESP32 built-in LED

LEDHandler led(ledPin, true);
ClickHandler clickHandler;

void onButtonClick() {
    Serial.println("Button pressed");
    led.turnOnForSeconds(7);
}

void setup() {
    Serial.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.println("Setup started");
}

void loop() {
    if (digitalRead(buttonPin) == LOW) {
        clickHandler.handleClick(onButtonClick);
        while (digitalRead(buttonPin) == LOW) {
            delay(10);
        }
    }
}