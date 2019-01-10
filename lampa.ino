#include <SoftwareSerial.h>

#define RED_PIN 10
#define GREEN_PIN 11
#define BLUE_PIN 12

SoftwareSerial bluetooth(7, 8); // RX, TX  

//  H | S | V
// 43|21|32

char START_CMD_CHAR = '^';
char END_CMD_CHAR = '$';
char SEP_CMD_CHAR = '|';

float color[3];

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  bluetooth.begin(9600);

  delay(500);
  setRed();
  delay(500);
  setGreen();
  delay(500);
  setBlue();
  delay(500);
//  setWhite();
}

void loop() {
  if (bluetooth.available() < 1) return;
  updateLed(bluetooth.read());
//  setColor(hsv2rgb(hue, 1.0, 1.0, col));
}

void setColor(float *rgb) {
  analogWrite(RED_PIN, (int)((1.0 - rgb[0]) * 255));
  analogWrite(GREEN_PIN, (int)((1.0 - rgb[1]) * 255));
  analogWrite(BLUE_PIN, (int)((1.0 - rgb[2]) * 255));  
}

void updateLed(char input) {
  switch(input) {
    case 'r':
      setRed();
      break;
    case 'g':
      setGreen();
      break;
    case 'b':
      setBlue();
      break;
    default:
      setWhite();
      break;
  }
}

// https://gist.github.com/postspectacular/2a4a8db092011c6743a7
float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

float step(float e, float x) { return x < e ? 0.0 : 1.0; }

float* hsv2rgb(float h, float s, float b, float* rgb) {
  rgb[0] = b * mix(1.0, constrain(abs(fract(h + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[1] = b * mix(1.0, constrain(abs(fract(h + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  rgb[2] = b * mix(1.0, constrain(abs(fract(h + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), s);
  return rgb;
}

float* rgb2hsv(float r, float g, float b, float* hsv) {
  float s = step(b, g);
  float px = mix(b, g, s);
  float py = mix(g, b, s);
  float pz = mix(-1.0, 0.0, s);
  float pw = mix(0.6666666, -0.3333333, s);
  s = step(px, r);
  float qx = mix(px, r, s);
  float qz = mix(pw, pz, s);
  float qw = mix(r, px, s);
  float d = qx - min(qw, py);
  hsv[0] = abs(qz + (qw - py) / (6.0 * d + 1e-10));
  hsv[1] = d / (qx + 1e-10);
  hsv[2] = qx;
  return hsv;
}

// View

void setWhite() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);
}     

void setRed() { 
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void setGreen() { 
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
}

void setBlue() { 
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
}
