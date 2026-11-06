#include <Arduino.h>

#define DAC_PIN 25          // true analog output (DAC1)
#define PWM_PIN 16          // PWM output
#define PWM_FREQ 5000       // 5 kHz
#define PWM_RES 8           // 8-bit (0-255)
#define PWM_CHANNEL 0       // used only in core 2.x
#define FADE_DELAY_MS 20

void setup()
{
    Serial.begin(115200);
    Serial.println("Program 3: DAC (GPIO25) + PWM (GPIO16) simultaneous fade");

#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(PWM_PIN, PWM_FREQ, PWM_RES);
#else
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);
#endif
}

void writePWM(int value)
{
#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PWM_PIN, value);
#else
    ledcWrite(PWM_CHANNEL, value);
#endif
}

void loop()
{
    // fade up: 0 -> 255
    for (int value = 0; value <= 255; value++)
    {
        dacWrite(DAC_PIN, value);
        writePWM(value);
        delay(FADE_DELAY_MS);
    }

    // fade down: 255 -> 0
    for (int value = 255; value >= 0; value--)
    {
        dacWrite(DAC_PIN, value);
        writePWM(value);
        delay(FADE_DELAY_MS);
    }
}