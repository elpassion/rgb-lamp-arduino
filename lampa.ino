#include <SoftwareSerial.h>
#define RED_PIN 10
#define GREEN_PIN 11
#define BLUE_PIN 12

SoftwareSerial bluetooth(7, 8); // RX, TX  

//  H | S | V
// [0-359]|[0.0-1.0]|[0.0-1.0]
// 43|0.43|0.23

char START_CMD_CHAR = '^';
char END_CMD_CHAR = '$';
char SEP_CMD_CHAR = '|';

struct hsvColor {
  float hue;
  float saturation;
  float value;
};

struct rgbColor {
  float red;
  float green;
  float blue;
};

hsvColor lampColor = { 0.2, 1.0, 1.0 };

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  Serial.begin(9600);
  bluetooth.begin(9600);  
}

void loop() {
//  if (bluetooth.available() < 1) return;

  lampColor.hue += 0.01;
  if (lampColor.hue >= 1.0) {
    lampColor.hue = 0.0;
  }

  updateLamp(rgbFromHsv(lampColor));
  delay(100);
}

void updateLamp(rgbColor rgb) {
  analogWrite(RED_PIN, (int)((1.0 - rgb.red) * 255));
  analogWrite(GREEN_PIN, (int)((1.0 - rgb.green) * 255));
  analogWrite(BLUE_PIN, (int)((1.0 - rgb.blue) * 255));  
}

// https://gist.github.com/postspectacular/2a4a8db092011c6743a7
float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

rgbColor rgbFromHsv(hsvColor hsv) {
  float red = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);
  float green = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);
  float blue = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);

  return { red, green, blue };
}
