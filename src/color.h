#pragma once

#include <stdint.h>

typedef struct {
    double r;
    double g;
    double b;
} rgb_t;

typedef struct {
    double h;
    double s;
    double v;
} hsv_t;

hsv_t rgb2hsv(rgb_t in);
rgb_t hsv2rgb(hsv_t in);
