#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins
const int buzzerPin = 8;
const int ledPin = 7;
const int buttonPin = 9;
const int servoPin = 6;

// Servo setup
Servo dispenserServo;

// Timing
unsigned long previousMillis = 0;
const unsigned long interval = 1000;
int countdown = 15;
int cycleCount = 0;
bool waitingForButton = false;
int servoAngle = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Default I2C address
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pill Dispenser");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  dispenserServo.attach(servoPin);
  dispenserServo.write(0);  // Start at 0°
}

void loop() {
  if (!waitingForButton && cycleCount < 4) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      countdown--;

      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Cycle: ");
      display.println(cycleCount + 1);

      display.setCursor(0, 20);
      display.print("Time left: ");
      if (countdown < 10) display.print("0");
      display.print(countdown);
      display.println("s");
      display.display();

      if (countdown <= 0) {
        dispensePill();
        waitingForButton = true;
      }
    }
  }

  if (waitingForButton && digitalRead(buttonPin) == LOW) {
    stopAlarm();
    cycleCount++;

    if (cycleCount >= 4) {
      resetSystem();
    } else {
      countdown = 15;
      previousMillis = millis();
      waitingForButton = false;
    }
  }
}

void dispensePill() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Take your pill!");
  display.display();

  tone(buzzerPin, 1000);
  digitalWrite(ledPin, HIGH);

  servoAngle += 45;
  if (servoAngle > 180) servoAngle = 180;

  dispenserServo.write(servoAngle);
}

void stopAlarm() {
  noTone(buzzerPin);
  digitalWrite(ledPin, LOW);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Pill Taken");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.display();
}

void resetSystem() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Session Done!");
  display.setCursor(0, 20);
  display.println("Resetting...");
  display.display();

  delay(3000);
  dispenserServo.write(0);
  servoAngle = 0;
  countdown = 15;
  cycleCount = 0;
  waitingForButton = false;

  display.clearDisplay();
  display.display();
}