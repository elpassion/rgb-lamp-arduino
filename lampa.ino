#include <SoftwareSerial.h>
#define RED_PIN 2
#define GREEN_PIN 4
#define BLUE_PIN 12

#define RED_LED 6
#define GREEN_LED 5
#define BLUE_LED 3

#define BLUETOOTH_TX 7
#define BLUETOOTH_RX 8

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

//  Hue|Saturation|Value|
// [0.0-1.0]|[0.0-1.0]|[0.0-1.0]|
// 0.52|0.43|0.95

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

hsvColor lampColor = { 0.4, 1.0, 1.0 };

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.begin(9600);
  bluetooth.begin(9600);
  updateLamp(rgbFromHsv(lampColor));
}

void loop() {
  if (bluetooth.available() < 1) return;

  float hue = bluetooth.parseFloat();
  if (bluetooth.read() != SEP_CMD_CHAR) return;

  float saturation = bluetooth.parseFloat();
  if (bluetooth.read() != SEP_CMD_CHAR) return;

  float value = bluetooth.parseFloat();
  lampColor = { hue, saturation, value };

//  lampColor.hue += 0.05;
//  if (lampColor.hue >= 1.0) {
//    lampColor.hue = 0.0;
//  }
//  delay(1000);

  updateLamp(rgbFromHsv(lampColor));
}

void updateLamp(rgbColor rgb) {
  analogWrite(RED_PIN, (int)(rgb.red * 255));
  analogWrite(GREEN_PIN, (int)(rgb.green * 255));
  analogWrite(BLUE_PIN, (int)(rgb.blue * 255));

  analogWrite(RED_LED, (int)(rgb.red * 255));
  analogWrite(GREEN_LED, (int)(rgb.green * 255));
  analogWrite(BLUE_LED, (int)(rgb.blue * 255));  
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
