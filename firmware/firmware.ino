#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <AceButton.h>
#include "esp_now_midi.h"
#include "Adafruit_TCS34725.h"
#include "./config.h"

using namespace ace_button;


esp_now_midi ESP_NOW_MIDI;
AceButton _recordButton(RECORD_BUTTON_PIN);

TwoWire _playI2CWire = TwoWire(0);
TwoWire _recordI2CWire = TwoWire(1);
Adafruit_TCS34725 _playTcs = Adafruit_TCS34725();
Adafruit_TCS34725 _recordTcs = Adafruit_TCS34725();
bool _recording = false;  //recording is switched on
int _recordCC = -1;       //cc thats currently recording
unsigned long _frameCounter = 0;


// Function to send a MIDI CC message with value 127 based on the color range
void sendMidiMessage(int midiCC, int value = 127) {
  esp_err_t result = ESP_NOW_MIDI.sendControlChange(midiCC, value, ID);
  if (result == ESP_OK) {
    Serial.print("MIDI CC ");
    Serial.print(midiCC);
    Serial.print(" with value 127 sent successfully");
    Serial.println();
  } else {
    Serial.println("Error sending MIDI CC");
  }
}

// Function to initialize a TCS34725 sensor with a specific I2C wire object
void setupSensor(Adafruit_TCS34725& sensor, TwoWire& wire) {
  if (sensor.begin(TCS34725_ADDRESS, &wire)) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1) {}
  }
}

void handleEvent(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  switch (eventType) {
    case AceButton::kEventPressed:
      sendMidiMessage(_recordCC, 127);
      _recording = true;
      Serial.println("start recording");
      break;
    case AceButton::kEventReleased:
      sendMidiMessage(_recordCC, 0);
      _recording = false;
      Serial.println("stop recording");
      break;
    case AceButton::kEventLongPressed:
      Serial.println("Button Long Pressed");
      break;
  }
}

// Function to check if a color is within a specified range
bool isColorInRange(uint16_t r, uint16_t g, uint16_t b, ColorRange range) {
  return (r >= range.rLower && r <= range.rUpper) && (g >= range.gLower && g <= range.gUpper) && (b >= range.bLower && b <= range.bUpper);
}


void customOnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) {
  Serial.print("Custom Callback - Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

void setup() {
  Serial.begin(115200);

  // setup esp now midi
  WiFi.mode(WIFI_STA);
  ESP_NOW_MIDI.setup(broadcastAddress, customOnDataSent);

  // setup color sensors
  setupSensor(_playTcs, _playI2CWire);
  setupSensor(_recordTcs, _recordI2CWire);

  // turn on the lights for both color sensors
  pinMode(PLAY_LED_PIN, OUTPUT);
  pinMode(RECORD_LED_PIN, OUTPUT);
  digitalWrite(PLAY_LED_PIN, HIGH);
  digitalWrite(RECORD_LED_PIN, HIGH);

  // setup the record button
  ButtonConfig* buttonConfig = _recordButton.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  pinMode(RECORD_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  _recordButton.check();
  uint16_t r, g, b, c, colorTemp, lux;

  if (!_recording) {
    // Get raw data from the sensor
    _playTcs.getRawData(&r, &g, &b, &c);

    // Calculate color temperature and lux (optional)
    colorTemp = _playTcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = _playTcs.calculateLux(r, g, b);

    // check which token is on the player
    for (int i = 0; i < 16; i++) {
      if (isColorInRange(r, g, b, colorRanges[i])) {
        sendMidiMessage(colorRanges[i].triggerCC);
        break;  // Exit the loop once a matching range is found
      }
    }
  }

  // check which token is on the recorder
  if (_frameCounter % 32 == 0) {
    _recordTcs.getRawData(&r, &g, &b, &c);
    colorTemp = _recordTcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = _recordTcs.calculateLux(r, g, b);
    auto foundToken = false;
    for (int i = 0; i < 16; i++) {
      if (isColorInRange(r, g, b, colorRanges[i])) {
        // TODO: there should be some filtering, runningAvg, ...
        _recordCC = (colorRanges[i].recordCC);
        foundToken = true;
        break;  // Exit the loop once a matching range is found
      }
    }
    if (!foundToken) {
      _recordCC = -1;
    }
  }

  // and finally incresse the frame counter
  _frameCounter++;
}
