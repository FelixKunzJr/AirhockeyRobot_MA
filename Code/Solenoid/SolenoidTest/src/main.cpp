#include <Arduino.h>
#include <MegaDevBoard.h>

void setup(){
pinMode(48,OUTPUT);

}

void loop() {
//  if(digitalRead(uint8_t)){
digitalWrite(48, HIGH);
delay(2000);
digitalWrite(48, LOW);
delay(500);
//}
  }
