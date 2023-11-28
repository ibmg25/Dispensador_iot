#include <ESP32Servo.h>

const int triggerPin1 = 16, echoPin1 = 17;
const int triggerPin2 = 18, echoPin2 = 19;
const int servoPin = 25;
int cm1, cm2;

Servo myServo;

long readUltrasonicDistance(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(triggerPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(triggerPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(servoPin, OUTPUT);
  //myServo.attach(servoPin);
}

void loop() {
  cm1 = 0.01723 * readUltrasonicDistance(triggerPin1, echoPin1);
  cm2 = 0.01723 * readUltrasonicDistance(triggerPin2, echoPin2);
  Serial.println(cm1);
  Serial.println(cm2);
  int vaso = 10, liq = 10;
  if (cm1 < vaso && cm2 < liq) {
    digitalWrite(servoPin, HIGH);
    //myServo.write(90);
    delay(500);
  } else {
    digitalWrite(servoPin, LOW);
    //myServo.write(0);  
    delay(500);
  }
  delay(1000);
}
