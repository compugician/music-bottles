#include <StackList.h>

#include <Adafruit_NeoPixel.h>

#include "weight_system.h"

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, COLOR_CYCLE, RAINBOW_CYCLE_LIMIT };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
  public:

    // Member Variables:
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position

    uint32_t Color1, Color2, Color3;  // What colors are in use
    uint32_t PrevColor1, PrevColor2, PrevColor3;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Speed;
    uint16_t Index;  // current step within the pattern

    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }

    // Update the pattern
    void Update()
    {
      if ((millis() - lastUpdate) > Interval) // time to update
      {
        lastUpdate = millis();
        switch (ActivePattern)
        {
          case RAINBOW_CYCLE:
            RainbowCycleUpdate();
            break;
          case COLOR_CYCLE:
            ColorCycleUpdate();
            break;
          case RAINBOW_CYCLE_LIMIT:
            RainbowCycleLimitUpdate();
            break;
          case THEATER_CHASE:
            TheaterChaseUpdate();
            break;
          case COLOR_WIPE:
            ColorWipeUpdate();
            break;
          case SCANNER:
            ScannerUpdate();
            break;
          case FADE:
            FadeUpdate();
            break;
          default:
            break;
        }
      }
    }

    // Increment the Index and reset at the end
    void Increment()
    {
      if (Direction == FORWARD)
      {
        Index++;
        if (Index >= TotalSteps)
        {
          Index = 0;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
      else // Direction == REVERSE
      {
        --Index;
        if (Index <= 0)
        {
          Index = TotalSteps - 1;
          if (OnComplete != NULL)
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
    }

    // Reverse pattern direction
    void Reverse()
    {
      if (Direction == FORWARD)
      {
        Direction = REVERSE;
        Index = TotalSteps - 1;
      }
      else
      {
        Direction = FORWARD;
        Index = 0;
      }
    }

    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = RAINBOW_CYCLE;
      Interval = interval;
      TotalSteps = 255;
      Index = 0;
      Direction = dir;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
      }
      show();
      Increment();
    }

    // Initialize for a RainbowCycle
    void RainbowCycleLimit(uint8_t interval, uint16_t limit, direction dir = FORWARD)
    {
      ActivePattern = RAINBOW_CYCLE_LIMIT;
      Interval = interval;
      TotalSteps = limit;
      Index = 0;
      Direction = dir;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleLimitUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Wheel(((i * (TotalSteps + 1) / numPixels()) + Index) & TotalSteps));
      }
      show();
      Increment();
    }

    // Initialize for a RainbowCycle
    void ColorCycle(uint32_t color1, uint32_t color2, uint32_t color3, uint8_t interval, uint16_t stepsSpeed, direction dir = FORWARD)
    {
      ActivePattern = COLOR_CYCLE;
      Interval = interval;
      TotalSteps = numPixels();
      Index = 0;
      PrevColor1 = Color1;
      PrevColor2 = Color2;
      PrevColor3 = Color3;
      Color1 = color1;
      Color2 = color2;
      Color3 = color3;
      Direction = dir;
      Speed = stepsSpeed;
    }

    // Update the Rainbow Cycle Pattern
    void ColorCycleUpdate()
    {
      // Calculate linear interpolation between Color1 and Color2
      // Optimise order of operations to minimize truncation error
      uint8_t red1 = ((Red(PrevColor1) * (TotalSteps - Index)) + (Red(Color1) * Index)) / Speed;
      uint8_t green1 = ((Green(PrevColor1) * (TotalSteps - Index)) + (Green(Color1) * Index)) / Speed;
      uint8_t blue1 = ((Blue(PrevColor1) * (TotalSteps - Index)) + (Blue(Color1) * Index)) / Speed;

      uint8_t red2 = ((Red(PrevColor2) * (TotalSteps - Index)) + (Red(Color2) * Index)) / Speed;
      uint8_t green2 = ((Green(PrevColor2) * (TotalSteps - Index)) + (Green(Color2) * Index)) / Speed;
      uint8_t blue2 = ((Blue(PrevColor2) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / Speed;

      uint8_t red3 = ((Red(PrevColor3) * (TotalSteps - Index)) + (Red(Color3) * Index)) / Speed;
      uint8_t green3 = ((Green(PrevColor3) * (TotalSteps - Index)) + (Green(Color3) * Index)) / Speed;
      uint8_t blue3 = ((Blue(PrevColor3) * (TotalSteps - Index)) + (Blue(Color3) * Index)) / Speed;

      for (int i = 0; i < numPixels(); i++)
      {
        if (i > Index && i <= numPixels() / 3.0 + Index) // Scan Pixel to the right
        {
          setPixelColor(i, Color(red1, green1, blue1));
        }
        else if (i > numPixels() / 3.0 + Index && i <= numPixels() * (2.0 / 3.0) + Index) // Scan Pixel to the left
        {
          setPixelColor(i, Color(red2, green2, blue2));
        }
        else // Fading tail
        {
          setPixelColor(i, Color(red3, green3, blue3));
        }
      }
      show();
      Increment();
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = THEATER_CHASE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        if ((i + Index) % 3 == 0)
        {
          setPixelColor(i, Color1);
        }
        else
        {
          setPixelColor(i, Color2);
        }
      }
      show();
      Increment();
    }

    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = COLOR_WIPE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color;
      Index = 0;
      Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
      setPixelColor(Index, Color1);
      show();
      Increment();
    }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
      ActivePattern = SCANNER;
      Interval = interval;
      TotalSteps = (numPixels() - 1) * 2;
      Color1 = color1;
      Index = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    {
      for (int i = 0; i < numPixels(); i++)
      {
        if (i == Index)  // Scan Pixel to the right
        {
          setPixelColor(i, Color1);
        }
        else if (i == TotalSteps - Index) // Scan Pixel to the left
        {
          setPixelColor(i, Color1);
        }
        else // Fading tail
        {
          setPixelColor(i, DimColor(getPixelColor(i)));
        }
      }
      show();
      Increment();
    }

    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = FADE;
      Interval = interval;
      TotalSteps = steps;
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Fade Pattern
    void FadeUpdate()
    {
      // Calculate linear interpolation between Color1 and Color2
      // Optimise order of operations to minimize truncation error
      uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
      uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
      uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

      ColorSet(Color(red, green, blue));
      show();
      Increment();
    }

    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
      // Shift R, G and B components one bit to the right
      uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
      return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
      for (int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, color);
      }
      show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
      return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
      return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
      return color & 0xFF;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos)
    {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85)
      {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }
      else if (WheelPos < 170)
      {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      else
      {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      }
    }
};


