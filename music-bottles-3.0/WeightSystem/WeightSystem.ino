
#include "HX711.h"
#include <Adafruit_NeoPixel.h>

#define NeoPixelPin 6

#define calibration_factor 200

#define Active 7  // pins to the Pi  
#define S0 8
#define S1 9
#define S2 10

#define CAP_CONE 80
#define CAP_SPHERE 129
#define CAP_STRAIGHT 105
#define TOLERANCE 10

#define DEBUG 0

// Pins to the load cell amp
#define CLK 2     // clock pin to the load cell amp
#define DOUT 3    // data pin to the lca

HX711 scale(DOUT, CLK);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, NeoPixelPin, NEO_RGBW+NEO_KHZ800);

void setup() {

   // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(Active, OUTPUT);

  digitalWrite(Active, HIGH);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);

  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
  Serial.println("READY.");

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

const int NUM_SAMPLES = 6;
float samples[NUM_SAMPLES];
int sampNum = 0;


float getAvgWeight() {
  float result = 0;
  for (int i = 0; i<NUM_SAMPLES; i++) {
     result += samples[i];
  }

  return result/NUM_SAMPLES;
}

float lastAvg = 0;
float lastLargeChange = 0;

bool wentUp = false;

int stableSamples = 0;

float lastStableWeight = 0;

void loop() {
  delay(100);
  float weight = scale.get_units();

  samples[sampNum++] = weight;

  if (sampNum>=NUM_SAMPLES) { sampNum = 0; }
  float avgWeight = getAvgWeight();

  if (abs(avgWeight - lastAvg)>5) {
    stableSamples = 0;
    
    float change = avgWeight-lastAvg;
    if (change>0) {
      if (!wentUp) {
        wentUp = true;
        lastLargeChange = change;
      } else {
        lastLargeChange += change;        
      }
    } else {
      if (wentUp) {
        wentUp=false;
        lastLargeChange = change;
      } else {
        lastLargeChange += change;
      }
    }
  } else {
    stableSamples++;
  }

  lastAvg = avgWeight;

  if (stableSamples==5) {
    float detectedChange;
    if ((lastLargeChange>0 && lastStableWeight >0) || (lastLargeChange<0 && lastStableWeight<0)) {
      detectedChange = lastLargeChange - lastStableWeight;
    } else {
      detectedChange = lastLargeChange;
    }
    Serial.println("------------");
    Serial.println(detectedChange);
    Serial.println("------------");

    if (abs(abs(detectedChange)-CAP_CONE)<TOLERANCE) {
       digitalWrite(S0,(detectedChange>0)?LOW:HIGH);
       if (detectedChange<=0) {
          rainbow(2);
          for (uint16_t i=0; i < strip.numPixels(); i=i+1) {
            strip.setPixelColor(i, 0);        //turn every third pixel off
          }
          strip.show();
       }
    }
    if (abs(abs(detectedChange)-CAP_STRAIGHT)<TOLERANCE) {
       digitalWrite(S1,(detectedChange>0)?LOW:HIGH);
       if (detectedChange<=0) {
          rainbow(2);
          for (uint16_t i=0; i < strip.numPixels(); i=i+1) {
            strip.setPixelColor(i, 0);        //turn every third pixel off
          }
          strip.show();
       }
    }
    if (abs(abs(detectedChange)-CAP_SPHERE)<TOLERANCE) {
       digitalWrite(S2,(detectedChange>0)?LOW:HIGH);
       if (detectedChange<=0) {
          rainbow(2);
          for (uint16_t i=0; i < strip.numPixels(); i=i+1) {
            strip.setPixelColor(i, 0);        //turn every third pixel off
          }
          strip.show();
       }
    }
    
    lastStableWeight = lastLargeChange;
  }
  
  if (DEBUG!=0) {
    Serial.print("LLC ");
    Serial.print(lastLargeChange);
    Serial.print(" : avgW ");
    Serial.print(avgWeight);
    Serial.print(" : weight");
    Serial.print(weight); //scale.get_units() returns a float
    Serial.print(" : stable ");
    Serial.print(stableSamples);
    Serial.print(""); //You can change this to kg but you'll need to refactor the calibration_factor
    Serial.println();
  }
//  
//  digitalWrite(S0, LOW);
//  digitalWrite(S1, LOW);
//  digitalWrite(S2, LOW);
//  Serial.println("before");
//  delay(7000);
//  digitalWrite(S1, HIGH);
//  Serial.println("after 1");
//  delay(3000);
//  digitalWrite(S2, HIGH);
//  Serial.println("after 2");
//  delay(3000);
//  digitalWrite(S0, HIGH);
//  Serial.println("after 0");
//  delay(3000);
  // put your main code here, to run repeatedly:

}


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 20));
    }
    strip.show();
    delay(wait);
  }
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
