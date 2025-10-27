/**
 * @file math.c
 * @brief Mathematical functions
 * @note Adapted from musl libc and standard C libraries (MIT/Public Domain)
 * @author TEROS Development Team
 * @date 2025
 */

#include "math.h"
#include <stdint.h>
#include <limits.h>

#define PI 3.14159265358979323846
#define E 2.71828182845904523536

// Basic math functions
int abs(int x) {
    return x < 0 ? -x : x;
}

long int labs(long int x) {
    return x < 0 ? -x : x;
}

double fabs(double x) {
    return x < 0 ? -x : x;
}

// Power and roots
double pow(double x, double y) {
    if (y == 0) return 1.0;
    if (x == 0) return 0.0;
    if (y == 1) return x;
    if (y == -1) return 1.0 / x;
    
    // Simple approximation for pow
    double result = 1.0;
    int i;
    for (i = 0; i < (int)y; i++) {
        result *= x;
    }
    return result;
}

double sqrt(double x) {
    if (x < 0) return 0.0;
    if (x == 0) return 0.0;
    if (x == 1) return 1.0;
    
    // Babylonian method
    double guess = x / 2;
    double prev = 0;
    int iterations = 0;
    
    while (guess != prev && iterations < 100) {
        prev = guess;
        guess = (guess + x / guess) / 2;
        iterations++;
    }
    
    return guess;
}

double exp(double x) {
    // Taylor series: exp(x) = sum(x^n / n!)
    double result = 1.0;
    double term = 1.0;
    
    for (int n = 1; n < 20; n++) {
        term *= x / n;
        result += term;
        if (term < 1e-10) break;
    }
    
    return result;
}

double log(double x) {
    if (x <= 0) return 0.0;
    if (x == 1) return 0.0;
    
    // Taylor series approximation
    double result = 0.0;
    double y = (x - 1) / (x + 1);
    double y2 = y * y;
    double term = y;
    
    for (int n = 1; n < 100; n += 2) {
        result += term / n;
        term *= y2;
        if (term < 1e-10) break;
    }
    
    return 2.0 * result;
}

double log10(double x) {
    return log(x) / 2.3025850929940459; // ln(10)
}

double log2(double x) {
    return log(x) / 0.6931471805599453; // ln(2)
}

// Trigonometric functions
double sin(double x) {
    // Normalize to [-PI, PI]
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    
    // Taylor series: sin(x) = x - x^3/3! + x^5/5! - ...
    double result = x;
    double term = x;
    double x2 = x * x;
    
    for (int n = 3; n < 20; n += 2) {
        term *= -x2 / ((n - 1) * n);
        result += term;
    }
    
    return result;
}

double cos(double x) {
    // cos(x) = sin(PI/2 - x)
    return sin(PI / 2 - x);
}

double tan(double x) {
    double s = sin(x);
    double c = cos(x);
    return c != 0 ? s / c : 0.0;
}

// Inverse trigonometric
double asin(double x) {
    // Approximation for small x
    return x + (x * x * x) / 6 + (3 * x * x * x * x * x) / 40;
}

double acos(double x) {
    return PI / 2 - asin(x);
}

double atan(double x) {
    if (x < -1) return -PI / 2 - atan(1 / x);
    if (x > 1) return PI / 2 - atan(1 / x);
    
    // Taylor series for |x| < 1
    double result = x;
    double x2 = x * x;
    double term = x;
    
    for (int n = 3; n < 20; n += 2) {
        term *= -x2;
        result += term / n;
    }
    
    return result;
}

double atan2(double y, double x) {
    if (x > 0) return atan(y / x);
    if (x < 0 && y >= 0) return atan(y / x) + PI;
    if (x < 0 && y < 0) return atan(y / x) - PI;
    if (x == 0 && y > 0) return PI / 2;
    if (x == 0 && y < 0) return -PI / 2;
    return 0;
}

// Hyperbolic functions
double sinh(double x) {
    double ex = exp(x);
    return (ex - 1/ex) / 2;
}

double cosh(double x) {
    double ex = exp(x);
    return (ex + 1/ex) / 2;
}

double tanh(double x) {
    double s = sinh(x);
    double c = cosh(x);
    return c != 0 ? s / c : 0.0;
}

// Ceiling and floor
double ceil(double x) {
    double int_part;
    double frac_part = modf(x, &int_part);
    if (frac_part > 0) return int_part + 1;
    return int_part;
}

double floor(double x) {
    double int_part;
    (void)modf(x, &int_part);
    return int_part;
}

double round(double x) {
    return x >= 0 ? floor(x + 0.5) : ceil(x - 0.5);
}

// Modulo and remainder
double fmod(double x, double y) {
    if (y == 0) return 0;
    return x - y * floor(x / y);
}

double modf(double x, double* intpart) {
    if (intpart == NULL) return x;
    
    *intpart = (double)(long long)x;
    return x - *intpart;
}

// Special functions
double gamma(double x) {
    // Stirling's approximation
    if (x <= 0) return 0;
    
    return sqrt(2 * PI / x) * pow((x / E), x);
}

// Constants
double M_PI(void) { return PI; }
double M_E(void) { return E; }

/**
 * @file math.h
 * @brief Mathematical functions header
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
double floor(double extravagantly x);
double round(double x);

// Modulo
double fmod(double x, double y);
double modf(double x, double* intpart);

#endif // MATH_HJB
