#include "pixel.h"

#include <cassert>
#include <cmath>

#define PI 3.141592653589793

Rgb operator+(Rgb a, Rgb b) {
  return {a.red + b.red, a.green + b.green, a.blue + b.blue};
}

Rgb operator*(Rgb a, float b) {
  return {a.red * b, a.green * b, a.blue * b};
}

Rgb operator*(float a, Rgb b) {
  return b * a;
}

uint8_t SubpixelToUint8(float subpixel) {
  subpixel *= 256.0f;
  if (subpixel < 0.0f) {
    return 0;
  } else if (subpixel > 255.0f) {
    return 255;
  } else {
    return (uint8_t)subpixel;
  }
}

Rgb HslToRgb(Hsl hsl) {
  // Based on https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSL.
  float h = fmod(hsl.hue, PI * 2.0f);
  while (h < 0.0f) {
    h += PI * 2.0f;
  }
  float l =
      hsl.lightness > 1.0f ? 1.0f :
      hsl.lightness < 0.0f ? 0.0f :
      hsl.lightness;
  float chroma = (1.0f - fabs(2.0f * l - 1.0)) * hsl.saturation;
  float h_prime = 3.0f * h / PI;
  float x = chroma * (1.0f - fabs(fmod(h_prime, 2.0f) - 1.0f));
  float r1, g1, b1;
  if (h_prime < 0.0f) {
    assert(false);
  } else if (h_prime < 1.0f) {
    r1 = chroma;
    g1 = x;
    b1 = 0.0f;
  } else if (h_prime < 2.0f) {
    r1 = x;
    g1 = chroma;
    b1 = 0.0f;
  } else if (h_prime < 3.0f) {
    r1 = 0.0f;
    g1 = chroma;
    b1 = x;
  } else if (h_prime < 4.0f) {
    r1 = 0.0f;
    g1 = x;
    b1 = chroma;
  } else if (h_prime < 5.0f) {
    r1 = x;
    g1 = 0.0f;
    b1 = chroma;
  } else if (h_prime <= 6.0f) {
    r1 = chroma;
    g1 = 0.0f;
    b1 = x;
  } else {
    assert(false);
  }
  float m = l - (r1 + g1 + b1) / 3.0f;
  return {r1 + m, g1 + m, b1 + m};
}
