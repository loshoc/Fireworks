#include <Adafruit_NeoPixel.h>
#define trigger1 7
#define echo1 6
#define PIN 2
#define NUMPIXELS 8

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int motorPin = 3;
char val;
int inRange = 45;
int TargetRange = 12;
const int NoiseReject = 25;
long duration, distance, lastDuration, unfiltered, Sonar, RawSonar;
const unsigned int maxDuration = 11650 / 2; // around 200 cm, the sensor gets flaky at greater distances.
const long speed_of_sound = 29.1;    // speed of sound microseconds per centimeter

//void rainbowCycle(uint8_t wait);
void theaterChaseRainbow(uint8_t wait);

void setup() {
  Serial.begin(9600);
  //Sensor Connections
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(motorPin, OUTPUT);


  pixels.begin(); // This initializes the NeoPixel library.
  wipe();
  //pixels.show();
}

void loop() {
  SingleSonar();

  Serial.write(Sonar);
  //Serial.println(Sonar);

  if (Serial.available())
  { val = Serial.read();
    if (val == 'A') {
      analogWrite(motorPin, 50);
    } else if (val == 'B') {
      analogWrite(motorPin, 60);
    } else if (val == 'C') {
      analogWrite(motorPin, 70);
    } else if (val == 'D') {
      analogWrite(motorPin, 80);

    }
  }
  wipe();
  delay(50);
  theaterChaseRainbow(100);
  //rainbowCycle(20);
  //theaterChaseRainbow(500);


}

//void rainbowCycle(uint8_t wait) {
//  uint16_t i, j;
//
//  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//    for(i=0; i< pixels.numPixels(); i++) {
//      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
//    }
//    pixels.show();
//    delay(wait);
//  }
//}

//void theaterChaseRainbow(uint8_t wait) {
//  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
//    for (int q = 0; q < 3; q++) {
//      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
//        pixels.setPixelColor(i + q, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));  //turn every third pixel on
//      }
//      pixels.show();
//
//      delay(wait);
//
//      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
//        pixels.setPixelColor(i + q, 0);      //turn every third pixel off
//      }
//    }
//  }
//}

void theaterChaseRainbow(uint8_t) { // modified from Adafruit example to make it a state machine
  static int j = 0, q = 0;
  static boolean on = true;
  if (on) {
    for (int i = 0; i < pixels.numPixels(); i = i + 3) {
      pixels.setPixelColor(i + q, Wheel(((i * 256 / pixels.numPixels()) + j) & 255)); //turn every third pixel on
    }
  }
  else {
    for (int i = 0; i < pixels.numPixels(); i = i + 3) {
      pixels.setPixelColor(i + q, 0);      //turn every third pixel off
    }
  }
  on = !on; // toggel pixelse on or off for next time
  pixels.show(); // display
  q++; // update the q variable
  if (q >= 3 ) { // if it overflows reset it and update the J variable
    q = 0;
    j++;
    if (j >= 256) j = 0;
  }
  //lastUpdate = millis(); // time for next change to the display
}

void wipe() { // clear all LEDs
  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void SonarSensor(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  unfiltered = (duration / 2) / speed_of_sound;
  if (duration <= 8) duration = ((inRange + 1) * speed_of_sound * 2);
  if (lastDuration == 0) lastDuration = duration;
  if (duration > (5 * maxDuration)) duration = lastDuration;
  if (duration > maxDuration) duration = maxDuration;
  if ((duration - lastDuration) < ((-1) * (NoiseReject / 100) * lastDuration)) {
    distance = (lastDuration / 2) / speed_of_sound; //Noise filter for low range drops
  }
  distance = (duration / 2) / speed_of_sound;
  lastDuration = duration; //Stores "successful" reading for filter compensation
}
void SingleSonar() {
  SonarSensor(trigger1, echo1);
  Sonar = distance;
  RawSonar = unfiltered;
  delay(500); //Delay 50ms before next reading.
}
