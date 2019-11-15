#include "MegaDevBoard"

void setup(){
pinMode(48,OUTPUT);

}

void loop() {
  if(Button1){
digitalWrite(48, HIGH);
delay(100);
digitalWrite(48, LOW);
delay(100);
}
  }
