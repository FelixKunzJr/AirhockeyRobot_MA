import processing.serial.*;

Serial port;
String portname = "COM6";
int baudrate = 9600;
int value = 0;
String val;

void setup(){

port = new Serial(this, portname, baudrate);
println(port);

}


void draw(){
  if ( port.available() > 0)
  {  // If data is available,
  val = port.readStringUntil('\n');         // read it and store it in val
  }
println(val); //print it out in the console



  if(keyPressed) {
   if (key == '0') {
     port.write (0);
   }
   if (key == '1') {
     port.write (1);
   }
   if (key == '2') {
     port.write (2);
   }
   if (key == '3') {
     port.write (3);
   }
   if (key == '4') {
     port.write (4);
   }
   if (key == '5') {
     port.write (5);
   }



 }



}
