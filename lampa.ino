#include <SoftwareSerial.h>
#include <Rotary.h>

#define RED_LED 6
#define GREEN_LED 5
#define BLUE_LED 3

#define BLUETOOTH_TX 8
#define BLUETOOTH_RX 7

#define HUE_ROTARY_LEFT 12
#define HUE_ROTARY_RIGHT 13

Rotary hueRotary = Rotary(HUE_ROTARY_LEFT, HUE_ROTARY_RIGHT);
#define HUE_JUMP 0.05

#define ROTARY_LEFT 11
#define ROTARY_RIGHT 10

Rotary saturationRotary = Rotary(ROTARY_LEFT, ROTARY_RIGHT);
#define SATURATION_JUMP 0.05

#define VALUE_DECREASE_PIN 4
#define VALUE_INCREASE_PIN 2
#define VALUE_JUMP 0.05

#define INIT_DELAY_TIME 500

#define VALUE_DIFF_DELAY 100

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);

// Bluetooth data format:
// Hue|Saturation|Value|
// [0.0-1.0]|[0.0-1.0]|[0.0-1.0]|
// 0.52|0.43|0.95|

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

hsvColor lampColor = { 0.32, 1.0, 1.0 };

void setup() {
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

  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(VALUE_DECREASE_PIN, INPUT_PULLUP);
  pinMode(VALUE_INCREASE_PIN, INPUT_PULLUP);

  hueRotary.begin(true);
  saturationRotary.begin(true);
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
  } else if(bluetooth.available() > 1) {
    float hue = bluetooth.parseFloat();
    if (bluetooth.read() != SEP_CMD_CHAR) return;
  
    float saturation = bluetooth.parseFloat();
    if (bluetooth.read() != SEP_CMD_CHAR) return;
  
    float value = bluetooth.parseFloat();
    lampColor = { hue, saturation, value };
  } else {
    return;
  }
  updateLamp();
}

void addRoundingDiff(float* value, float diff) {
  float newValue = *value + diff;
  if (newValue < 0) {
    newValue = 1.0;
  } else if (newValue > 1.0) {
    newValue = 0.0;
  }
  *value = newValue;
}

void addLimitedDiff(float* value, float diff) {
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
  Serial.println("Update RGB");
  Serial.println((int)(rgb.red * 255));
  Serial.println((int)(rgb.green * 255));
  Serial.println((int)(rgb.blue * 255));

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
