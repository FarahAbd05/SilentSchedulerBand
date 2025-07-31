#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

// Setup LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 8, 5, 4, 3, 2);

// RTC object
RTC_DS3231 rtc;

// RGB LED Pins
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;

// Motor pin (connected through L293D Input 1)
const int motorPin = 7;


void setup() {
  // Start Serial Monitor
  Serial.begin(9600);

  // Start LCD
  lcd.begin(16, 2);

  // Initialize RTC
  if (!rtc.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("RTC not found");
    while (1); // Stop here
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting time to compile time.");
    lcd.setCursor(0, 1);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(motorPin, OUTPUT);
}

void setColor(bool redOn, bool greenOn, bool blueOn) {
  digitalWrite(redPin, redOn);
  digitalWrite(greenPin, greenOn);
  digitalWrite(bluePin, blueOn);
}

void buzzPattern1() {
  digitalWrite(motorPin, HIGH);
  delay(200);
  digitalWrite(motorPin, LOW);
  delay(150);
  digitalWrite(motorPin, HIGH);
  delay(200);
  digitalWrite(motorPin, LOW);
}


void loop() {
  DateTime now = rtc.now();

  // Print to Serial Monitor
  Serial.print("Time: ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.println(now.second());

  // Print to LCD
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.setCursor(6, 0); // Aligns time in row 0 after "Time: "
  
  // Format time as HH:MM:SS
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());


  // if (now.hour() == 14 && now.minute() == 2) {
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Drink Water");
  // }

  // // RGB Color Cycle (1s per color)
  // analogWrite(9, 255);  // Red
  // analogWrite(10, 0);
  // analogWrite(11, 0);
  // delay(1000);

  // analogWrite(9, 0);
  // analogWrite(10, 255); // Green
  // analogWrite(11, 0);
  // delay(1000);

  // analogWrite(9, 0);
  // analogWrite(10, 0);
  // analogWrite(11, 255); // Blue
  // delay(1000);

  // analogWrite(9, 128);  // Purple
  // analogWrite(10, 0);
  // analogWrite(11, 128);
  // delay(1000);

  // === Task Alert: Drink Water at 14:30:00 ===
  if (now.hour() == 20 && now.minute() == 41 && now.second() == 20) {
    lcd.setCursor(0, 1);
    lcd.print("Drink Water     ");
    setColor(0, 0, 1); // Blue
    buzzPattern1();
  } else {
    lcd.setCursor(0, 1);
    lcd.print("                "); // clear message
    setColor(0, 0, 0); // turn off LED
  }

  delay(1000);
}

