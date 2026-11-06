# Week 8 Lab: ESP32 DAC and PWM LED Control

**Name:** Marium Basahrat
**Registration No:** 24-NTU-CS-FL-1045
**Course:** Embedded IoT Systems (BSCS 5th Semester, Section A)

## Objective

To control LED brightness on an ESP32 using the built-in 8-bit DAC (GPIO25 / GPIO26) and to compare it with PWM output (GPIO16).

## Hardware / Tools

- ESP32 development board (or Wokwi simulator)
- 2 x LED
- 2 x resistor (220-330 ohm)
- Breadboard and jumper wires
- PlatformIO / Arduino framework

## Repository Structure

```
Week8-DAC-Lab/
├── program1/                    # Step 2: Stepped DAC output
├── program2/                    # Step 3: Gradual fade (20 ms)
├── program2_delay_experiment/   # Step 4: 5 ms / 20 ms / 50 ms
├── program_gpio26/              # Step 5: DAC2 on GPIO26
├── program_dac_pwm/             # Step 6: DAC + PWM together
├── images/                      # Circuit screenshots
└── README.md
```

## Lab Procedure Checklist

- [x] 1. Build the GPIO25 + resistor (220-330 ohm) + LED circuit
- [x] 2. Run Program 1 (Stepped Output) for 0, 64, 128, 192, 255
- [x] 3. Run Program 2 and observe the brightness fade
- [x] 4. Change the loop delay (20 ms to 5 ms, then 50 ms) and compare
- [x] 5. Move the LED to GPIO26 and use DAC2
- [x] 6. Run the simultaneous DAC/PWM script (GPIO25 + GPIO16)

---

## Step 1: Circuit

GPIO25 -> resistor (220-330 ohm) -> LED (+) -> LED (-) -> GND

![Step 1 circuit](images/step1.png)

## Step 2: Program 1 (Stepped Output)

Five DAC values are written one after another using `dacWrite(DAC_PIN, value)`.

```cpp
int dacValues[] = {0, 64, 128, 192, 255};
```

| DAC value | Output voltage | LED observation |
|---|---|---|
| 0 | 0 V | Off |
| 64 | ~0.83 V | Usually off / very dim |
| 128 | ~1.65 V | Dim |
| 192 | ~2.48 V | Glowing |
| 255 | ~3.3 V | Highest brightness |

**Key insight:** LED brightness is not proportional to the DAC code because of the LED's non-linear current-voltage characteristics.

## Step 3: Program 2 (Gradual Fade)

The DAC value is increased from 0 to 255 and then decreased back to 0 in steps of 1, with `delay(20)` between steps.

**Observation:** The LED fades in and out smoothly. The DAC has only 256 discrete levels, so the fade is technically stepped, but it appears fluid to the human eye. The LED stays dark for the first part of the sweep, then brightens quickly.

## Step 4: Loop Delay Experiment

| Delay | Time per full cycle | Visual effect |
|---|---|---|
| 5 ms | ~2.6 s | Very fast fade, almost pulsing |
| 20 ms | ~10.2 s | Smooth, natural fade |
| 50 ms | ~25.6 s | Very slow fade, individual steps may be noticeable |

**Conclusion:** A smaller delay gives a faster fade and a larger delay gives a slower fade. The number of steps (256) stays the same; only the time per step changes.

## Step 5: LED on GPIO26 (DAC2)

The LED was moved to GPIO26 and the code was changed to `#define DAC_PIN 26`.

**Observation:** The fade behaves the same as on GPIO25. Both GPIO25 (DAC1) and GPIO26 (DAC2) are 8-bit DAC channels with an output range of 0-3.3 V.

## Step 6: Simultaneous DAC and PWM

- LED 1 on GPIO25 is driven by the DAC.
- LED 2 on GPIO16 is driven by PWM (5 kHz, 8-bit).

Both are faded from 0 to 255 and back at the same time.

| Feature | DAC (GPIO25) | PWM (GPIO16) |
|---|---|---|
| Output type | True analog voltage (0-3.3 V) | Digital switching with variable duty cycle |
| Available pins | Only GPIO25 and GPIO26 | Almost any GPIO |
| LED at low values | Stays dark at first (LED needs ~1.8-2 V) | Starts dimly lighting early |
| Fade appearance | Non-linear, brightens quickly later | Smoother and more linear |
| Flicker | None | Not visible at 5 kHz, but the signal is switching |

![Step 6 circuit](images/step6.png)

## Conclusion

- The ESP32 DAC produces a true analog voltage, but only on GPIO25 and GPIO26.
- PWM works on almost any pin and controls brightness through average power, which gives a smoother-looking fade for LEDs.
- LED brightness does not scale linearly with the DAC code because of the LED's non-linear current-voltage characteristics.
- The loop delay controls fade speed without changing the number of brightness levels.