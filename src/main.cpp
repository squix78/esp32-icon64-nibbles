#include <Arduino.h>
#include <SmartLeds.h>

const int LED_COUNT = 64;
const int DATA_PIN = 22;
const int CHANNEL = 0;

enum Animation {
  Gradient, 
  Nibbels
};

enum Direction {
  Up, Down, Left, Right
};

uint8_t nibbelsX = 5;
uint8_t nibbelsY = 5;

Animation animation = Nibbels;

// SmartLed -> RMT driver (WS2812/WS2812B/SK6812/WS2813)
SmartLed leds( LED_WS2812, LED_COUNT, DATA_PIN, CHANNEL, DoubleBuffer );

//const int CLK_PIN = 23;
// APA102 -> SPI driver
//Apa102 leds(LED_COUNT, CLK_PIN, DATA_PIN, DoubleBuffer);

void setup() {
  Serial.begin(9600);  
  randomSeed(analogRead(0));
}

uint8_t getLedIndex(uint8_t x, uint8_t y) {
  // y = 0; x = 0: index = 0
  // y = 0; x = 7; index = 7
  // y= 1; x = 0; index = 15
  // y = 1; x = 7; index = 8;
  // y = 1; x = 6; index = 9;
  // y = 2; x = 0; index = 16;
  // y = 2; x = 7; index = 23;
  if (y % 2 == 0) {
    return y * 8 + x;
  } else {
    return y*8 + (7 - x);
  }
  
}

Direction getOppositeDirection(Direction direction) {
  switch (direction) {
  case Up:
    return Down;
  case Down:
    return Up;
  case Left:
    return Right;
  case Right:
    return Left;
  }
}

uint8_t hue;
void showGradient() {
    hue++;
    // Use HSV to create nice gradient
    for ( int i = 0; i != LED_COUNT; i++ )
        leds[ i ] = Hsv{ static_cast< uint8_t >( hue ), 255, 10 };
    leds.show();
    // Show is asynchronous; if we need to wait for the end of transmission,
    // we can use leds.wait(); however we use double buffered mode, so we
    // can start drawing right after showing.
    if (hue> 200) {
      animation = Nibbels;
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

uint8_t counter = 0;
const uint8_t nibbelsLength = 5;
uint8_t x[nibbelsLength] = {5,5,5,5,5};
uint8_t y[nibbelsLength] = {5,5,5,5,5};
Direction nibbelsDirection = Up;
int gradientX = 1;
int gradientY = 0;
void showNibbels() {
  counter++;
  hue++;
  uint8_t head = counter % nibbelsLength;
  uint8_t oldHead = (counter - 1) % nibbelsLength;
  uint8_t oldX = x[oldHead];
  uint8_t oldY = y[oldHead];

  if (oldX == 7 && nibbelsDirection == Right) {
      if (random(9)> 4) {
        nibbelsDirection = Up;
      } else {
        nibbelsDirection = Down;
      }
  } else if (oldX == 0 && nibbelsDirection == Left) {
      if (random(9)> 4) {
        nibbelsDirection = Up;
      } else {
        nibbelsDirection = Down;
      }
  } else if (oldY == 7 && nibbelsDirection == Up) {
    if (random(9)> 4) {
      nibbelsDirection = Right;
    } else {
      nibbelsDirection = Left;
    }
  } else if (oldY == 0 && nibbelsDirection == Down) {
    if (random(9)> 4) {
      nibbelsDirection = Right;
    } else {
      nibbelsDirection = Left;
    }
  } else {
    Direction newDirection = (Direction) random(4);
    if (getOppositeDirection(newDirection) != nibbelsDirection) {
      nibbelsDirection = newDirection;
    }
  }

  switch(nibbelsDirection) {
    case Up:
      gradientX = 0;
      gradientY = 1;
      break;
    case Down:
      gradientX = 0;
      gradientY = -1;
      break;
    case Left:
      gradientX = -1;
      gradientY = 0;
      break;
    case Right:
      gradientX = 1;
      gradientY = 0;
      break;
  }
  if (oldX + gradientX > 7 || oldX + gradientX <0) {
    gradientX = 0;
  }
  if (oldY + gradientY > 7 || oldY + gradientY <0) {
    gradientY = 0;
  }
  x[head] = (oldX + gradientX) % 8;
  y[head] = (oldY + gradientY) % 8;
  Serial.printf("%d, %d\n", x[head], y[head]);
  //x[head] = (uint8_t) (sin(2 * PI * counter / 15) * 3.2 + 4.0);
  //y[head] = (uint8_t) (cos(2 * PI * counter / 15) * 3.2 + 4.0);
  
  for ( int i = 0; i != LED_COUNT; i++ ) {
    leds[i] = Hsv{ static_cast< uint8_t >( hue ), 255, 0 };
  }
  uint8_t color = hue;
  for (int i = 0; i < nibbelsLength; i++) {
    if (i == head) {
      color = 10;
    } else {
      color = hue;
    }
    leds[getLedIndex(x[i], y[i])] = Hsv{ color, 255, 255 };
  }

  leds.show();

}

void loop() {
    
    //if ( millis() % 10000 < 5000 )
    switch(animation) {
      case Gradient:
        showGradient();
        break;
      case Nibbels:
        showNibbels();
        break;

    }
        
    //else
    //    showRgb();
    delay( 200 );
}