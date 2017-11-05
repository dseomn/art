#ifndef VIDEO_SCAFFOLDING_PIXEL_H
#define VIDEO_SCAFFOLDING_PIXEL_H

#include <cinttypes>

// Data types and utilities for working with pixels.

// HSL pixel. Hue is in radians, other values are nominally in the range [0,
// 1].
struct Hsl {
  float hue;
  float saturation;
  float lightness;
};

// RGB pixel, all values nominally in the range [0, 1].
struct Rgb {
  float red;
  float green;
  float blue;
};

Rgb operator+(Rgb a, Rgb b);

Rgb operator*(Rgb a, float b);

Rgb operator*(float a, Rgb b);

// Convert a nominally-in-[0,1] subpixel to uint8_t, clamping to the min and
// max values of a uint8_t if needed.
uint8_t SubpixelToUint8(float subpixel);

Rgb HslToRgb(Hsl hsl);

#endif
