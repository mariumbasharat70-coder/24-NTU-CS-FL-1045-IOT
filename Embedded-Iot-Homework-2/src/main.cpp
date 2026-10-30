#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pins ----------
const int LED1_PIN = 18;
const int LED2_PIN = 19;
const int OLED_SDA = 21;
const int OLED_SCL = 22;

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- PWM settings ----------
// Channels 0 and 1 share one LEDC timer (same frequency), so LED2 uses channel 2
// to get its own timer and a different frequency.
const int LED1_CH = 0;
const int LED2_CH = 2;
const int LED1_FREQ = 1000;   // Hz
const int LED2_FREQ = 5000;   // Hz
const int PWM_RES = 10;       // bits -> duty range 0..1023
const int MAX_DUTY = (1 << PWM_RES) - 1;

// ---------- Fade state (same variables are used for PWM output and OLED text) ----------
int duty1 = 0;
int duty2 = MAX_DUTY;
int dir1 = 1;
int dir2 = -1;
const int STEP1 = 4;
const int STEP2 = 6;
const unsigned long FADE_INTERVAL = 20;     // ms between fade steps
const unsigned long OLED_INTERVAL = 100;    // ms between OLED refreshes
unsigned long lastFade = 0;
unsigned long lastOled = 0;
unsigned long lastPrint = 0;
bool oledOk = false;

int dutyToPercent(int duty) {
  return (duty * 100 + MAX_DUTY / 2) / MAX_DUTY;
}

void pwmBegin() {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED1_PIN, LED1_FREQ, PWM_RES);
  ledcAttach(LED2_PIN, LED2_FREQ, PWM_RES);
#else
  ledcSetup(LED1_CH, LED1_FREQ, PWM_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);
  ledcSetup(LED2_CH, LED2_FREQ, PWM_RES);
  ledcAttachPin(LED2_PIN, LED2_CH);
#endif
}

void pwmWrite(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? LED1_CH : LED2_CH, duty);
#endif
}

int nextDuty(int duty, int &dir, int step) {
  duty += dir * step;
  if (duty >= MAX_DUTY) { duty = MAX_DUTY; dir = -1; }
  else if (duty <= 0)   { duty = 0;        dir = 1;  }
  return duty;
}

void drawOled() {
  char line1[24];
  char line2[24];
  snprintf(line1, sizeof(line1), "LED1:%3d%% F:%dHz", dutyToPercent(duty1), LED1_FREQ);
  snprintf(line2, sizeof(line2), "LED2:%3d%% F:%dHz", dutyToPercent(duty2), LED2_FREQ);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 4);
  display.print("PWM MONITOR");
  display.setCursor(0, 28);
  display.print(line1);
  display.setCursor(0, 46);
  display.print(line2);
  display.display();
}

void setup() {
  Serial.begin(115200);

  // PWM first, so LEDs work even if the OLED is not found
  pwmBegin();
  pwmWrite(1, duty1);
  pwmWrite(2, duty2);

  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(400000);
  oledOk = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  if (!oledOk) {
    Serial.println("OLED not found - check wiring and I2C address");
  }
}

void loop() {
  unsigned long now = millis();

  if (now - lastFade >= FADE_INTERVAL) {
    lastFade = now;
    duty1 = nextDuty(duty1, dir1, STEP1);
    duty2 = nextDuty(duty2, dir2, STEP2);
    pwmWrite(1, duty1);
    pwmWrite(2, duty2);
  }

  if (oledOk && now - lastOled >= OLED_INTERVAL) {
    lastOled = now;
    drawOled();
  }

  if (now - lastPrint >= 500) {
    lastPrint = now;
    Serial.printf("LED1: %d%% F:%dHz | LED2: %d%% F:%dHz\n",
                  dutyToPercent(duty1), LED1_FREQ,
                  dutyToPercent(duty2), LED2_FREQ);
  }
}
