/*

 Ziel: Es wird der Punkt ermittelt, an welchem man den Ball abfangen will. der Einfachheit halber wird dieser Abfang punkt auf einer Linie Y sein.
 */

PVector v1, v2, v3, v4;

import processing.video.*;
import java.util.*;

//Capture video;
Movie video;

color trackColor;
float threshold = 50;
int setAccuracy = 20;

float topBoundary;
float bottomBoundary;
float rightBoundary;
float leftBoundary;

Date d;


float interceptionLine;

float[] orig  = new float[2];
float[] avg5  = new float[2];
float[] avg  = new float[2];
float[] reflection = new float[2];
float[][] coordinates  = new float[2][2];
float[] interceptionPoint = new float[2];


int count = 0;
int i = 0;

int locX;
int locY;






void setup() {
  size(1284, 720);

  video = new Movie(this, "bounceSmall.mov");

  //video.start();
  video.loop();

  trackColor = color(255, 0, 0);


  topBoundary= 27;
  bottomBoundary= 907;
  rightBoundary= 1191;
  leftBoundary= 93;

  interceptionLine= 600;

  int m = millis();
  println(m);
}

void movieEvent(Movie video) {
  video.read();
}

void draw() {
  // video.();
  image(video, 0, 0, width, height);





  getCoordinates();
  getInterceptionPoint();




  /*  print(avg[0]);
   print(" ");
   print(avg[1]);
   println(";");
   */
}



void getCoordinates() {

  count=0;
  locX =0;
  locY =0;


  for (int x = 0; x < video.width; x++ ) {
    for (int y = 0; y < video.height; y++ ) {
      int loc = x + y * video.width;
      // What is current color



      color currentColor = video.pixels[loc];
      float r1 = red(currentColor);
      float g1 = green(currentColor);
      float b1 = blue(currentColor);
      float r2 = red(trackColor);
      float g2 = green(trackColor);
      float b2 = blue(trackColor);

      float d = distSq(r1, g1, b1, r2, g2, b2);

      if (d < threshold*threshold) {
        stroke(0, 0, 255);
        strokeWeight(1);
        point(x, y);
        locX += x;
        locY += y;
        count++;
      }
    }
  }


  if (count > 0) {
    avg[0] = locX / count;
    avg[1] = locY / count;
  }
}

void getInterceptionPoint() {
  if (avg[0]>0) {
    if (i < setAccuracy) {
      //getCoordinates();
      if (i < 1) {
        orig[0]=  avg[0];
        orig[1]=  avg[1];
      }


      coordinates[0][0]= coordinates[0][0]+ avg[0];
      coordinates[0][1]= coordinates[0][1]+ avg[1];


      i++;

      avg5[0] = coordinates[0][0]/i;
      avg5[1]= coordinates[0][1]/i;
    }
  }




  ellipse(coordinates[0][0], coordinates[0][1], 250, 250);


  v1 = new PVector(avg5[0]-orig[0], avg5[1]-orig[1]);
  PVector v2 = PVector.mult(v1, 100);
  line(orig[0], orig[1], orig[0]+v2.x, orig[1]+v2.y );
  line(rightBoundary, bottomBoundary, rightBoundary, topBoundary);
  line(leftBoundary, interceptionLine, rightBoundary, interceptionLine);


  reflection[0]= rightBoundary;
  reflection[1]= (((rightBoundary-orig[0])/(v1.x))*v1.y)+orig[1];



  ellipse(reflection[0], reflection[1], 24, 24);



  fill(255);
  strokeWeight(4.0);
  stroke(1);
  ellipse(avg[0], avg[1], 24, 24);
  ellipse(reflection[0], reflection[1], 24, 24);

  v3 = new PVector((-1*(v1.x)), v1.y);
  println(v3);
  PVector v4 = PVector.mult(v3, 100);
  line(reflection[0], reflection[1], reflection[0]+v4.x, reflection[1]+v4.y );

  interceptionPoint[0] = (((interceptionLine-reflection[1])/(v3.y))*v3.x)+reflection[0];
  interceptionPoint[1] = interceptionLine;

  println(interceptionPoint[0]);
  println(interceptionPoint[1]);

  /*
  println(reflection[0]);
   println(reflection[1]);
   */
  line(interceptionPoint[0], interceptionPoint[1]+10, interceptionPoint[0], interceptionPoint[1]-10);
}

float distSq(float x1, float y1, float z1, float x2, float y2, float z2) {
  float d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1);
  return d;
}
