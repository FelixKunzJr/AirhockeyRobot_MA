#include <MegaDevBoard.h>


#include <LiquidCrystal.h>

LiquidCrystal lcd(LCDRs, LCDEn, LCD4, LCD5, LCD6, LCD7);

int led;

void setup() {
  Serial.begin(9600);
  LCDinit();
  lcd.begin(16,2);
  //pinMode(1, INPUT);
  lcd.print("011001ENTER10010");

}

void loop() {
    lcd.setCursor(0,2);
lcd.print(analogRead(1));

 }   
  
