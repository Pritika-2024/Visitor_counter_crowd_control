#include <LiquidCrystalI2C.h>
#include <Wire.h>
#include <Servo.h>
#include <Keypad.h>

#define ENTRY_SENSOR 2
#define EXIT_SENSOR 3
#define DOOR_SERVO_PIN 9

int visitor_count = 0;
LiquidCrystalI2C lcd(0x27, 16, 2);
Servo doorServo;
bool maxCountReached = false;
bool lastEntryState = HIGH;
bool lastExitState = HIGH;
bool doorMoving = false;
unsigned long lastExitTime = 0;
const int debounceDelay = 1000;

const byte ROWNUM = 4;
const byte COLUMNNUM = 4;
byte pinrows[ROWNUM] = {12, 11, 10, 8};
byte pincolumn[COLUMNNUM] = {4, 5, 6, 7};
char keys[ROWNUM][COLUMNNUM] = {
  {'A', '3', '2', '1'},
  {'B', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'D', '#', '0', '*'}
};

Keypad keypad = Keypad(makeKeymap(keys), pinrows, pincolumn, ROWNUM, COLUMNNUM);

String validRegistrationNumbers[] = {"1238", "5678", "9101", "1121"};
const int maxVisitors = 4;
String enteredNumber = "";

void updateDisplay();
bool isValidRegistrationNumber(String number);
String getRegistrationNumber();
void handleEntry();
void handleExit();

void setup() {
  pinMode(ENTRY_SENSOR, INPUT);
  pinMode(EXIT_SENSOR, INPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  doorServo.attach(DOOR_SERVO_PIN);
  delay(100);
  doorServo.write(0);
  updateDisplay();
}

void loop() {
  if (!doorMoving) {
    bool entryState = digitalRead(ENTRY_SENSOR);
    bool exitState = digitalRead(EXIT_SENSOR);

    if (entryState == LOW && lastEntryState == HIGH) {
      handleEntry();
    }
    lastEntryState = entryState;

    if (exitState == LOW && lastExitState == HIGH && (millis() - lastExitTime > debounceDelay)) {
      handleExit();
      lastExitTime = millis();
    }
    lastExitState = exitState;
  }
}

void handleEntry() {
  if (visitor_count < maxVisitors) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Reg. Number");
    enteredNumber = getRegistrationNumber();

    if (isValidRegistrationNumber(enteredNumber)) {
      visitor_count++;
      Serial.print("Visitor Entered | Count: ");
      Serial.println(visitor_count);
      updateDisplay();
      doorMoving = true;
      doorServo.write(90);
      delay(3000);
      doorServo.write(0);
      delay(1000);
      doorMoving = false;
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Number");
      delay(2000);
      lcd.clear();
    }
  } else {
    maxCountReached = true;
    updateDisplay();
  }
}

void handleExit() {
  if (visitor_count > 0) {
    visitor_count--;
    Serial.print("Visitor Exited | Count: ");
    Serial.println(visitor_count);
    maxCountReached = false;
    updateDisplay();
  }
}

String getRegistrationNumber() {
  String number = "";
  char key;
  lcd.setCursor(0, 1);
  while (true) {
    key = keypad.getKey();
    if (key) {
      if (key == '#') {
        if (number.length() == 4) {
          return number;
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Enter 4 digits! ");
          delay(2000);
          lcd.setCursor(0, 1);
          lcd.print("                ");
          number = "";
          lcd.setCursor(0, 1);
        }
      } else if (key == '*') {
        number = "";
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
      } else if (number.length() < 4) {
        number += key;
        lcd.print(key);
      }
    }
  }
}

bool isValidRegistrationNumber(String number) {
  int listSize = sizeof(validRegistrationNumbers) / sizeof(validRegistrationNumbers[0]);
  for (int i = 0; i < listSize; i++) {
    if (validRegistrationNumbers[i] == number) {
      return true;
    }
  }
  return false;
}

void updateDisplay() {
  lcd.clear();
  if (maxCountReached) {
    lcd.setCursor(0, 0);
    lcd.print("Max Count Reached");
    lcd.setCursor(0, 1);
    lcd.print("Entry Prohibited");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Visitor Counter");
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(visitor_count);
    lcd.setCursor(12, 1);
    lcd.print("/4");
  }
}
