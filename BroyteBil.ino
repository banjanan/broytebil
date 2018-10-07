const int receiverVenstrePin = 5;
const int receiverHoyrePin=6;
const int receiverReversePin=3;
const int motorPwmPinLeft=10;
const int motorPwmPinRight=11;
const int directionPin1 = 12;
const int directionPin2 = 13;

const long pulseInDelayMicros=250000;
const int throttleMin=1100;
const int throttleMax=1800;
const int switchOn = 1600;
const int minMotorStart=10;
boolean directionStateForward=HIGH;
int motorSpeedLeft=0;
int motorSpeedRight=0;
int counter1=0;

void setup() {
  pinMode(receiverVenstrePin, INPUT);
  pinMode(receiverHoyrePin, INPUT);
  pinMode(motorPwmPinLeft, OUTPUT);
  pinMode(motorPwmPinRight, OUTPUT);
  pinMode(directionPin1, OUTPUT);
  pinMode(directionPin2, OUTPUT);
}

void loop() {
  motorSpeedLeft=constrain((((int)pulseIn(receiverVenstrePin,HIGH,pulseInDelayMicros)-throttleMin)/3), 0, 253);
  motorSpeedRight=constrain((((int)pulseIn(receiverHoyrePin,HIGH,pulseInDelayMicros)-throttleMin)/3),0,253);
  
  if(counter1>10){
    if((int)pulseIn(receiverReversePin,HIGH,pulseInDelayMicros)>switchOn) {directionStateForward=HIGH;}
    else {directionStateForward=LOW;}
    counter1=0;
    }
  counter1++;

  analogWrite(motorPwmPinLeft,motorSpeedLeft);
  analogWrite(motorPwmPinRight,motorSpeedRight);
  
  digitalWrite(directionPin1,directionStateForward);
  digitalWrite(directionPin2,!directionStateForward);
}
