#ifndef ADSB_DECODING_H
#define ADSB_DECODING_H

/*===============================
These functions are used to extract
the information contained in an adsb
message.
=================================*/

typedef struct msg adsbMsg;

int getCallsign(char *msgi, char *msgf);
int getVelocities(char *msgi, float *speed, float *head, int *rateCD, char *tag);
int isPositionMessage(char *msgi);
int getPositionType(char *msgi);
int getCPRLatitude(char *msgi);
int getCPRLongitude(char *msgi);
int getCprNL(float lat);
int getAirbornePosition(char *msgEVEN, char *msgODD, double timeE, double timeO, float *lat, float *lon);
int getAltitude(char *msgi);
adsbMsg* setPosition(char *msg, adsbMsg *no);
adsbMsg* decodeMessage(char* buffer, adsbMsg* messages, adsbMsg** nof);

#define PI_MATH 3.14159265358979323846

static const  float transitionLatTable[58] = 
{
87.000000, 86.535370, 85.755416, 84.891662, 83.991736,
83.071994, 82.139570, 81.198014, 80.249232, 79.294282,
78.333741, 77.367895, 76.396844, 75.420563, 74.438933,
73.451774, 72.458845, 71.459865, 70.454511, 69.442426,
68.423220, 67.396468, 66.361710, 65.318452, 64.266166,
63.204275, 62.132167, 61.049178, 59.954593, 58.847639,
57.727473, 56.593190, 55.443785, 54.278175, 53.095161,
51.893426, 50.671501, 49.427767, 48.160390, 46.867334,
45.546269, 44.194548, 42.809139, 41.386522, 39.922566,
38.412421, 36.850250, 35.228999, 33.539935, 31.772098,
29.911357, 27.938991, 25.829249, 23.545043, 21.029393,
18.186265, 14.828167, 10.470480
};

#endif