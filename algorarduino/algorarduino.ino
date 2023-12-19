#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int redLedPin = 9;
const int yellowLedPin = 8;
const int greenLedPin = 7;

// Адреса I2C для LCD-дисплея (залежить від вашого конкретного дисплея)
const int lcdAddress = 0x3f;
LiquidCrystal_I2C lcd(lcdAddress, 16, 2);

int getNumberOfCars(int current_cars);


void setup() {
    Serial.begin(9600);

  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  // Ініціалізація I2C LCD-дисплея
  
  lcd.init();
  lcd.backlight();

  digitalWrite(9, HIGH);
}
int numberOfCars = 0;

unsigned seconds = 0;
unsigned period = 4;
unsigned new_period = period;

void loop() {
  getNumberOfCars();

  // Виведення часу очікування на I2C LCD-дисплей
  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  if (seconds <= period*3/8) {
    digitalWrite(greenLedPin, HIGH);
    lcd.print("  GREEN");
    lcd.setCursor(0, 1);
    lcd.print(seconds);
  } else if (seconds <= period*4/8) {
    lcd.print("  YELLOW");
    digitalWrite(yellowLedPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(seconds - period*3/8);
  } else if (seconds <= period*7/8){
    digitalWrite(redLedPin, HIGH);
    lcd.print("  RED");
    lcd.setCursor(0, 1);
    lcd.print(seconds - period*4/8);
  } else {
    lcd.print("  YELLOW");
    digitalWrite(yellowLedPin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(seconds - period*7/8);
  }
  
  
  lcd.print(" s");

  delay(1000);
  seconds++;
  if (seconds > period) {
    seconds = 0;
    period = new_period;
  }
}

int getNumberOfCars() {
   if (Serial.available() > 0) {
    // Отримати рядок з Serial
    String receivedData = Serial.readString();
    // Виконати дії з отриманим рядком, наприклад, вивести його
    unsigned cars = atoi(receivedData.c_str());
    new_period = convertCarsToTime(cars);
  }
}

int convertCarsToTime(int numberOfCars) {
  if (numberOfCars <= 3) {
    return 8;
  } else if (numberOfCars <= 5 ) {
    return 16;
  } else {
    return 24;
  }
}

