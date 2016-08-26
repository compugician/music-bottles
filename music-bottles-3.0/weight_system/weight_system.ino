#include <StackList.h>
#include <Adafruit_NeoPixel.h>

#include "pinConfiguration.h"

#include "weight_system.h"

#include "neopixelbottles.h"

#include "bottle_constants.h"

#include "HX711.h"

#define DEBUG 0

unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousScaleMillis = 0;

//disabled this (set to 1), to maximize speed. Put this back if lights become unstable...
const int scaleReadInterval = 1; // number of millisecs between blinks

long everythingOnWeight;

HX711 scale(DOUT_PIN, CLK_PIN);

Change lastQuickChange;

Bottles bottleStates;

void setStatesAllOn() {
  bottleStates.cone.bottleOn = true;
  bottleStates.cone.capOn = true;
  bottleStates.straight.bottleOn = true;
  bottleStates.straight.capOn = true;
  bottleStates.sphere.bottleOn = true;
  bottleStates.sphere.capOn = true;  
}

void tareWithEverythingOn() {
  Serial.println("Setting 'all on' weight.");
  scale.tare(100); // take the average of 100 measurements to set the '0' weight;
  everythingOnWeight = sampleScale();
  setStatesAllOn();
  Serial.println("READY.");
}

void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(CONE_PIN, OUTPUT);
  pinMode(STRAIGHT_PIN, OUTPUT);
  pinMode(SPHERE_PIN, OUTPUT);
  pinMode(Active, OUTPUT);
  pinMode(RESET_SW, INPUT_PULLUP);

  digitalWrite(Active, HIGH);
  digitalWrite(CONE_PIN, LOW);
  digitalWrite(STRAIGHT_PIN, LOW);
  digitalWrite(SPHERE_PIN, LOW);

  scale.set_scale(CALIBRATION_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
//  scale.tare(ZERO_FACTOR);
  tareWithEverythingOn();
 
  lights.begin();

  // Kick off a pattern
  lights.RainbowCycle(5);
  //Ring2.Color1 = Ring1.Color1;
  //Stick.Scanner(Ring1.Color(255,0,0), 55);

  lastQuickChange.valid=false;
}

int lastAvg = 0;

float lastLargeChange = 0;

bool wentUp = false;

int stableSamples = 0;

long lastStableWeight = 0;


Change getChangeFromWeight(int weight) {
  bool found = false;
  Change result;

  result.type =  (weight < 0) ? removed : added;

  for (int i = 0; i < 3 && !found; i++) {
    if (abs(abs(weight) - capWeights[i]) < CAP_TOLERANCE) {
      result.shape = shapes[i];
      result.object = cap;
      found = true;
    } else if (abs(abs(weight) - bottleWeights[i]) < BOTTLE_TOLERANCE) {
      result.shape = shapes[i];
      result.object = bottle;
      found = true;
    } else if (abs(abs(weight) - bottleWeights[i] - capWeights[i]) < BOTH_TOLERANCE) {
      result.shape = shapes[i];
      result.object = both;
      found = true;
    }
  }
  result.valid = found;

  return result;
}

void handleChange(Change change) {
  Serial.print("Handling: ");
  Serial.println(changeToString(change));

  if (change.valid == false) {
    Serial.println("NO!");
    return;
  }
  
  int value = (change.type == added) ? LOW : HIGH;

  //if a bottle was added, do the same logic as the previous code [[this is for Jasmine to change later, if/however desired, only here for backwards compatibility with rest of logic]
  if (change.object == bottle && change.type == added) {
    bottle_added = true;
    Serial.println("bottle added");
    lights.Fade(lights.Color(127, 127, 127), lights.Color(0, 80, 0), 100, 30);
  }

  
  if (change.object == bottle && change.type == removed) {
    switch (change.shape) {
      case cone:
        digitalWrite(CONE_PIN, LOW);
        break;
  
      case straight:
        digitalWrite(STRAIGHT_PIN, LOW);
        break;
  
      case sphere:
        digitalWrite(SPHERE_PIN, LOW);
        break;
  
      default:
        break;
    }
  }

  if (change.object == cap) {
    switch (change.shape) {
      case cone:
        digitalWrite(CONE_PIN, value);
        capChange(change.type, lights.Color(248, 255, 103));
        break;
  
      case straight:
        digitalWrite(STRAIGHT_PIN, value);
        capChange(change.type, lights.Color(231, 103, 255));
        break;
  
      case sphere:
        digitalWrite(SPHERE_PIN, value);
        capChange(change.type, lights.Color(103, 255, 211));
        break;
  
      default:
        break;
    }
  }
}

