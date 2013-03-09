#ifndef _GENERAL_UTILS_H
#define _GENERAL_UTILS_H

#define PI (3.141592653589793238462643383279502884197)

int BuildTexture(char *szPathName);

float getDegreeBetweenVectors(float v1[3], float v2[3]);

void calculateCrossProduct(float v1[3], float v2[3], float result[3]);

double getDistanceBetweenPoints(float p1[3], float p2[3]);

void normalize(float v[3]);

void addTwoVectors(float v1[3], float v2[3]);

#endif
