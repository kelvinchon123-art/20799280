#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int PIN_ENA = 3;    
const int PIN_IN1 = A1; 
const int PIN_IN2 = A2; 
const int PIN_ENB = 11;   
const int PIN_IN3 = 13;   
const int PIN_IN4 = 12; 
const int TRIG_PIN = A3; 
const int ECHO_PIN = A4; 

int speedNormal = 160; 
int speedTurn   = 180; 
unsigned long lastLCDUpdate = 0; 

void setup() {
  pinMode(PIN_IN3, OUTPUT); digitalWrite(PIN_IN3, LOW); 
  pinMode(PIN_ENA, OUTPUT); pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENB, OUTPUT); pinMode(PIN_IN4, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("TASK III: Avoid");
  lcd.setCursor(0, 1);
  lcd.print("Status: Ready");
  stopMotors(); 
  delay(1000);
}

void loop() {
  float distance = getDistance(); 
   
  if (distance < 20.0 && distance > 0) {
    stopMotors(); 
    delay(500); 
    setMotorSpeed(-speedNormal, -speedNormal); 
    delay(800); 
    setMotorSpeed(speedTurn, -speedTurn); 
    delay(1000); 
    stopMotors();
    delay(200);
  } else {
    setMotorSpeed(speedNormal, speedNormal);
  }
   
  if (millis() - lastLCDUpdate > 300) {
    updateLCD(distance);
    lastLCDUpdate = millis();
  }
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return 999.0; 
  return (duration * 0.034) / 2.0;
}

void updateLCD(float distance) {
    lcd.setCursor(0, 0); 
    lcd.print("Dist:"); lcd.print(distance, 1); lcd.print("cm    ");
    lcd.setCursor(0, 1);
    if(distance < 20.0 && distance > 0) {
      lcd.print("!!OBSTACLE FOUND!!");
    } else {
      lcd.print("Moving Forward...");
    }
}

void setMotorSpeed(int speedL, int speedR) {
  if (speedL >= 0) {
    digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, HIGH); 
    analogWrite(PIN_ENA, constrain(speedL, 0, 255)); 
  } else {
    digitalWrite(PIN_IN1, HIGH); digitalWrite(PIN_IN2, LOW); 
    analogWrite(PIN_ENA, constrain(abs(speedL), 0, 255)); 
  }
  if (speedR >= 0) {
    digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, HIGH); 
    analogWrite(PIN_ENB, constrain(speedR, 0, 255)); 
  } else {
    digitalWrite(PIN_IN3, HIGH); digitalWrite(PIN_IN4, LOW); 
    analogWrite(PIN_ENB, constrain(abs(speedR), 0, 255)); 
  }
}

void stopMotors() {
  analogWrite(PIN_ENA, 255); analogWrite(PIN_ENB, 255);
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, LOW);
}