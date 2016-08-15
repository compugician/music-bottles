
#include "HX711.h"

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
    }
    if (abs(abs(detectedChange)-CAP_STRAIGHT)<TOLERANCE) {
       digitalWrite(S1,(detectedChange>0)?LOW:HIGH);
    }
    if (abs(abs(detectedChange)-CAP_SPHERE)<TOLERANCE) {
       digitalWrite(S2,(detectedChange>0)?LOW:HIGH);
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
