#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// ESP32 pins
const int BUTTON_PIN = 18;
const int SDA_PIN = 21;
const int SCL_PIN = 22;

// Debounce settings
const unsigned long DEBOUNCE_MS = 50;

Adafruit_SSD1306 display(
    SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);

int pressCount = 0;

int lastRawReading = HIGH;
int stableButtonState = HIGH;

unsigned long lastChangeTime = 0;

void updateDisplay() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("ESP32 BUTTON");

    display.drawLine(0, 12, 127, 12, SSD1306_WHITE);

    display.setCursor(0, 20);
    display.print("Status: ");

    if (stableButtonState == LOW) {
        display.println("PRESSED");
    } else {
        display.println("RELEASED");
    }

    display.setCursor(0, 38);
    display.print("Press Count: ");
    display.println(pressCount);

    display.display();
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Wire.begin(SDA_PIN, SCL_PIN);

    if (!display.begin(
            SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        Serial.println("OLED initialization failed!");
        while (true) {
            // Stop if the OLED cannot initialize.
        }
    }

    display.clearDisplay();
    updateDisplay();

    Serial.println("ESP32 OLED Button Counter Ready");
    Serial.println("Press the button to increase count.");
}

void loop() {
    unsigned long currentTime = millis();

    // Read the button. LOW means pressed.
    int rawReading = digitalRead(BUTTON_PIN);

    // Detect a possible change in the raw input.
    if (rawReading != lastRawReading) {
        lastChangeTime = currentTime;
        lastRawReading = rawReading;
    }

    // Accept the change only after it stays stable.
    if (currentTime - lastChangeTime >= DEBOUNCE_MS) {
        if (rawReading != stableButtonState) {
            stableButtonState = rawReading;

            // Count only the transition to PRESSED.
            if (stableButtonState == LOW) {
                pressCount++;

                Serial.print("Accepted press count: ");
                Serial.println(pressCount);
            }

            updateDisplay();
        }
    }
}