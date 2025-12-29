#include <LiquidCrystal.h>
#include <SoftwareSerial.h> 

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int PIN_ENA = 3;    
const int PIN_IN1 = A1; 
const int PIN_IN2 = A2; 
const int PIN_ENB = 11;   
const int PIN_IN3 = 13;   
const int PIN_IN4 = 12; 

SoftwareSerial myBluetooth(2, 1); 

char command = 'S'; 
int speedNormal = 180; 
int speedTurn   = 200; 

void setup() {
  pinMode(PIN_IN3, OUTPUT); digitalWrite(PIN_IN3, LOW); 
  pinMode(PIN_ENA, OUTPUT); pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENB, OUTPUT); pinMode(PIN_IN4, OUTPUT);
  myBluetooth.begin(9600); 
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("BT Task IV");
  lcd.setCursor(0, 1);
  lcd.print("Pin 2&1 Ready");
}

void loop() {
  if (myBluetooth.available()) {
    command = myBluetooth.read(); 
    lcd.setCursor(0, 1);
    lcd.print("Cmd: ");
    lcd.print(command);
    lcd.print("    "); 
  }

  switch (command) {
    case 'F': 
      setMotorSpeed(speedNormal, speedNormal);
      break;
    case 'B': 
      setMotorSpeed(-speedNormal, -speedNormal);
      break;
    case 'L': 
      setMotorSpeed(-speedTurn, speedTurn);
      break;
    case 'R': 
      setMotorSpeed(speedTurn, -speedTurn);
      break;
    case 'S': 
      stopMotors();
      break;
    default: 
      stopMotors();
      break;
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