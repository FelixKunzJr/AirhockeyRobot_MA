#include <Arduino.h>
#include <MegaDevBoard.h>

#define solenoid 48
void interrupt();

void setup(){
pinMode(48,OUTPUT);
attachInterrupt(digitalPinToInterrupt(20), interrupt, LOW);

pinMode(LED1,OUTPUT);
pinMode(LED2,OUTPUT);
Serial.begin(9600);

}

void loop() {

digitalWrite(LED2,LOW);
digitalWrite(LED1, HIGH);
delay(1000);
digitalWrite(LED1, LOW);
delay(1000);

  }

void shoot(){
digitalWrite(solenoid, HIGH);
delay(500);
digitalWrite(solenoid, LOW);
Serial.println("shoot");

}

  void interrupt(){
    digitalWrite(LED2, HIGH);
    shoot();
  }