#include "HX711.h"

#define NeoPixelPin 6

//this value was obtained by calibraion
#define CALIBRATION_FACTOR -195350

//this value was obtained by reading nothing but the scale plate (without even the lights)
#define ZERO_FACTOR -96200
#define Active 7  // pins to the Pi  
#define S0 8
#define S1 9
#define S2 10
#define RESET_SW 11

#define DEBUG 0

// Pins to the load cell amp
#define CLK 2     // clock pin to the load cell amp
#define DOUT 3    // data pin to the lca

unsigned long currentMillis = 0;    // stores the value of millis() in each iteration of loop()
unsigned long previousScaleMillis = 0;
const int scaleReadInterval = 100; // number of millisecs between blinks

bool bottle_added = false;

HX711 scale(DOUT, CLK);

Change lastQuickChange;


void lightsComplete();
NeoPatterns lights(108, NeoPixelPin, NEO_RGBW + NEO_KHZ800, &lightsComplete);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, NeoPixelPin, NEO_RGBW+NEO_KHZ800);
StackList <uint32_t> ColorQueue;
void setup() {

  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(Active, OUTPUT);
  pinMode(RESET_SW, INPUT_PULLUP);

  digitalWrite(Active, HIGH);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);

  scale.set_scale(CALIBRATION_FACTOR); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();
  Serial.println("READY.");

  lights.begin();

  // Kick off a pattern
  lights.RainbowCycle(5);
  //Ring2.Color1 = Ring1.Color1;
  //Stick.Scanner(Ring1.Color(255,0,0), 55);

  lastQuickChange.valid=false;
}

int lastAvg = 0;
int baseline = 0;
float lastLargeChange = 0;

bool wentUp = false;

int stableSamples = 0;

