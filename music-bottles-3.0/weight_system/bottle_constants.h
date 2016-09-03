
//this value was obtained by calibraion
#define CALIBRATION_FACTOR -195350

//this value was obtained by reading nothing but the scale plate (without even the lights)
#define ZERO_FACTOR -96200

//tolerances and thresholds

//a change smaller than WEIGHT_THRESH is ignored. Consecutive samples within WEIGHT_THRESH are considered the same value.
#define WEIGHT_THRESH 16

//set lower for speed (5, 5, 20 works well, 2016-08-27)
#define SAMPLE_AVGCOUNT 5 
#define SAMPLE_COUNT_QUICK 3
#define SAMPLE_COUNT_STABLE 12

#define BOTTLE_TOLERANCE 30
#define CAP_TOLERANCE 30
#define BOTH_TOLERANCE 30

#define STABLE_THRESH 40

ObjectShape shapes[] = { cone, straight, sphere };

//CONE:
int bottleWeights[] = { 739 , 788 , 856 };
int capWeights[] = { 228 , 147 , 430 };

//SQUARE:
//int bottleWeights[] = { 955 , 788 , 856 };
//int capWeights[] = { 319 , 216 , 370 };

