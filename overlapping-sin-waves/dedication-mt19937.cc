#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>
#include <random>

int main() {
  ::std::mt19937 random(SEED);
  ::std::uniform_real_distribution<float> white_noise(-1.0f, 1.0f);

  double last_nonzero_sample_t = ::std::numeric_limits<double>::infinity();

  for (int64_t n = 0; n >= 0; ++n) {
    double t = (double)n / SAMPLE_RATE;

    if (fmod(t, 1.0) == 0.0 && t - last_nonzero_sample_t > SILENCE_POST_MIN) {
      break;
    }

    float sample[2] = {white_noise(random), white_noise(random)};

    float factor = 1.0f;
    if (t < FADE_IN_DURATION) {
      factor = t / FADE_IN_DURATION;
    } else if (t >= FADE_OUT_START) {
      factor =
          erfc(
              FADE_OUT_ERF_OFFSET +
              (t - FADE_OUT_START) * FADE_OUT_ERF_FACTOR) /
          2.0;
    }
    sample[0] *= factor;
    sample[1] *= factor;

    if (t >= FADE_OUT_START &&
        (
            fabs(sample[0]) > SILENCE_THRESHOLD ||
            fabs(sample[1]) > SILENCE_THRESHOLD)) {
      last_nonzero_sample_t = t;
    }

    size_t written = fwrite(&sample, sizeof(sample[0]), 2, stdout);
    assert(written == 2);
  }
}
