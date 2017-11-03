#include <libnoise/module/perlin.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

#define PI 3.141592653589793

#define ASPECT_RATIO ((float)(WIDTH)/(float)(HEIGHT))

float GetFactor(double d, double period) {
  float x = fmod(fabs(d), period) / period;
  if (x < 0.0f) {
    assert(false);
  } else if (x < 1.0f/8.0f) {
    return 1.0f;
  } else if (x < 3.0f/8.0f) {
    return 1.0f - 4.0f * (x - 1.0f/8.0f);
  } else if (x < 5.0f/8.0f) {
    return 0.0f;
  } else if (x < 7.0f/8.0f) {
    return 4.0f * (x - 5.0f/8.0f);
  } else if (x <= 1.0f) {
    return 1.0f;
  } else {
    assert(false);
  }
}

int main() {
  ::noise::module::Perlin perlin;
  perlin.SetOctaveCount(1);
  perlin.SetFrequency(1.0);
  perlin.SetNoiseQuality(::noise::QUALITY_BEST);
  perlin.SetSeed(SEED);

  for (int frame_number = 0; frame_number < FRAME_COUNT; ++frame_number) {
    double real_t = (double)frame_number / FRAMERATE;
    real_t = 1.0 + 0.05 * real_t;

    double unit_x[FACTOR_COUNT];
    double unit_y[FACTOR_COUNT];
    double period[FACTOR_COUNT];
    for (int factor_number = 0; factor_number < FACTOR_COUNT; ++factor_number) {
      double theta = PI * perlin.GetValue(factor_number, 0.0, real_t);
      unit_x[factor_number] = cos(theta);
      unit_y[factor_number] = sin(theta);

      period[factor_number] = 0.5 * perlin.GetValue(factor_number, 1.0, real_t) + 0.5;
      period[factor_number] = ::std::max(0.01, period[factor_number]);

      //fprintf(stderr, "%06d[%02d]: (%lf, %lf) / %lf\n", frame_number, factor_number, unit_x[factor_number], unit_y[factor_number], period[factor_number]);
    }

    for (int y = 0; y < HEIGHT; ++y) {
      float real_y = (2.0f * y) / HEIGHT - 1.0f;
      if (ASPECT_RATIO < 1.0f) {
        real_y /= ASPECT_RATIO;
      }

      for (int x = 0; x < WIDTH; ++x) {
        float real_x = (2.0f * x) / HEIGHT - 1.0f;
        if (ASPECT_RATIO > 1.0f) {
          real_x *= ASPECT_RATIO;
        }

        float luma = 1.0f;
        for (int factor_number = 0; factor_number < FACTOR_COUNT; ++factor_number) {
          double d = real_x * unit_x[factor_number] + real_y * unit_y[factor_number];
          luma *= GetFactor(d, period[factor_number]);
        }

        float pixel = luma * 256.0f;
        if (pixel < 0.0f) {
          putchar(0);
        } else if (pixel > 255.0f) {
          putchar(255);
        } else {
          putchar((int)pixel);
        }
      }
    }
  }

  return 0;
}
