#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal.h>

// Setup LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// RTC object
RTC_DS3231 rtc;

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

  if (now.hour() == 14 && now.minute() == 2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Drink Water");
  }


  delay(1000); // Update every second
}

