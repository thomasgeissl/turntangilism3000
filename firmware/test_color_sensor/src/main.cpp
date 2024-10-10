#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Create an instance of the sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// Function to convert RGB to HSV
void rgbToHsv(float r, float g, float b, float &h, float &s, float &v) {
  float maxVal = max(r, max(g, b));
  float minVal = min(r, min(g, b));
  v = maxVal;

  float delta = maxVal - minVal;

  if (maxVal != 0) {
    s = delta / maxVal;
  } else {
    s = 0;
    h = -1;
    return;
  }

  if (r == maxVal) {
    h = (g - b) / delta; // between yellow & magenta
  } else if (g == maxVal) {
    h = 2 + (b - r) / delta; // between cyan & yellow
  } else {
    h = 4 + (r - g) / delta; // between magenta & cyan
  }

  h *= 60;    // convert to degrees

  if (h < 0) {
    h += 360;
  }
}

void setup() {
  Serial.begin(115200);
  
  // Check if sensor is connected
  if (!tcs.begin()) {
    Serial.println("Couldn't find TCS34725 sensor!");
    while (1);
  }

  Serial.println("TCS34725 is detected");
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Normalize RGB values (convert to 0-1 range)
  float red = (float)(r) / 65535.0;
  float green = (float)(g) / 65535.0;
  float blue = (float)(b) / 65535.0;

  // Convert RGB to HSV
  float h, s, v;
  rgbToHsv(red, green, blue, h, s, v);

  // Print out HSV values
  Serial.print("RGB: (");
  Serial.print(r); Serial.print(", ");
  Serial.print(g); Serial.print(", ");
  Serial.print(b);
  Serial.print(")  HSV: (");
  Serial.print(h); Serial.print(", ");
  Serial.print(s); Serial.print(", ");
  Serial.print(v);
  Serial.println(")");
}
