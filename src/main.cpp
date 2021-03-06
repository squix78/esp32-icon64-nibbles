#include <Arduino.h>
#include <SmartLeds.h>

const int LED_COUNT = 64;
const int DATA_PIN = 22;
const int CHANNEL = 0;


// SmartLed -> RMT driver (WS2812/WS2812B/SK6812/WS2813)
SmartLed leds( LED_WS2812, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer );


void setup() {
  Serial.begin(115200); 

  // seed the random number generator 
  randomSeed(analogRead(0));
}

// convert coordinates into the index of the 
// WS2812 bus
uint8_t getLedIndex(uint8_t x, uint8_t y) {
  if (y % 2 == 0) {
    return y * 8 + x;
  } else {
    return y*8 + (7 - x);
  }
}

void showRgb() {
    leds[ 0 ] = Rgb{ 255, 0, 0 };
    leds[ 1 ] = Rgb{ 0, 255, 0 };
    leds[ 2 ] = Rgb{ 0, 0, 255 };
    leds[ 3 ] = Rgb{ 0, 0, 0 };
    leds[ 4 ] = Rgb{ 255, 255, 255 };
    leds.show();
}

struct Coordinates {
  uint8_t x;
  uint8_t y;
};
uint8_t counter = 0;
const uint8_t nibblesMaxLength = 10;
uint8_t nibblesCurrentLength = 5;
Coordinates nibbles[nibblesMaxLength];
Coordinates food;

int gradientX = 1;
int gradientY = 0;
uint8_t field[8][8] = {{3,3}};

uint8_t headIndex = 0;
bool isInitialized = false;
uint8_t foodHue = random(255);
uint8_t hue = 20;

int sgn(int value) {
  if (value == 0) {
    return 0;
  } else if (value < 0) {
    return -1;
  } else {
    return 1;
  }
}

void showNibbels() {
  if (!isInitialized) {
    headIndex = 0;
    nibblesCurrentLength = 5;
    for (int i = 0; i < nibblesCurrentLength; i++) {
      nibbles[i] = {3,3};
    }
    food.x = random(8);
    food.y = random(8);
    foodHue = random(255);

    isInitialized = true;
  }
  
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      field[x][y] = 0;
    }
  }

  for (int i = 0; i < nibblesCurrentLength; i++) {
    field[nibbles[i].x][nibbles[i].y] = 1;
  }

  Coordinates head = nibbles[headIndex];
  uint8_t i = 0;
  while(true) {
    Coordinates newHead = {4,4};
    uint8_t direction = random(2);
    switch(direction) {
      case 0:
        newHead.x = head.x - sgn(head.x - food.x);
        newHead.y = head.y;
        break;
      case 1:
      default:
        newHead.x = head.x;
        newHead.y = head.y - sgn(head.y - food.y);
        break;
    }

    if (newHead.x >= 0 && newHead.x < 8 
      && newHead.y >= 0 && newHead.y < 8
      && field[newHead.x][newHead.y] == 0
      ) {
        headIndex = (headIndex + 1) % nibblesCurrentLength;
        nibbles[headIndex].x = newHead.x;
        nibbles[headIndex].y = newHead.y;
        if (newHead.x == food.x && newHead.y == food.y) {
          while(true) {
            food.x = random(8);
            food.y = random(8);
            if (field[food.x][food.y] == 0) {
              break;
            }
          }
          hue = foodHue;
          foodHue = random(255);
          if (nibblesCurrentLength < nibblesMaxLength) {
            nibblesCurrentLength++;
            nibbles[nibblesCurrentLength - 1].x = newHead.x;
            nibbles[nibblesCurrentLength - 1].y = newHead.y;
          }
        }
        break;
    }
      
    if (i > 100) {
      isInitialized = false;
      break;
    }
    i++;
  }

  // Clear Leds
  for ( int i = 0; i != LED_COUNT; i++ ) {
    leds[i] = Hsv{ static_cast< uint8_t >( hue ), 255, 0 };
  }

  // draw nibbles
  for (int i = 0; i < nibblesCurrentLength; i++) {
    uint8_t color = hue;
    if (i == headIndex) {
      color = 30;
    }
    leds[getLedIndex(nibbles[i].x, nibbles[i].y)] = Hsv{ color, 255, 255 };
  }
  leds[getLedIndex(food.x, food.y)] = Hsv{ foodHue, 255, 255 };
  leds.show();
  
}

void loop() {
    showNibbels();
    delay( 300 );
}