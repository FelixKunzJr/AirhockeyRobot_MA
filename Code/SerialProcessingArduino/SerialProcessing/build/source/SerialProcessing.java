import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class SerialProcessing extends PApplet {



Serial port;
String portname = "COM6";
int baudrate = 9600;
int value = 0;
String val;

public void setup(){

port = new Serial(this, portname, baudrate);
println(port);

}


public void draw(){
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
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "SerialProcessing" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
