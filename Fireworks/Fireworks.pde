import processing.serial.*;
import oscP5.*;
OscP5 oscP5;

Serial myPort;
long sensorValue;
int a = 60;

int found;
float poseScale;
PVector poseOrientation = new PVector(); // stores an (x,y,z)
PVector posePosition = new PVector();

float mouthHeight;

ArrayList<Firework> fireworks;
PImage back;
PVector gravity = new PVector(0, 0.3);

void setup() {
  myPort = new Serial(this, Serial.list()[5], 9600);
  //fullScreen(P2D);
  size(1000, 750, P2D);

  oscP5 = new OscP5(this, 8338);
  oscP5.plug(this, "poseOrientation", "/pose/orientation");
  oscP5.plug(this, "posePosition", "/pose/position");
  oscP5.plug(this, "mouthHeightReceived", "/gesture/mouth/height");

  back = loadImage("fsad.png");
  fireworks = new ArrayList<Firework>();
  colorMode(HSB, 360, 100, 100, 100);
  background(0);
  smooth();
  frameRate(30);
}

void draw() {
  //printArray(Serial.list());
  if (myPort.available()>0) {
    sensorValue = myPort.read();
    println(sensorValue);
  }

  noStroke();
  fill(0, 25);
  rect(0, 0, width, height);
  image(back, 0, 0, 1600, 900);


  for (int i = fireworks.size()-1; i >= 0; i--) {
    Firework f = fireworks.get(i);
    f.run();
    if (f.done()) {
      fireworks.remove(i);
    }
  }

  if (sensorValue>80) {
    tint(360, 100);
  } else if (sensorValue<=80&&sensorValue>60) {
    tint(360, 75);
    myPort.write('A');
  } else if (sensorValue<=60&&sensorValue>40) {
    tint(360, 50);
    myPort.write('B');
  } else if (sensorValue<=40&&sensorValue>30) {
    tint(360, 25);
    myPort.write('C');
  } else if (sensorValue<=30) {
    tint(360, 0);
    myPort.write('D');
  }

  if (mouthHeight>6) {
    fireworks.add(new Firework());
  }
}

public void found(int i) {
  //println("found: " + i);
  found = i;
}

public void poseScale(float s) {
  //println("scale: " + s);
  poseScale = s;
}

public void posePosition(float x, float y) {
  //println("pose position\tX: " + x + " Y: " + y );
  posePosition.set(x, y, 0);
}

public void poseOrientation(float x, float y, float z) {
  //println("pose orientation\tX: " + x + " Y: " + y + " Z: " + z);
  poseOrientation.set(x, y, z);
}

public void mouthHeightReceived(float h) {
  //println("mouth height: " + h);
  mouthHeight = h;
}

void mousePressed() {
  fireworks.add(new Firework(mouseX));
}
