#include <FastLED.h> 
#include <CapacitiveSensor.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

#define NUM_LEDS      10
#define LED_DATA_PIN  4
#define NUM_MODES     12
#define TEST_PIN      3
//#define ENABLE_TEST   1

// USER VARS
int mode = 0;

// INIT STUFF

int counter = 0;
int dir = 1;
long timer = 0;

// Start Sensor
CapacitiveSensor cs1 = CapacitiveSensor(0, 1); // 10M resistor between pins 0 & 1

// Start Strip
CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, LED_DATA_PIN>(leds, NUM_LEDS);
#ifdef ENABLE_TEST
  pinMode(TEST_PIN, OUTPUT);
#endif
}

void loop() {
    static unsigned long lastTouchTime;
    long total1 = cs1.capacitiveSensor(30);    
    if (total1 > 300 && (millis() - lastTouchTime) > 800) {
      counter = 0;
      lastTouchTime = millis();
      mode++;
    }

#ifdef ENABLE_TEST
    if ((millis() - timer) < 500)
    {
      digitalWrite(TEST_PIN, LOW);
    }
    else if ((millis() - timer) < 1000)
    {
      digitalWrite(TEST_PIN, HIGH);
    }
    else
    {
      timer = millis();
    }
#endif
    
    // if mode greater than NUM_MODES reset
    if (mode > NUM_MODES) { mode = 0; }
    // main function
    doSomething(mode);
}


void doSomething(int var) {
    switch (var) {
    case 0:
        colorFill(CRGB::Black, 0);
        break;
    case 1:
        colorFill(CRGB::White, 0);
        break;
    case 2:
        colorFill(CRGB(255, 147, 41), 0); // soft-white
        break;
    case 3:
        // flame
        randomize(11,34, 200,255, random(30,80));
        break; 
    case 4:
        twinkle(CRGB(0, 0, 31), CRGB::White, 50);
        break;
//    case 5:
//        chaseLights(CRGB::Red, CRGB::Blue, 100);
//        break;  
    case 6:
        rainbow(50);
        break;
    case 7:
        rainbow(1);
        break;
    case 8:
        rainbowCycle(10);
        break;
    case 9:
        // totally random
        randomize(0,359, 255,255, 50);
        break;
    case 10:
        colorWipe(CRGB(random(255), random(255), random(255)), 100); 
        break;
    case 11:
        sineFirefly(70);
        break;  
    case 12:
        colorFirefly(100);
        break;
//    case 13:
//        chaseLightsOddEven(100);
//        break;
    default:
        mode++;
        break;
    }
    
    if (counter > 254) {
        dir = -1;
    }
    if (counter < 1) {
        dir = 1;
    }

    counter += dir;
}

// PATTERN FUNCTIONS
// Created, or adapted from Adafruit and Funkboxing!

// Fill the dots one after the other with a color
void colorWipe(CRGB c, uint8_t wait) {
  static CRGB latchedColor = c;

  if (counter > NUM_LEDS) {
    counter = 0;
    latchedColor = c;
  }

  leds[counter] = latchedColor;
  FastLED.show();
  delay(wait);
}

// Fill the entire strip with a single color at once
void colorFill(CRGB c, uint8_t wait) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = c;
  }
  FastLED.show();
  delay(wait);
}


void rainbow(uint8_t wait) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = Wheel((i + counter) & 255);
    }
    FastLED.show();
    delay(wait);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = Wheel(((i * 256 / NUM_LEDS) + counter) & 255);
    }
    FastLED.show();
    delay(wait);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
CRGB Wheel(byte WheelPos) {
  return CHSV(WheelPos, 255, 255);
}

void chaseLights(CRGB c1, CRGB c2, int wait) { //-POLICE LIGHTS (TWO COLOR SINGLE LED)
    if (counter >= NUM_LEDS) {
        counter = 0;
    }
    int idexR = counter;
    int idexB = antipodal_index(idexR);
    for (int i = 0; i < NUM_LEDS; i++) {
        if (i == idexR) {
            leds[i] = c1;
        } else if (i == idexB) {
            leds[i] = c2;
        } else {
            leds[i] = CRGB::Black;
        }
    }
    FastLED.show();
    delay(wait);
}

void chaseLightsOddEven(int wait) { //-POLICE LIGHTS (TWO COLOR ODD/EVEN)
  for (int i = 0; i < NUM_LEDS; i++) {
    if (counter & 1 && i & 1) {
      leds[i] = CRGB::Red;
    } else if (((counter|i) & 1) == 0) {
      leds[i] = CRGB::Blue;
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
  delay(wait);
}

void twinkle(CRGB base, CRGB twinkle, int wait) {
    leds[random(NUM_LEDS)] = twinkle;
    FastLED.show();
    delay(wait);
    colorFill(base, random(30,500));
}

void randomize(int minHue, int maxHue, int minVal, int maxVal, int wait) {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CHSV(random(minHue,maxHue), 255, random(minVal,maxVal));
  }    
  FastLED.show();
  delay(wait);
}

// This table generated from: sin((6.28/30)*(float)(counter)) *255;
static const uint8_t brightnessTable[] = {0,52,103,149,189,220,242,253,253,242,220,189,150,104,53,0};
void sineFirefly(int wait) {
  static int myPix = 0;
  if(counter < sizeof(brightnessTable)) {
    leds[myPix] = CHSV(brightnessTable[counter], brightnessTable[counter], brightnessTable[counter]);
    
    FastLED.show();
    delay(wait);
  } else {
    myPix = random(0,NUM_LEDS);
    counter = 0;
    colorFill(0,0);
  }
}

void colorFirefly(int wait) {
  static int myPix = 0;
  if(counter < sizeof(brightnessTable)) {
    leds[myPix] = CHSV(60, 255, brightnessTable[counter]);
//    leds[myPix] = CHSV((256/sizeof(brightnessTable))*counter, 255, brightnessTable[counter]);
    
    FastLED.show();
    delay(wait);
  } else {
    if (random(20) == 0) {
      myPix = random(0,NUM_LEDS);
      counter = 0;
      colorFill(0,0);
    } else {
      delay(wait);
    }
  }
}



// HELPER FUNCTIONS, DERIVED FROM FUNKBOXING FAST_SPI!
// YAYAYAYAYAYYAYA

// antipodal point
int antipodal_index(int i) {
    //int N2 = int(NUM_LEDS/2);
    int iN = i + NUM_LEDS / 2;
    if (i >= (NUM_LEDS / 2)) {
        iN = (i + (NUM_LEDS / 2)) % NUM_LEDS;
    }
    return iN;
}
