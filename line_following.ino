#include <LiquidCrystal.h> 

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); 

const int L_EN = 13; 
const int L_DIR = 12; 
const int R_EN = 11; 
const int R_DIR = 3; 

const int PIN_DIGITAL_LEFT = A5; 
const int PIN_ANALOG_RIGHT = A4; 

const int FORCE_TURN_TIME = 180; 
const int FWD_PULSE = 20; 
const int FWD_REST = 30;

unsigned long startTime = 0; 
unsigned long lastLCDUpdate = 0; 
const float SPEED_ESTIMATE = 25.0; 

int analogThreshold = 500; 

const int BLACK_STATE = HIGH; 

void setup() {
  pinMode(L_EN, OUTPUT); digitalWrite(L_EN, LOW); 
  pinMode(L_DIR, OUTPUT); digitalWrite(L_DIR, LOW);
  pinMode(R_EN, OUTPUT); digitalWrite(R_EN, LOW);
  pinMode(R_DIR, OUTPUT); digitalWrite(R_DIR, LOW);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(PIN_DIGITAL_LEFT, INPUT);
  
  analogThreshold = 500; 
  
  lcd.clear();
  lcd.print("Mode: V70 FINAL");
  delay(500); 
  
  lcd.clear();
  startTime = millis();
}

void loop() {
  bool leftVal = (digitalRead(PIN_DIGITAL_LEFT) == HIGH);
  bool rightVal = (analogRead(PIN_ANALOG_RIGHT) > analogThreshold);

  unsigned long currentTime = millis();
  float durationSeconds = (currentTime - startTime) / 1000.0;
  float distanceCm = durationSeconds * SPEED_ESTIMATE;

  if (currentTime - lastLCDUpdate > 250) {
    lcd.setCursor(0, 0);
    lcd.print("Run: "); lcd.print(durationSeconds, 1); lcd.print("s    ");
    lcd.setCursor(0, 1);
    lcd.print("Dst: "); lcd.print(distanceCm, 0); lcd.print("cm    ");
    lastLCDUpdate = currentTime;
  }

  if (digitalRead(PIN_DIGITAL_LEFT) == LOW && analogRead(PIN_ANALOG_RIGHT) < analogThreshold) {
    stopMotors(); 
    
    float finalTime = (millis() - startTime) / 1000.0;
    float finalDist = finalTime * SPEED_ESTIMATE;

    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("Time: "); lcd.print(finalTime, 1); lcd.print("s");
    
    lcd.setCursor(0, 1);
    lcd.print("Dist: "); lcd.print(finalDist, 1); lcd.print("cm");
    
    while(true) { 
      stopMotors(); 
    } 
  }
  else if (leftVal && !rightVal) {
    forceSpinLeft(); 
    delay(FORCE_TURN_TIME); 
    stopMotors(); delay(50); 
  }
  else if (!leftVal && rightVal) {
    forceSpinRight(); 
    delay(FORCE_TURN_TIME);
    stopMotors(); delay(50);
  }
  else {
    pulseForward();
  }
}

void pulseForward() {
  digitalWrite(L_EN, LOW); digitalWrite(L_DIR, HIGH);
  digitalWrite(R_EN, LOW); digitalWrite(R_DIR, HIGH);
  delay(FWD_PULSE); 
  
  digitalWrite(L_EN, LOW); digitalWrite(L_DIR, LOW);
  digitalWrite(R_EN, LOW); digitalWrite(R_DIR, LOW);
  delay(FWD_REST);
}

void forceSpinLeft() {
  digitalWrite(L_EN, HIGH); digitalWrite(L_DIR, LOW);
  digitalWrite(R_EN, LOW);  digitalWrite(R_DIR, HIGH);
}

void forceSpinRight() {
  digitalWrite(L_EN, LOW);  digitalWrite(L_DIR, HIGH);
  digitalWrite(R_EN, HIGH); digitalWrite(R_DIR, LOW);
}

void stopMotors() {
  digitalWrite(L_EN, LOW); digitalWrite(L_DIR, LOW);
  digitalWrite(R_EN, LOW); digitalWrite(R_DIR, LOW);
}