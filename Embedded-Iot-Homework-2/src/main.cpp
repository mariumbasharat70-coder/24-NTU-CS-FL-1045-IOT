#include <Arduino.h>

// ---------- Pins ----------
#define LED1_PIN 25
#define LED2_PIN 26

// ---------- PWM configuration ----------
const uint32_t LED1_FREQ = 1000;   // Hz
const uint32_t LED2_FREQ = 5000;   // Hz
const uint8_t  PWM_RES   = 10;     // bits -> duty 0..1023
const int      MAX_DUTY  = (1 << PWM_RES) - 1;

#if ESP_ARDUINO_VERSION_MAJOR < 3
  // Core 2.x: channels 0 and 2 -> different LEDC timers
  const uint8_t LED1_CH = 0;
  const uint8_t LED2_CH = 2;
#endif

// ---------- Fade state (shared with the OLED in Task 2) ----------
int duty1 = 0, dir1 = 1, step1 = 8;   // LED1 fades faster
int duty2 = 0, dir2 = 1, step2 = 5;   // LED2 fades slower

void writeDuty(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? LED1_CH : LED2_CH, duty);
#endif
}

// Advance one fade step, bounce at 0 and MAX_DUTY
void fade(int &duty, int &dir, int step) {
  duty += dir * step;
  if (duty >= MAX_DUTY) { duty = MAX_DUTY; dir = -1; }
  if (duty <= 0)        { duty = 0;        dir =  1; }
}

void setup() {
  Serial.begin(115200);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED1_PIN, LED1_FREQ, PWM_RES);
  ledcAttach(LED2_PIN, LED2_FREQ, PWM_RES);
#else
  ledcSetup(LED1_CH, LED1_FREQ, PWM_RES);
  ledcSetup(LED2_CH, LED2_FREQ, PWM_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);
  ledcAttachPin(LED2_PIN, LED2_CH);
#endif
}

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 15;

void loop() {
  unsigned long now = millis();

  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    fade(duty1, dir1, step1);
    fade(duty2, dir2, step2);

    writeDuty(1, duty1);
    writeDuty(2, duty2);

    Serial.printf(
      "LED1 %3d%% @%luHz | LED2 %3d%% @%luHz\n",
      duty1 * 100 / MAX_DUTY, (unsigned long)LED1_FREQ,
      duty2 * 100 / MAX_DUTY, (unsigned long)LED2_FREQ
    );
  }
}