String bottleToString(BottleState bs) {
  String result = "";
  result=((bs.bottleOn)?"Bottle: ON ":"Bottle: OFF");
  result+=" ";
  result+=((bs.capOn)?"Cap: ON ":"Cap: OFF");

  return result;
}

String stateToString(Bottles b) {
  return "Cone:     "+bottleToString(b.cone)+"\n"
        +"Straight: "+bottleToString(b.straight)+"\n"
        +"Sphere:   "+bottleToString(b.sphere)+"\n";
}

String changeToString(Change c) {
  String result = "Change: ";
  result += (c.valid) ? "VALID" : "INVALID";
  result += "\t";
  result += (c.type == added) ? "ADDED" : "REMOVED";
  result += "\t";
  switch (c.object) {
    case cap:
      result += "CAP";
      break;
    case bottle:
      result += "BOTTLE";
      break;
    case both:
      result += "BOTH";
      break;
    default:
      result += "UNKNONW";
      break;
  }

  result += "\t";

  switch (c.shape) {
    case cone:
      result += "CONE";
      break;
    case straight:
      result += "STRAIGHT";
      break;
    case sphere:
      result += "SPHERE";
      break;
    default:
      result += "UNKNOWN";
      break;
  }

  return result;
}

//todo: this could be a stack of changes, instead of just the last valid one.
void handleQuickWeightChange(int weight) {
  Serial.print("handling QUICK weight change of: ");
  Serial.println(weight);

  Change c = getChangeFromWeight(weight);
  if (c.valid == true) {
    lastQuickChange = c;
    handleChange(c);
  } else {
    Serial.println("unknown change");
  }

}

bool changeEquals(Change c1, Change c2) {
  return ( c1.valid == c2.valid
           && c1.type == c2.type
           && c1.object == c2.object
           && c1.shape == c2.shape);
}

void setBottleStates(int cone, int straight, int sphere) {
  Serial.print("Setting Bottle States: ");
  Serial.print(sphere);
  Serial.print(straight);
  Serial.print(cone);
  bottleStates.cone.bottleOn = (cone==0 || cone==1);
  bottleStates.cone.capOn = (cone==0);

  bottleStates.straight.bottleOn = (straight==0 || straight==1);
  bottleStates.straight.capOn = (straight==0);
  
  bottleStates.sphere.bottleOn = (sphere==0 || sphere==1);
  bottleStates.sphere.capOn = (sphere==0);
  
}

