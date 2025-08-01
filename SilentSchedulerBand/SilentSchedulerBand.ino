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

// Pushbutton pin (dismiss button)
const int dismissPin = A0;

// Task structure to hold task details
struct Task {
  uint8_t hour;       // Task hour (24-hour format)
  uint8_t minute;     // Task minute
  uint8_t second;     // Task second
  const char* message;  // Task message to display
  // RGB color for task
  bool redOn;
  bool greenOn;
  bool blueOn;
  // Buzz pattern selector (1 or 2)
  uint8_t buzzPattern;
};

//Define multiple tasks here
Task tasks[] = {
  // Morning Hygiene + Health
  {7, 30, 0, "Brush Teeth",      1, 1, 1, 1},   // White,   Buzz 1
  {7, 45, 0, "Check Insulin",    0, 1, 0, 2},   // Green,   Buzz 2
  {8, 0, 0, "Eat Breakfast",     0, 0, 1, 1},   // blue,    Buzz 1
  {8, 15, 0, "Take Meds",        1, 0, 0, 2},   // Red,     Buzz 2
  {9, 30, 0, "Take Shower",      0, 0, 1, 1},   // blue,    Buzz 1

  // Midday Movement + Health
  {11, 0, 0, "Drink Water",      0, 0, 1, 1},   // blue     Buzz 1
  {12, 0, 0, "Eat Lunch",        0, 0, 1, 1},   // blue,    Buzz 1
  {13, 30, 0, "Stretch Body",    0, 0, 1, 1},   // blue,    Buzz 1
  {14, 30, 0, "Check Insulin",   0, 1, 0, 2},   // Green,   Buzz 2

  // Evening Routine + Social
  {15, 30, 0, "Drink Water",     0, 0, 1, 1},   // blue,    Buzz 1
  {17, 30, 0, "Take Meds",       1, 0, 0, 2},   // Red,     Buzz 2
  {18, 30, 0, "Eat Dinner",      0, 0, 1, 1},   // blue,    Buzz 1
  {20, 0, 0, "Call Family",      0, 1, 0, 2},   // Green,   Buzz 2
  {21, 30, 0, "Brush Teeth",     1, 1, 1, 1},   // White,   Buzz 1

  // Night Health Check
  {22, 0, 0, "Check Insulin",    0, 1, 0, 2},   // Green,   Buzz 2
  {22, 15, 0, "Take Meds",       1, 0, 0, 2}    // Red,     Buzz 2
};

const int numTasks = sizeof(tasks) / sizeof(tasks[0]);

// State variable to track if alert is active
bool alertActive = false;
// Track current task index being alerted
int currentTaskIndex = -1;

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
    lcd.setCursor(0, 1);
    lcd.print("RTC lost power");
  }

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  
  // Setup dismiss button pin as input
  pinMode(dismissPin, INPUT);
}

void setColor(bool redOn, bool greenOn, bool blueOn) {
  digitalWrite(redPin, redOn ? HIGH : LOW);
  digitalWrite(greenPin, greenOn ? HIGH : LOW);
  digitalWrite(bluePin, blueOn ? HIGH : LOW);
}

// Buzz pattern 1: one long buzz
void buzzPattern1() {
  digitalWrite(motorPin, HIGH);
  delay(500);
  digitalWrite(motorPin, LOW);
  delay(200);
}

// Buzz pattern 2: two short buzzes
void buzzPattern2() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(motorPin, HIGH);
    delay(200);
    digitalWrite(motorPin, LOW);
    delay(200);
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

  // Display current time on LCD
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());
  lcd.print("  ");  // Clear trailing chars if any

  // Check if dismiss button pressed
  int dismissState = digitalRead(dismissPin);
  if (dismissState == HIGH && alertActive) {
    // User pressed dismiss button, so alert stops 
    alertActive = false;
    currentTaskIndex = -1;
    lcd.setCursor(0, 1);
    lcd.print("Alert dismissed  ");
    setColor(0, 0, 0);
    digitalWrite(motorPin, LOW);
    delay(5000); // Give a moment to show dismiss message
  }

  // If no alert active, check for new tasks to trigger
  if (!alertActive) {
    for (int i = 0; i < numTasks; i++) {
      if (now.hour() == tasks[i].hour && now.minute() == tasks[i].minute && now.second() == tasks[i].second) {
        // Trigger task alert
        alertActive = true;
        currentTaskIndex = i;
        break;
      }
    }
  }

  // If alert is active, show task info and run buzz + LED
  if (alertActive && currentTaskIndex != -1) {
    Task currentTask = tasks[currentTaskIndex];

    // Display task message on LCD
    lcd.setCursor(0, 1);
    lcd.print(currentTask.message);
    int len = strlen(currentTask.message);
    // Clear any leftover characters on LCD line if message shorter than 16
    for (int i = len; i < 16; i++) {
      lcd.print(' ');
    }

    // Set LED color
    setColor(currentTask.redOn, currentTask.greenOn, currentTask.blueOn);

    // Run buzz pattern
    if (currentTask.buzzPattern == 1) {
      buzzPattern1();
    } else if (currentTask.buzzPattern == 2) {
      buzzPattern2();
    }
  } else {
    // No alert active: clear LCD second line and LEDs/motor
    lcd.setCursor(0, 1);
    lcd.print("                ");  // clear line
    setColor(0, 0, 0);
    digitalWrite(motorPin, LOW);
  }

  delay(1000);  // small delay for timing smoothness
}

