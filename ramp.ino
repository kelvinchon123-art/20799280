#include <MPU6050_tockn.h>
#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
MPU6050 mpu6050(Wire);

const int PIN_ENA = 3;    
const int PIN_IN1 = A1; 
const int PIN_IN2 = A2; 
const int PIN_ENB = 11;   
const int PIN_IN3 = 13;   
const int PIN_IN4 = 12; 

const int SPEED_CLIMB = 160;     
const int SPEED_TURN  = 200;     
const int TURN_TIME   = 2500;    
const float RAMP_THRESHOLD = 8.0; 

double baseOffset = 0;           
double maxAngle = 0;             
unsigned long climbStartTime = 0;
const int MIN_CLIMB_TIME = 2000; 

bool hasClimbed = false; 
bool taskDone = false;    

void setup() {
  pinMode(PIN_IN3, OUTPUT); 
  pinMode(PIN_ENA, OUTPUT); pinMode(PIN_IN1, OUTPUT); pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENB, OUTPUT); pinMode(PIN_IN4, OUTPUT);
   
  lcd.begin(16, 2);
  lcd.print("Calibrating..."); 

  Wire.begin();
  mpu6050.begin();
   
  double sumAngle = 0;
  for(int i=0; i<50; i++) {
    mpu6050.update();
    double angle = atan2(mpu6050.getAccY(), mpu6050.getAccZ()) * 57.29; 
    sumAngle += angle;
    delay(10);
  }
  baseOffset = sumAngle / 50.0; 
   
  lcd.clear();
  lcd.print("Flat is: ");
  lcd.print((int)baseOffset); 
  delay(1500); 
   
  lcd.clear();
  lcd.print("Ready! GO!");
}

void loop() {
  mpu6050.update();

  if (taskDone) {
    goForward(150); 
    return; 
  }

  double rawAngle = atan2(mpu6050.getAccY(), mpu6050.getAccZ()) * 57.29;
  double currentAngle = abs(rawAngle - baseOffset);

  if (currentAngle > 5.0 && currentAngle > maxAngle) {
      maxAngle = currentAngle;
  }

  lcd.setCursor(0, 0);
  lcd.print("Ramp: ");
  lcd.print((int)maxAngle); 
  lcd.print(" Now:");
  lcd.print((int)currentAngle); 

  if (currentAngle > RAMP_THRESHOLD) {
    if (!hasClimbed) {
      climbStartTime = millis(); 
      hasClimbed = true;         
      maxAngle = currentAngle;   
    }
    lcd.setCursor(0, 1);
    lcd.print("Climbing...     ");
    goForward(SPEED_CLIMB);
  } 
  else {
    if (!hasClimbed) {
      lcd.setCursor(0, 1);
      lcd.print("Flat Run...     ");
      goForward(180);
    }
    else {
      unsigned long timePassed = millis() - climbStartTime;
      if (timePassed < MIN_CLIMB_TIME) {
         lcd.setCursor(0, 1);
         lcd.print("FORCE CLIMB!    "); 
         goForward(SPEED_CLIMB);
      } 
      else {
         stopMotors();
         lcd.setCursor(0, 1);
         lcd.print("Done! Angle:");
         lcd.print((int)maxAngle); 
         delay(4000); 
         lcd.setCursor(0, 1);
         lcd.print("Spinning...     ");
         spinCircle(); 
         taskDone = true; 
         goForward(150); 
      }
    }
  }
}

void goForward(int speed) {
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, HIGH); 
  analogWrite(PIN_ENA, speed); 
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, HIGH); 
  analogWrite(PIN_ENB, speed); 
}

void stopMotors() {
  analogWrite(PIN_ENA, 0); analogWrite(PIN_ENB, 0);
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW); digitalWrite(PIN_IN4, LOW);
}

void spinCircle() {
  digitalWrite(PIN_IN1, LOW); digitalWrite(PIN_IN2, HIGH); 
  analogWrite(PIN_ENA, SPEED_TURN); 
  digitalWrite(PIN_IN3, HIGH); digitalWrite(PIN_IN4, LOW); 
  analogWrite(PIN_ENB, SPEED_TURN); 
  delay(TURN_TIME); 
  stopMotors();
}