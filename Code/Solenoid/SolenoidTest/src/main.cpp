#include <Arduino.h>
#include <Wire.h>
#include <MegaDevBoard.h>

void setup(){
pinMode(48,OUTPUT);

}

void loop() {
  if(Button1){
digitalWrite(48, HIGH);
delay(1000);
digitalWrite(48, LOW);
delay(1000);
}
  }
