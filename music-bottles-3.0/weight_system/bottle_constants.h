
//this value was obtained by calibraion
#define CALIBRATION_FACTOR -195350

//this value was obtained by reading nothing but the scale plate (without even the lights)
#define ZERO_FACTOR -96200

//tolerances and thresholds

//a change smaller than WEIGHT_THRESH is ignored. Consecutive samples within WEIGHT_THRESH are considered the same value.
#define WEIGHT_THRESH 16

//set lower for speed
#define SAMPLE_AVGCOUNT 5 

#define SAMPLE_COUNT_QUICK 5
#define SAMPLE_COUNT_STABLE 20

#define BOTTLE_TOLERANCE 30
#define CAP_TOLERANCE 30
#define BOTH_TOLERANCE 30

#define STABLE_THRESH 44

ObjectShape shapes[] = { cone, straight, sphere };
int bottleWeights[] = { 830, 895, 965 };
int capWeights[] = { 270, 150, 475 };

