#pragma once
#define ID 1
#define PLAY_LED_PIN 2
#define RECORD_LED_PIN 3
#define RECORD_BUTTON_PIN 4

#define SDA_0 21
#define SCL_0 22
#define SDA_1 25
#define SCL_1 26

// Structure to represent a color range
struct ColorRange {
  uint16_t rLower;
  uint16_t rUpper;
  uint16_t gLower;
  uint16_t gUpper;
  uint16_t bLower;
  uint16_t bUpper;
  int triggerCC;
  int recordCC;
};

// Replace with your receiver MAC address
uint8_t broadcastAddress[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Define 16 color ranges with corresponding MIDI CC numbers
ColorRange colorRanges[16] = {
  { 0, 50, 0, 50, 0, 50, 1, 65 },     // Example: Dark Red range, sends MIDI CC 1
  { 51, 100, 0, 50, 0, 50, 2, 66 },   // Lighter Red, sends MIDI CC 2
  { 0, 50, 51, 100, 0, 50, 3, 67 },   // Dark Green, sends MIDI CC 3
  { 0, 50, 0, 50, 51, 100, 4, 68 },   // Dark Blue, sends MIDI CC 4
  { 101, 150, 0, 50, 0, 50, 5, 69 },  // Example ranges for different colors and MIDI CCs
  { 0, 50, 101, 150, 0, 50, 6, 70 },
  { 0, 50, 0, 50, 101, 150, 7, 71 },
  { 151, 200, 0, 50, 0, 50, 8, 72 },
  { 0, 50, 151, 200, 0, 50, 9, 73 },
  { 0, 50, 0, 50, 151, 200, 10, 74 },
  { 201, 255, 0, 50, 0, 50, 11, 75 },
  { 0, 50, 201, 255, 0, 50, 12, 76 },
  { 0, 50, 0, 50, 201, 255, 13, 77 },
  { 0, 50, 51, 100, 51, 100, 14, 78 },
  { 51, 100, 51, 100, 0, 50, 15,79 },
  { 51, 100, 0, 50, 51, 100, 16, 80 }
};