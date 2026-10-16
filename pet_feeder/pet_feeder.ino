/*
  Automated Pet Feeder
  CSU Bakersfield, Spring 2026

  Dispenses food on a schedule the owner chooses. At startup, turn the
  knob to pick a feeding interval (1 minute to 24 hours) and press the
  button to confirm. The feeder dispenses once right away, then again
  every interval, showing a countdown to the next feeding on the LCD.

  Hardware: Arduino Uno R3, stepper motor + step/dir stepper driver,
  16x2 I2C LCD, potentiometer, push button.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int dirPin    = 2;   // Stepper driver DIR
const int stepPin   = 3;   // Stepper driver STEP
const int buttonPin = 7;   // Push button to GND (uses the internal pull-up)
const int vrPin     = A0;  // Potentiometer wiper, used to pick the interval

LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16x2 LCD at I2C address 0x27

// How long the motor spins each feeding
const unsigned long dispenseTimeMs = 5000;  // 5 seconds

// Motor speed (microseconds between step pulses)
const int stepDelayUs = 1000;

// Feeding intervals the owner can choose from, in minutes
const unsigned int intervalMinutes[] = {
  1, 5, 10, 15, 30, 60, 120, 360, 720, 1440
};

const int numIntervals = sizeof(intervalMinutes) / sizeof(intervalMinutes[0]);

unsigned long feedIntervalMs = 0;
unsigned long nextFeedTime = 0;

// The button connects the pin to GND, so pressed reads LOW
bool buttonPressed() {
  return digitalRead(buttonPin) == LOW;
}

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // No external resistor needed

  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);

  lcd.init();
  lcd.backlight();

  feedIntervalMs = chooseFeedInterval();

  dispenseFood();

  nextFeedTime = millis() + feedIntervalMs;
}

void loop() {
  // Subtracting before comparing keeps this working when millis() rolls over
  if ((long)(millis() - nextFeedTime) >= 0) {
    dispenseFood();
    nextFeedTime = millis() + feedIntervalMs;
  }

  displayTimeRemaining();
}

// Let the owner pick an interval with the knob, then confirm with the button
unsigned long chooseFeedInterval() {
  int lastIndex = -1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set interval");

  while (true) {
    int vrValue = analogRead(vrPin);

    int index = map(vrValue, 0, 1023, 0, numIntervals - 1);
    index = constrain(index, 0, numIntervals - 1);

    // Only redraw the LCD when the selection changes
    if (index != lastIndex) {
      lastIndex = index;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set interval:");

      lcd.setCursor(0, 1);
      printInterval(intervalMinutes[index]);
      lcd.print(" press");
    }

    if (buttonPressed()) {
      delay(200);  // Debounce

      while (buttonPressed()) {
        // Wait until the button is released
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Selected:");

      lcd.setCursor(0, 1);
      printInterval(intervalMinutes[index]);

      delay(1000);

      return (unsigned long)intervalMinutes[index] * 60000UL;
    }

    delay(100);
  }
}

// Spin the stepper for dispenseTimeMs to drop a portion of food
void dispenseFood() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dispensing");

  unsigned long startTime = millis();

  while (millis() - startTime < dispenseTimeMs) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelayUs);

    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelayUs);
  }

  digitalWrite(stepPin, LOW);

  delay(100);

  // Re-initialize the LCD in case motor noise scrambled it
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Done");
  delay(1000);
}

// Show a countdown to the next feeding, updated once per second
void displayTimeRemaining() {
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();

    unsigned long remainingMs = nextFeedTime - millis();
    unsigned long remainingSeconds = remainingMs / 1000;

    unsigned long hours = remainingSeconds / 3600;
    unsigned long minutes = (remainingSeconds % 3600) / 60;
    unsigned long seconds = remainingSeconds % 60;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Next feed in:");

    lcd.setCursor(0, 1);

    if (hours > 0) {
      lcd.print(hours);
      lcd.print("h ");
      lcd.print(minutes);
      lcd.print("m");
    } else {
      lcd.print(minutes);
      lcd.print("m ");
      lcd.print(seconds);
      lcd.print("s");
    }
  }
}

// Print an interval as "15 min", "2 hr", or "1h 30m"
void printInterval(unsigned int minutes) {
  if (minutes < 60) {
    lcd.print(minutes);
    lcd.print(" min");
  } else if (minutes % 60 == 0) {
    lcd.print(minutes / 60);
    lcd.print(" hr");
  } else {
    lcd.print(minutes / 60);
    lcd.print("h ");
    lcd.print(minutes % 60);
    lcd.print("m");
  }
}
