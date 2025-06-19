#include <Arduino.h>
#include <ESP32Servo.h>

#define SPEED_OF_SOUND 0.034

Servo left_servo;
Servo right_servo;

const int LEFT_SERVO_PIN = 5;
const int RIGHT_SERVO_PIN = 17;
const int TRIGGER_PIN = 14;
const int ECHO_PIN = 12;
const int RED_LED_PIN = 2;
const int ORANGE_LED_PIN = 3;
const int GREEN_LED_PIN = 16;

long duration;
float distance;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(ORANGE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  left_servo.attach(LEFT_SERVO_PIN);
  right_servo.attach(RIGHT_SERVO_PIN);
  left_servo.write(0);
  right_servo.write(0);
}

void loop()
{
  // put your main code here, to run repeatedly:

  digitalWrite(TRIGGER_PIN, LOW);
  delay(200);

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  distance = duration * SPEED_OF_SOUND / 2;

  if (distance <= 15)
  {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(ORANGE_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(100);
    left_servo.write(90);
    right_servo.write(90);
  }
  else if (distance <= 25 && distance > 15)
  {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(ORANGE_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);

    delay(100);
    left_servo.write(45);
    right_servo.write(45);
  }
  else
  {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(ORANGE_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(100);
    left_servo.write(0);
    right_servo.write(0);
  }
  delay(100);
}