void handleWeightChangeAbsolute(int weight) {
  int found = 0;
  Serial.print("Handling Absolute Weight: ");
  Serial.println(weight);
  for (int i=0; i<27; i++) {
    int cone = i%3;
    int straight = (i/3)%3;
    int sphere = (i/9)%3; 

    // for each bottle: 0 is both bottle and cap are on, 1 is only the bottle is on, and 2 is both bottle and cap are missing...
    int weightTarget = 0;
    if (cone==1) { weightTarget -= capWeights[0]; }
    if (cone==2) { weightTarget -= (bottleWeights[0] + capWeights[0]); }
    
    if (straight==1) { weightTarget -= capWeights[1]; }
    if (straight==2) { weightTarget -= (bottleWeights[1] + capWeights[1]); }

    if (sphere==1) { weightTarget -= capWeights[2]; }
    if (sphere==2) { weightTarget -= (bottleWeights[2] + capWeights[2]); }

    Serial.print(i);
    Serial.print(":");
    Serial.print(sphere);
    Serial.print(":");
    Serial.print(straight);
    Serial.print(":");
    Serial.print(cone);
    Serial.print(":");
    Serial.print(weightTarget);

    if (abs(weightTarget - weight)<STABLE_THRESH) {
      found++;
      Serial.print(" <<< MATCH ");
      setBottleStates(cone,straight,sphere);
    } 
    Serial.println();
  }
  if (found==0) {
    Serial.println("!!!!!!!!! FOUND NO MATCHING SETTINGS !!!!!!!!!!!!");
  } else if (found>1) {
    Serial.println("!!!!!!!!! FOUND MORE THAN ONE MATCHING SETTINGS !!!!!!!!!!!!");
  }

  digitalWrite(CONE_PIN, (!bottleStates.cone.capOn && bottleStates.cone.bottleOn) ? HIGH : LOW);
  digitalWrite(STRAIGHT_PIN, (!bottleStates.straight.capOn && bottleStates.straight.bottleOn) ? HIGH : LOW);
  digitalWrite(SPHERE_PIN, (!bottleStates.sphere.capOn && bottleStates.sphere.bottleOn) ? HIGH : LOW);
  
}

int sampleScale() {
  return -scale.read_average(SAMPLE_AVGCOUNT) / 100; 
}

void readScale() {
  if (currentMillis - previousScaleMillis >= scaleReadInterval) {
    previousScaleMillis = currentMillis;

    int avgWeight = sampleScale();

    if (abs(avgWeight - lastAvg) > WEIGHT_THRESH) {
      stableSamples = 0;

      lastLargeChange = avgWeight - lastStableWeight;
    } else {
      if (stableSamples < 255) {
        stableSamples++;
      }
    }

    lastAvg = avgWeight;


    if (stableSamples == SAMPLE_COUNT_STABLE) {
      Serial.println(" **** ");
      
      Serial.print(" STABLE ");
      Serial.print(avgWeight);
      Serial.print(" [");
      Serial.print(avgWeight - lastStableWeight );
      Serial.print("]");
      Serial.print(" {");
      Serial.print(avgWeight - everythingOnWeight);
      Serial.println("}");
      lastStableWeight = avgWeight;
 
      handleWeightChangeAbsolute(avgWeight - everythingOnWeight);
      Serial.print(stateToString(bottleStates));
     
      Serial.println(" **** ");

    }

    if (stableSamples == SAMPLE_COUNT_QUICK) {
      Serial.println(" ---- ");
      handleQuickWeightChange(avgWeight - lastStableWeight);
      Serial.println(" ---- ");
    }

    if (DEBUG != 0) {
      Serial.print("LLC ");
      Serial.print(lastLargeChange);
      Serial.print(" : avgW ");
      Serial.print(avgWeight);
      Serial.print(" : stable ");
      Serial.print(stableSamples);
      Serial.print(""); //You can change this to kg but you'll need to refactor the calibration_factor
      Serial.println();
    }

  }

}


void handleReset() {
  static bool resetClickedFlag = false;
  if (digitalRead(RESET_SW) == LOW) {
    if (!resetClickedFlag) {
      Serial.println("RESET!");
      digitalWrite(CONE_PIN, LOW);
      digitalWrite(STRAIGHT_PIN, LOW);
      digitalWrite(SPHERE_PIN, LOW);

      tareWithEverythingOn();
      
      resetClickedFlag = true;
    }
  } else {
    resetClickedFlag = false;
  }
}

void loop() {
  currentMillis = millis();

  readScale();
  lights.Update();
  handleReset();
}
