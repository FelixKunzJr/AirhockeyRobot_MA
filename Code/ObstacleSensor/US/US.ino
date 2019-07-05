//Didnt work for some reason???

const int trig = 46;
const int echo = 48;
long time, dist;
void setup(){
 Serial.begin(9600);
 pinMode(trig, OUTPUT);
 pinMode(echo, INPUT);
}
void loop(){
 digitalWrite(trig, LOW);
 delayMicroseconds(2);
 digitalWrite(trig, HIGH);
 delayMicroseconds(10);
 digitalWrite(trig, LOW);

 time = pulseIn(echo, HIGH);
 dist = (time/2) / 29.1;

 if(dist>500 or dist==0) Serial.println("Out of Range");
 else{
 Serial.print(dist);
 Serial.println(" cm");
 }

}
