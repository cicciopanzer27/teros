/**
 * @file math.h
 * @brief Mathematical functions header for TEROS kernel
 * @note Freestanding implementation (no SSE, no system libc)
 */

#ifndef MATH_H
#define MATH_H

#include <stdint.h>

// Constants
#define M_PI 3.14159265358979323846
#define M_E 2.71828182845904523536
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.78539816339744830962

// Basic math
int abs(int x);
long int labs(long int x);
double fabs(double x);

// Power and roots
double pow(double x, double y);
double sqrt(double x);
double cbrt(double x);

// Exponential and logarithmic
double exp(double x);
double exp2(double x);
double log(double x);
double log10(double x);
double log2(double x);

// Trigonometric
double sin(double x);
double cos(double x);
double tan(double x);
double asin(double x);
double acos(double x);
double atan(double x);
double atan2(double y, double x);

// Hyperbolic
double sinh(double x);
double cosh(double x);
double tanh(double x);

// Rounding
double ceil(double x);
double floor(double x);
double round(double x);

// Modulo
double fmod(double x, double y);
double modf(double x, double* intpart);

// Gamma function
double gamma(double x);

#endif // MATH_H

