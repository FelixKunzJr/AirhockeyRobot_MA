#include <MegaDevBoard.h>


#include <LiquidCrystal.h>

LiquidCrystal lcd(LCDRs, LCDEn, LCD4, LCD5, LCD6, LCD7);

int led;

void setup() {
  Serial2.begin(9600);
  LCDinit();
  lcd.begin(16,2);
  
  lcd.print("011001ENTER10010");

}

void loop() {
    lcd.setCursor(0,2);

if( Serial2.available() )       
 { 
 led =  Serial2.read(); 
 lcd.print(led);

 }   
  if(Button1){
    Serial2.print("Hello, World!");
    }
}
