/*

Ziel: Es wird nur ein vektor per CV-funktion erstellt.
Die genauigkeit dieses vektors ist noch nicht relevant.
Danach wäre es interessant eine gleichung zu lösen um den
y wert des schnittpunktes an der bande (x achse) zu ermitteln
und da den x wert des vektores zu invertieren.

*/

PVector v1;

import processing.video.*;

//Capture video;
Movie video;

color trackColor;
float threshold = 100;

float topBoundary;
float bottomBoundary;
float rightBoundary;
float leftBoundary;

float[] orig  = new float[2];
float[] avg5  = new float[2];
float[] avg  = new float[2];
float[] intersection = new float[2];
float[][] coordinates  = new float[2][2];


int count = 0;
int i = 0;

int locX;
int locY;


int setAccuracy;



void setup() {
  size(1284,720);

  video = new Movie(this, "bounceSmall.mov");

  //video.start();
    video.loop();

  trackColor = color(255, 0, 0);

  setAccuracy = 50;
  topBoundary= 27;
  bottomBoundary= 907;
  rightBoundary= 1191;
  leftBoundary= 93;

}

void movieEvent(Movie video) {
  video.read();
}

void draw() {
 // video.();
  image(video, 0, 0, width, height);



 getCoordinates();

if(avg[0]>0){
if(i < setAccuracy){
//getCoordinates();
if(i < 1){
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




ellipse(coordinates[0][0],coordinates[0][1],250,250);


v1 = new PVector(avg5[0]-orig[0],avg5[1]-orig[1]);
PVector v2 = PVector.mult(v1, 100);
line(orig[0],orig[1],orig[0]+v2.x,orig[1]+v2.y );
line(rightBoundary,bottomBoundary,rightBoundary,topBoundary);


intersection[0]= (((rightBoundary-orig[0])/(v1.x))*v1.y)+orig[1];
intersection[1]= rightBoundary;
println(intersection[0]);
println(intersection[1]);

ellipse(intersection[0], intersection[1], 24, 24);



    fill(255);
    strokeWeight(4.0);
    stroke(1);
    ellipse(avg[0], avg[1], 24, 24);
ellipse(intersection[0], intersection[1], 24, 24);

/*  print(avg[0]);
    print(" ");
    print(avg[1]);
    println(";");
*/




}





void getCoordinates(){

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
        stroke(0,0,255);
        strokeWeight(1);
        point(x, y);
        locX += x;
        locY += y;
        count++;
      }
    }
  }


if(count > 0){
    avg[0] = locX / count;
    avg[1] = locY / count;
  }

}

float distSq(float x1, float y1, float z1, float x2, float y2, float z2) {
  float d = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) +(z2-z1)*(z2-z1);
  return d;
}
