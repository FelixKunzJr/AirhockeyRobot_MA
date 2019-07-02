// libraries
import processing.serial.*;
// serial connection
Serial port;
 
 
void setup() {
  // init serial-port
  port = new Serial(this, Serial.list()[2], 57600);
}
 
void draw() {}
 
void mousePressed() {
  // create random number
  int number = (int) random(100);
  // debug
  println("now sending number: "+number);
  // send number
  port.write(Integer.toString(number));
  // write any charcter that marks the end of a number
  port.write('e');
}
 
// this part is executed, when serial-data is received
void serialEvent(Serial p) {
  try {
    // get message till line break (ASCII > 13)
    String message = p.readStringUntil(13);
    // just if there is data
    if (message != null) {
      println("message received: "+trim(message));
    }
  }
  catch (Exception e) {
  }
}
