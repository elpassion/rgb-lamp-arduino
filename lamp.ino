/* 
Bluetooth data format:
Hue|Saturation|Value|
[0.0-1.0]|[0.0-1.0]|[0.0-1.0]|
0.52|0.43|0.95|
*/

#include <SoftwareSerial.h>
#include <Rotary.h>

#define LED_RED 9
#define LED_GREEN 10
#define LED_BLUE 11

#define HUE_ROTARY_RIGHT 6
#define HUE_ROTARY_LEFT 7
#define HUE_JUMP 0.05

#define SATURATION_ROTARY_RIGHT 2
#define SATURATION_ROTARY_LEFT 3
#define SATURATION_JUMP 0.05

#define VALUE_DECREASE_PIN 8
#define VALUE_INCREASE_PIN 4
#define VALUE_JUMP 0.05

#define INIT_DELAY_TIME 500
#define VALUE_DIFF_DELAY 100

Rotary hueRotary = Rotary(HUE_ROTARY_LEFT, HUE_ROTARY_RIGHT);
Rotary saturationRotary = Rotary(SATURATION_ROTARY_LEFT, SATURATION_ROTARY_RIGHT);

SoftwareSerial bluetooth(0, 1);

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

hsvColor lampColor = {0.32, 1.0, 1.0};

void setup() {
  startupSequence();
  
  bluetooth.begin(9600);
  hueRotary.begin(true);
  saturationRotary.begin(true);
  
  pinMode(VALUE_DECREASE_PIN, INPUT_PULLUP);
  pinMode(VALUE_INCREASE_PIN, INPUT_PULLUP);
}

void loop() {
  int valueIncrease = digitalRead(VALUE_INCREASE_PIN);
  int valueDecrease = digitalRead(VALUE_DECREASE_PIN);

  unsigned char hueResult = hueRotary.process();
  unsigned char saturationResult = saturationRotary.process();

  if (valueDecrease == LOW) {
    addLimitedDiff(&lampColor.value, -VALUE_JUMP);
    delay(VALUE_DIFF_DELAY);
  } else if (valueIncrease == LOW) {
    addLimitedDiff(&lampColor.value, VALUE_JUMP);
    delay(VALUE_DIFF_DELAY);
  } else if (hueResult) {
    if (hueResult == DIR_CW) {
      addRoundingDiff(&lampColor.hue, -HUE_JUMP);
    } else {
      addRoundingDiff(&lampColor.hue, HUE_JUMP);
    }
  } else if (saturationResult) {
    if (saturationResult == DIR_CW) {
      addLimitedDiff(&lampColor.saturation, -SATURATION_JUMP);
    } else {
      addLimitedDiff(&lampColor.saturation, SATURATION_JUMP);
    }
  } else if (bluetooth.available() > 1) {
    float hue = bluetooth.parseFloat();
    if (bluetooth.read() != SEP_CMD_CHAR)
      return;

    float saturation = bluetooth.parseFloat();
    if (bluetooth.read() != SEP_CMD_CHAR)
      return;

    float value = bluetooth.parseFloat();
    lampColor = {hue, saturation, value};
  } else {
    return;
  }
  
  updateLamp();
}

void startupSequence() {
  changeLampColor({1.0, 0.0, 0.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({0.0, 1.0, 0.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({0.0, 0.0, 1.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({0.0, 1.0, 1.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({1.0, 0.0, 1.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({1.0, 1.0, 0.0});
  delay(INIT_DELAY_TIME);

  changeLampColor({1.0, 1.0, 1.0});
  delay(INIT_DELAY_TIME);

  updateLamp();
}

void addRoundingDiff(float *value, float diff) {
  float newValue = *value + diff;
  
  if (newValue < 0) {
    newValue = 1.0;
  } else if (newValue > 1.0) {
    newValue = 0.0;
  }
  
  *value = newValue;
}

void addLimitedDiff(float *value, float diff) {
  float newValue = *value + diff;
  if (newValue < 0) {
    newValue = 0.0;
  } else if (newValue > 1.0) {
    newValue = 1.0;
  }
  
  *value = newValue;
}

void updateLamp() {
  changeLampColor(rgbFromHsv(lampColor));
}

void changeLampColor(rgbColor rgb) {
  analogWrite(LED_RED, (int)(rgb.red * 255));
  analogWrite(LED_GREEN, (int)(rgb.green * 255));
  analogWrite(LED_BLUE, (int)(rgb.blue * 255));
}

// https://gist.github.com/postspectacular/2a4a8db092011c6743a7
float fract(float x) { return x - int(x); }

float mix(float a, float b, float t) { return a + (b - a) * t; }

rgbColor rgbFromHsv(hsvColor hsv) {
  float red = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 1.0) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);
  float green = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 0.6666666) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);
  float blue = hsv.value * mix(1.0, constrain(fabs(fract(hsv.hue + 0.3333333) * 6.0 - 3.0) - 1.0, 0.0, 1.0), hsv.saturation);

  return {red, green, blue};
}