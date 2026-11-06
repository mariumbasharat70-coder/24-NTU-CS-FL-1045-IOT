#include <Arduino.h>

#define DAC_PIN 25

void setup()
{
    Serial.begin(115200);
    Serial.println("ESP32 DAC Brightness Fade");
}

void loop()
{
    for (int value = 0; value <= 255; value++)
    {
        dacWrite(DAC_PIN, value);
        delay(10);
    }

    for (int value = 255; value >= 0; value--)
    {
        dacWrite(DAC_PIN, value);
        delay(10);
    }
}