float lastStableWeight = 0;

void loop() {
  currentMillis = millis();

  readScale();
  lights.Update();

  if (digitalRead(RESET_SW) == LOW) {
    Serial.println("RESET!");
    digitalWrite(S0, LOW);
    digitalWrite(S1, LOW);
    digitalWrite(S2, LOW);
  }

}


#define BOTTLE_TOLERANCE 30
#define CAP_TOLERANCE 30
#define BOTH_TOLERANCE 30

ObjectShape shapes[] = { cone, straight, sphere };
int bottleWeights[] = { 835, 895, 965 };
int capWeights[] = { 180, 100, 250 };

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

#define CONE_PIN S0
#define STRAIGHT_PIN S1
#define SPHERE_PIN S2


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

void handleWeightChange(int weight) {
  Serial.print("handling weight change of: ");
  Serial.println(weight);
  Change c = getChangeFromWeight(weight);
  if (lastQuickChange.valid && !changeEquals(lastQuickChange, c)) {
    //undo the last quick change...
    Serial.println("REVERSING LAST QUICK DECISION!");
    lastQuickChange.type = (lastQuickChange.type == added) ? removed : added;
    handleChange(lastQuickChange);

  }

  if (c.valid == true) {

    handleChange(c);

    //make it so we ignore the last quick change next time unless it actually re-sets by a vaild quick change...
    lastQuickChange.valid = false;
  } else {
    Serial.println("unknown change");
  }
}


void readScale() {
  if (currentMillis - previousScaleMillis >= scaleReadInterval) {
    previousScaleMillis = currentMillis;

    int avgWeight = -scale.read_average(1) / 100; //set to 1 for speed

    if (DEBUG != 0) {
      Serial.print("Read ");
      Serial.print(avgWeight);
      Serial.print("\t");
      Serial.print(lastAvg);
      Serial.print("\t");
      Serial.println(baseline);
    }

    if (abs(avgWeight - lastAvg) > 5) {
      stableSamples = 0;

      float change = avgWeight - lastAvg;
      if (change > 0) {
        if (!wentUp) {
          wentUp = true;
          lastLargeChange = change;
        } else {
          lastLargeChange += change;
        }
      } else {
        if (wentUp) {
          wentUp = false;
          lastLargeChange = change;
        } else {
          lastLargeChange += change;
        }
      }
    } else {
      if (stableSamples < 255) {
        stableSamples++;
      }
    }

    lastAvg = avgWeight;


//to gain robustness uncomment this if you implement the stack approach (see 'todo' above), Jasmine
/*
    if (stableSamples == 10) {
      Serial.println(" **** ");
      handleWeightChange(avgWeight - baseline);
      Serial.println(" **** ");

      baseline = avgWeight;
    }
*/

    if (stableSamples == 2) {
      int detectedChange;
      if ((lastLargeChange > 0 && lastStableWeight > 0) || (lastLargeChange < 0 && lastStableWeight < 0)) {
        detectedChange = lastLargeChange - lastStableWeight;
      } else {
        detectedChange = lastLargeChange;
      }

      Serial.println(" ---- ");
      handleQuickWeightChange(detectedChange);
      Serial.println(" ---- ");

      lastStableWeight = lastLargeChange;
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

void capChange(ChangeType changeType , uint32_t currColor) {
  if (changeType == added) {
    ColorQueue.push(lights.Color2);
    lights.Fade(lights.Color2, currColor, 100, 30);
  }
  else {
    if (lights.Color2 == currColor) {
      uint32_t prevColor;
      if (!ColorQueue.isEmpty()) {
        prevColor = ColorQueue.pop();
      }
      else {
        prevColor = lights.Color(127, 127, 127);
      }
      lights.Fade(lights.Color2, prevColor, 100, 30);
    }
  }
}

// Ring1 Completion Callback
void lightsComplete()
{
  //if a bottle was added - fade to white
  if (bottle_added || lights.ActivePattern == RAINBOW_CYCLE) {
    lights.Fade(lights.Color2, lights.Color(127, 127, 127), 100, 30);
  }
  else { // else, staty on the faded in color
    lights.Fade(lights.Color2, lights.Color2, 100, 30);
  }
}
