# Automated Pet Feeder

An Arduino-controlled pet feeder that dispenses food on a schedule the owner chooses, built by a 7-person team at **California State University, Bakersfield** (Spring 2026).

At startup, the owner turns a knob to pick a feeding interval and presses a button to confirm. The feeder dispenses a portion right away, then again every interval, with a live countdown to the next feeding on the LCD.

## Features

- **Adjustable schedule:** choose from 1, 5, 10, 15, or 30 minutes, or 1, 2, 6, 12, or 24 hours
- **Stepper-motor dispensing:** the motor runs for a set time each feeding to drop a consistent portion
- **LCD status display:** shows the interval being selected, "Dispensing" while feeding, and a countdown to the next feeding
- **Noise-tolerant display:** the LCD is re-initialized after each feeding in case motor noise scrambles it

## Hardware

| Part | Purpose |
|---|---|
| Arduino Uno R3 | Runs the schedule and controls everything |
| Stepper motor + step/dir stepper driver | Turns the dispensing mechanism |
| 16×2 I2C LCD (address 0x27) | Shows the interval, status, and countdown |
| Potentiometer | Selects the feeding interval |
| Push button | Confirms the selection |
| AC/DC adapter | Powers the system |
| 3D-printed housing | Food container, dispensing mechanism, electronics housing, and bowl, designed in Fusion 360 |

## Wiring

| Arduino pin | Connects to |
|---|---|
| 2 | Stepper driver DIR |
| 3 | Stepper driver STEP |
| 7 | Push button (other side to GND) |
| A0 | Potentiometer middle pin (outer pins to 5V and GND) |
| A4 / A5 | LCD SDA / SCL |

## How it works

1. **Choose an interval.** The potentiometer reading is mapped to one of ten preset intervals, shown on the LCD. Pressing the button locks it in.
2. **Dispense.** The Arduino pulses the stepper driver for 5 seconds, turning the dispensing mechanism to release food.
3. **Wait and repeat.** The LCD counts down to the next feeding, and the cycle repeats every interval.

To change the portion size, edit `dispenseTimeMs`. To change the motor speed, edit `stepDelayUs`.

## Running it

1. In the Arduino IDE, install the **LiquidCrystal I2C** library (Sketch → Include Library → Manage Libraries).
2. Open `pet_feeder/pet_feeder.ino`, select **Arduino Uno** as the board, and upload.

If the LCD stays blank, your display may use I2C address `0x3F` instead of `0x27`.

## Notes on this version

This is a cleaned-up copy of the team's sketch. Comments were added, and the button now uses the Arduino's internal pull-up resistor (`INPUT_PULLUP`), since the circuit had no external resistor on the button. The feeding logic is unchanged.

## Team

Jagbinder Bhullar, Victor Flores, Andres Leal, Armando Mata, Adrian Ramos, Nathan Sanabia, and Damian Delgado (Engineering Design Lead). Department of Computer and Electrical Engineering and Computer Science, CSU Bakersfield.
