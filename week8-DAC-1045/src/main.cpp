#include <Arduino.h>

#define DAC_PIN 26          // GPIO26 = DAC2
#define FADE_DELAY_MS 20

void setup()
{
    Serial.begin(115200);
    Serial.println("Program 2: gradual DAC fade on GPIO26 (DAC2)");
}

void loop()
{
    // fade up: 0 -> 255
    for (int value = 0; value <= 255; value++)
    {
        dacWrite(DAC_PIN, value);
        delay(FADE_DELAY_MS);
    }

    // fade down: 255 -> 0
    for (int value = 255; value >= 0; value--)
    {
        dacWrite(DAC_PIN, value);
        delay(FADE_DELAY_MS);
    }
}