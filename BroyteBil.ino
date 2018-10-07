
const int currentSensor = A0;
const int actuatorDirectionPin1 = 2;
const int actuatorDirectionPin2 = 3;
const int receiverReversePin = 4;
const int receiverThrottlePin = 5;
const int receiverSteeringPin = 6;
const int actuatorPwmPin=9;
const int motorPwmPinLeft = 10;
const int motorPwmPinRight = 11;
const int directionPin1 = 12;
const int directionPin2 = 13;
const int directionPin3 = 7;
const int directionPin4 = 8;
const long pulseInDelayMicros = 50000;
int steeringCenterPosition = 0;
int throttleMin = 0;
//const int throttleMax = 1800;
const int switchPosHigh = 1600;
const int switchPosLow = 1200;
int throttlePosition = 0;
int steeringPosition = 0;
int motorSpeedLeft = 0;
int motorSpeedRight = 0;
int turnLeftBias = 0;
int turnRightBias = 0;
int switchValue = 1000;
int everyTenthCounter = 11;
bool actuatorExtended = LOW;
bool actuatorRetracted = LOW;
int actuatorExtendTimer=0;
int actuatorRetractTimer=0;
int currentThreshold=200;

void leftForward() {
  digitalWrite(directionPin3, HIGH);
  digitalWrite(directionPin4, LOW);
}
void leftReverse() {
  digitalWrite(directionPin3, LOW);
  digitalWrite(directionPin4, HIGH);
}
void rightForward() {
  digitalWrite(directionPin1, HIGH);
  digitalWrite(directionPin2, LOW);
}
void rightReverse() {
  digitalWrite(directionPin1, LOW);
  digitalWrite(directionPin2, HIGH);
}
void extendingActuator() {
  digitalWrite(actuatorDirectionPin1, HIGH);
  digitalWrite(actuatorDirectionPin2, LOW);
  analogWrite(actuatorPwmPin,200);
}
void retractingActuator() {
  digitalWrite(actuatorDirectionPin1, LOW);
  digitalWrite(actuatorDirectionPin2, HIGH);
  analogWrite(actuatorPwmPin,200);
}
void stoppingActuator() {
  digitalWrite(actuatorDirectionPin1, LOW);
  digitalWrite(actuatorDirectionPin2, LOW);
  analogWrite(actuatorPwmPin,0);
}
bool isRotatingLeft() {
  return throttlePosition == 0 && steeringPosition < steeringCenterPosition - 4;
}
bool isRotatingRight() {
  return throttlePosition == 0 && steeringPosition > steeringCenterPosition + 4;
}
bool isTurningLeft() {
  return steeringPosition < steeringCenterPosition - 2;
}
bool isTurningRight() {
  return steeringPosition > steeringCenterPosition + 2;
}
bool isReversing() {
  return switchValue > switchPosHigh;
}
bool isActuatorMode() {
  return switchValue < switchPosHigh && switchValue > switchPosLow;
}
void setup() {
  pinMode(currentSensor,INPUT);
  pinMode(receiverReversePin,INPUT);
  pinMode(receiverThrottlePin, INPUT);
  pinMode(receiverSteeringPin, INPUT);
  pinMode(motorPwmPinLeft, OUTPUT);
  pinMode(motorPwmPinRight, OUTPUT);
  pinMode(actuatorPwmPin,OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
  pinMode(directionPin3, OUTPUT);
  pinMode(directionPin4, OUTPUT);
  pinMode(actuatorDirectionPin1, OUTPUT);
  pinMode(actuatorDirectionPin2, OUTPUT);
  Serial.begin(9600);
  analogWrite(motorPwmPinLeft, 0);
  analogWrite(motorPwmPinRight, 0);
  analogWrite(actuatorPwmPin,0);
  delay(1000);
  while ((int)pulseIn(receiverThrottlePin, HIGH, pulseInDelayMicros) < 500) {
    delay(100);
  }
  for (int i = 0; i < 4; i++) {
    throttleMin += (int)pulseIn(receiverThrottlePin, HIGH, pulseInDelayMicros);
    steeringCenterPosition += (int)pulseIn(receiverSteeringPin, HIGH, pulseInDelayMicros);
    delay(100);
  }
  throttleMin = throttleMin / 4 + 5;
  steeringCenterPosition = (((steeringCenterPosition / 4) - throttleMin) / 3);
}
//----------------------------------------------------------------------------------------------------------------------
void loop() {
  throttlePosition = constrain((((int)pulseIn(receiverThrottlePin, HIGH, pulseInDelayMicros) - throttleMin) / 3), 0, 253);
  steeringPosition = constrain((((int)pulseIn(receiverSteeringPin, HIGH, pulseInDelayMicros) - throttleMin) / 3), 0, 253);
  
  if (everyTenthCounter > 8) { //Sjekk reversbryter hver 10. iterasjon.
    switchValue = (int)pulseIn(receiverReversePin, HIGH, pulseInDelayMicros);  
    everyTenthCounter = 0;
  }
  else everyTenthCounter++;
  
  if (isTurningLeft()) {
    turnLeftBias = constrain((steeringCenterPosition - steeringPosition), 0, 120);
    turnRightBias = -constrain((steeringCenterPosition - steeringPosition), 0, 120) / 2;
  }
  else if (isTurningRight()) {
    turnRightBias = constrain((steeringPosition - steeringCenterPosition), 0, 120);
    turnLeftBias = -constrain((steeringPosition - steeringCenterPosition), 0, 120) / 2;
  }
  else turnLeftBias = turnRightBias = 0;
  
  if (!isActuatorMode()) {
    if (isRotatingLeft()) {
      rightForward();
      leftReverse();
      analogWrite(motorPwmPinLeft, turnLeftBias);
      analogWrite(motorPwmPinRight, turnLeftBias);
    }
    else if (isRotatingRight()) {
      rightReverse();
      leftForward();
      analogWrite(motorPwmPinLeft, turnRightBias);
      analogWrite(motorPwmPinRight, turnRightBias);
    }
    else if (isReversing()) {
      rightReverse();
      leftReverse();
      analogWrite(motorPwmPinRight, constrain(throttlePosition - turnLeftBias * 2, 0, 253));
      analogWrite(motorPwmPinLeft, constrain(throttlePosition - turnRightBias * 2, 0, 253));
    }
    else { //Forward
      leftForward();
      rightForward();
      analogWrite(motorPwmPinLeft, constrain(throttlePosition - turnLeftBias * 2, 0, 253));
      analogWrite(motorPwmPinRight, constrain(throttlePosition - turnRightBias * 2, 0, 253));
    }
  }
  else {
    //actuator-kode
    analogWrite(motorPwmPinLeft, 0);
    analogWrite(motorPwmPinRight, 0);
    
    if (isTurningRight()&&!actuatorExtended){
      if(actuatorRetractTimer==0){
        actuatorExtendTimer=100;
        actuatorRetracted=LOW;
      }
      else{ 
        actuatorRetractTimer=0;
        stoppingActuator();
        delay(500);
      }
    }
    else if (isTurningLeft()&&!actuatorRetracted) {
      if(actuatorExtendTimer==0){
        actuatorRetractTimer=100;
        actuatorExtended=LOW;
      }
      else {      
        actuatorExtendTimer=0;
        stoppingActuator();
        delay(500);
      }
    }
  }
  if(analogRead(currentSensor)>currentThreshold){
    if(actuatorExtendTimer>0) actuatorExtended=HIGH;
    if(actuatorRetractTimer>0) actuatorRetracted=HIGH;
    actuatorExtendTimer=0;
    actuatorRetractTimer=0;
  }
  if(actuatorExtendTimer>0&&!actuatorExtended){
    extendingActuator();
    actuatorExtendTimer--;
  }
  else if(actuatorRetractTimer>0&&!actuatorRetracted){
    retractingActuator();
    actuatorRetractTimer--;
  }
  else stoppingActuator();
}
