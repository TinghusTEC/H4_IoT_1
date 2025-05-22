#include <Arduino.h>

struct ButtonLedMap {
    int buttonPin;
    int ledPin;
    bool ledState;
    unsigned long lastDebounceTime;
    int lastButtonState;
    int stableButtonState;
};

ButtonLedMap mappings[] = {
    {26, 23, false, 0, HIGH, HIGH},
    {27, 22, false, 0, HIGH, HIGH},
    {14, 19, false, 0, HIGH, HIGH},
    {13, 18, false, 0, HIGH, HIGH}
};
const int numMappings = sizeof(mappings) / sizeof(mappings[0]);
const unsigned long debounceDelay = 50; // ms

void setup() {
    Serial.begin(115200);
    delay(100);

    for (int i = 0; i < numMappings; ++i) {
        pinMode(mappings[i].buttonPin, INPUT_PULLUP);
        pinMode(mappings[i].ledPin, OUTPUT);
        digitalWrite(mappings[i].ledPin, LOW);
    }
    Serial.println("Button-to-LED toggle ready.");
}

void loop() {
    for (int i = 0; i < numMappings; ++i) {
        int reading = digitalRead(mappings[i].buttonPin);

        if (reading != mappings[i].lastButtonState) {
            mappings[i].lastDebounceTime = millis();
        }

        if ((millis() - mappings[i].lastDebounceTime) > debounceDelay) {
            // Only toggle if the stable state has changed
            if (reading != mappings[i].stableButtonState) {
                mappings[i].stableButtonState = reading;
                // Button press detected (active LOW)
                if (reading == LOW) {
                    mappings[i].ledState = !mappings[i].ledState;
                    digitalWrite(mappings[i].ledPin, mappings[i].ledState ? HIGH : LOW);
                    Serial.print("Button on GPIO ");
                    Serial.print(mappings[i].buttonPin);
                    Serial.print(" toggled LED on GPIO ");
                    Serial.print(mappings[i].ledPin);
                    Serial.print(" to ");
                    Serial.println(mappings[i].ledState ? "ON" : "OFF");
                }
            }
        }
        mappings[i].lastButtonState = reading;
    }
    delay(10);
}