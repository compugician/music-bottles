
#include "HX711.h"


// Pins to the load cell amp
#define CLK 2     // clock pin to the load cell amp
#define DOUT 3    // data pin to the lca

//this value was obtained by calibraion
#define CALIBRATION_FACTOR -195350

//this value was obtained by reading nothing but the scale plate (without even the lights)
#define ZERO_FACTOR -96200

HX711 scale(DOUT, CLK);

void setup() {
  Serial.begin(115200);
  
  scale.set_scale(CALIBRATION_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(ZERO_FACTOR);
  Serial.println("READY.");

}

void speedTest() {
  unsigned long a = millis();
  int weight = scale.read_average(1);
  unsigned long b = millis();

  Serial.print("Took ");
  Serial.print(b-a);
  Serial.print("ms to get: ");
  Serial.println(weight);
  
}

void lowPassTest(const float alpha[],int n) {
  static int initSize = 0;  

  const int SAMP_COUNT = 1;
  unsigned long mStart = millis();
  long value =  scale.read_average(SAMP_COUNT);
  unsigned long mDuration = (millis()-mStart)/SAMP_COUNT;

  const int CNT_TO_STABLE = 5;
  const int STABLE_THRESH = 3000; // +/-

  static long* reading; 
  static long* readingLastStable;
  static int* stableCount;
   
  if (initSize!=n) {
    if (0==reading) {
      reading = (long*) malloc(n * sizeof(long));
      readingLastStable = (long*) malloc(n * sizeof(long));
      stableCount = (int*) malloc(n * sizeof(int));

      for (int i = 0; i<n; i++) {
        reading[i]=value;
        readingLastStable[i]=value;
        stableCount[i]=0;
      }

    } else {
      reading = (long*) realloc(reading, n * sizeof(long));
      readingLastStable = (long*) realloc(readingLastStable, n * sizeof(long));
      stableCount = (int*) realloc(stableCount, n * sizeof(int));
    }
    initSize = n;
  }

  for (int i = 0; i<n; i++) {
    long newValue = reading[i] * (1-alpha[i]) + value * alpha[i];    
    if (abs(reading[i]-value)<STABLE_THRESH) {
      if (stableCount[i]<255) {
        stableCount[i]++;      
      }
    } else {
      stableCount[i] = 0;
    }
    
    bool isStable = (((int)stableCount[i])>=CNT_TO_STABLE);
    
    reading[i] = newValue;

    if (isStable && abs(readingLastStable[i]-reading[i])>STABLE_THRESH) {
      //TODO
    }

    Serial.print((long)reading[i]/100);
    Serial.print(" ");
    Serial.print(isStable?"*":" ");
    
    if (i<n-1) {Serial.print("\t"); } else { Serial.print("\t"); Serial.print(mDuration); }
  }
  Serial.println();
}

void loop() {

  //speedTest();
   
  lowPassTest((const float[]){1, 0.20, 0.10 , 0.05},4);

}
