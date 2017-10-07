#include <libnoise/module/perlin.h>

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <cstdio>

#define PI 3.141592653589793

#define ASPECT_RATIO ((float)(WIDTH)/(float)(HEIGHT))
#define VALUES_PER_PIXEL \
    (OVERSAMPLE_SPATIAL * OVERSAMPLE_SPATIAL * OVERSAMPLE_TEMPORAL)

struct Hsl {
  float hue;
  float saturation;
  float lightness;
};

struct Rgb {
  float red;
  float green;
  float blue;

  void OutputSubpixel(float subpixel) {
    subpixel *= 256.0f;
    if (subpixel < 0.0f) {
      putchar(0);
    } else if (subpixel > 255.0f) {
      putchar(255);
    } else {
      putchar((uint8_t)subpixel);
    }
  }

  void Output() {
    OutputSubpixel(red);
    OutputSubpixel(green);
    OutputSubpixel(blue);
  }
};

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
  float x = chroma * (1.0f - fabs(fmod(h_prime, 2.0f - 1.0f)));
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
  } else if (h_prime < 6.0f) {
    r1 = chroma;
    g1 = 0.0f;
    b1 = x;
  } else {
    assert(false);
  }
  float m = l - (r1 + g1 + b1) / 3.0f;
  return {r1 + m, g1 + m, b1 + m};
}

class ContourLines {
 public:
  ContourLines() {
    perlin_.SetOctaveCount(1);
    perlin_.SetFrequency(1.0);
    perlin_.SetNoiseQuality(::noise::QUALITY_BEST);
    perlin_.SetSeed(0);  // Chosen by fair roll of a d1.
  }

  // Get the value at a single point. (x, y) are spatial coordinates in the range
  // [-1, 1]. t is the number of seconds into the video.
  Rgb PointValue(float x, float y, float t) {
    if (ASPECT_RATIO > 1.0f) {
      x *= ASPECT_RATIO;
    } else {
      y /= ASPECT_RATIO;
    }
    double elevation = 0.5 * perlin_.GetValue(x * 1.0, y * 1.0, t * 0.02) + 0.5;
    if (elevation < 0.0) {
      elevation = 0.0;
    }
    double line_elevation = (floor(num_lines_ * elevation) + 0.5) / num_lines_;
    double distance_to_line = fabs(elevation - line_elevation);
    return HslToRgb(
        {(float)(line_elevation * 2.0 * PI),
         1.0f,
         (float)pow(256.0, -2.0 * num_lines_ * distance_to_line)});
  }

 private:
  ::noise::module::Perlin perlin_;
  int num_lines_ = 7;
};

int main() {
  ContourLines contour_lines;

  for (int frame_number = 0; frame_number < FRAME_COUNT; ++frame_number) {
    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        Rgb values[VALUES_PER_PIXEL];
        int value_count = 0;
        for (int sub_x = 0; sub_x < OVERSAMPLE_SPATIAL; ++sub_x) {
          float real_x = x + (float)sub_x / OVERSAMPLE_SPATIAL;
          real_x = 2.0f * real_x / WIDTH - 1.0f;
          for (int sub_y = 0; sub_y < OVERSAMPLE_SPATIAL; ++sub_y) {
            float real_y = y + (float)sub_y / OVERSAMPLE_SPATIAL;
            real_y = 2.0f * real_y / HEIGHT - 1.0f;
            for (int sub_frame = 0;
                sub_frame < OVERSAMPLE_TEMPORAL;
                ++sub_frame) {
              float real_t =
                  (frame_number + (float)sub_frame / OVERSAMPLE_TEMPORAL) /
                  FRAMERATE;
              assert(value_count < VALUES_PER_PIXEL);
              values[value_count++] =
                  contour_lines.PointValue(real_x, real_y, real_t);
            }
          }
        }
        assert(value_count == VALUES_PER_PIXEL);
        Rgb pixel = {0.0f, 0.0f, 0.0f};
        for (int i = 0; i < value_count; ++i) {
          pixel.red += values[i].red;
          pixel.green += values[i].green;
          pixel.blue += values[i].blue;
        }
        pixel.red /= value_count;
        pixel.green /= value_count;
        pixel.blue /= value_count;
        pixel.Output();
      }
    }
  }

  return 0;
}
