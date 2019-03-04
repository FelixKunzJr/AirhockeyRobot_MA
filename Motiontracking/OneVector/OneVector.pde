/*

Ziel: Es wird nur ein vektor per CV-funktion erstellt.
Die genauigkeit dieses vektors ist noch nicht relevant. 
Danach wäre es interessant eine gleichung zu lösen um den 
y wert des schnittpunktes an der bande (x achse) zu ermitteln 
und da den x wert des vektores zu invertieren.

*/


Movie video;
int ColFrameCount;
void setup(){
size(18500, 1036);
video = new Movie(this, "animation.mov");
video.loop();

}

void movieEvent(Movie video) {
  video.read();
}

void draw(){
image(video, 0, 0, width, height);

if(ColFrameCount < 2){

}

}
