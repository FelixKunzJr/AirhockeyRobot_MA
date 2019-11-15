#define IR 20
int detection = HIGH;    // no obstacle

void setup() {
  Serial.begin(9600);
  pinMode(IR, INPUT);
  attachInterrupt(digitalPinToInterrupt(20), interrupt, LOW);
}

void loop() {
  detection = digitalRead(IR);
  if(detection == LOW){
    //Serial.print("There is an obstacle!\n");
  }
  else{
    //Serial.print("No obstacle!\n");
  }
  delay(10);    // in ms
}

void interrupt(){

Serial.println("Interrupt");

}
