import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.video.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class OneVector extends PApplet {

/*

Ziel: Es wird nur ein vektor per CV-funktion erstellt.
Die genauigkeit dieses vektors ist noch nicht relevant.
Danach wäre es interessant eine gleichung zu lösen um den
y wert des schnittpunktes an der bande (x achse) zu ermitteln
und da den x wert des vektores zu invertieren.

*/

PVector v1;



//Capture video;
Movie video;

int trackColor;
float threshold = 100;

float vectX;
float vectY;

float prevX;
float prevY;

float avgX = 0;
float avgY = 0;

int count2 = 0;

int locX;
int locY;

public void setup() {
  

  video = new Movie(this, "animation.mov");

  //video.start();
    video.loop();

  trackColor = color(255, 0, 0);
}

public void movieEvent(Movie video) {
  video.read();
}

public void draw() {
 // video.loadPixels();
  image(video, 0, 0, width, height);
 //threshold = map(mouseX, 0, width, 0, 100);

getCoordinates();
print(avgX);
print(avgY);




  // Begin loop to walk through every pixel



    fill(255);
    strokeWeight(4.0f);
    stroke(1);
    ellipse(avgX, avgY, 24, 24);

  /*  ellipse(prevX, prevY, 24, 24);
   v1 = new PVector(avgX-prevX, avgY-prevY);
    PVector v2 = PVector.mult(v1, 100);
    line(avgX,avgY,avgX+v2.x,avgY+v2.y );

    prevX = avgX;
    prevY = avgY;

    vectX =vectX+ avgX;
      vectY = vectY+avgY;
      count2++;
*/
}

public float distSq(float x1, float y1, float z1, float x2, float y2, float z2) {
  float d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1);
  return d;
}

public void mousePressed() {
  // Save color where the mouse is clicked in trackColor variable
  int loc = mouseX + mouseY*video.width;
  trackColor = video.pixels[loc];
  print(trackColor);
}

public void getCoordinates(){

  int count = 0;


  for (int x = 0; x < video.width; x++ ) {
    for (int y = 0; y < video.height; y++ ) {
      int loc = x + y * video.width;
      // What is current color
      int locX = 0;
      int locY = 0;


      int currentColor = video.pixels[loc];
      float r1 = red(currentColor);
      float g1 = green(currentColor);
      float b1 = blue(currentColor);
      float r2 = red(trackColor);
      float g2 = green(trackColor);
      float b2 = blue(trackColor);

      float d = distSq(r1, g1, b1, r2, g2, b2);

      if (d < threshold*threshold) {
        stroke(0,0,255);
        strokeWeight(1);
        point(x, y);
        locX += x;
        locY += y;
        count++;
      }
    }
  }

  // We only consider the color found if its color distance is less than 10.
  // This threshold of 10 is arbitrary and you can adjust this number depending on how accurate you require the tracking to be.

    avgX = locX / count;
    avgY = locY / count;
    // Draw a circle at the tracked pixel






}
  public void settings() {  size(1850,1036); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "OneVector" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
