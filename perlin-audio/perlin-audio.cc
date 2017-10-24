#include <libnoise/module/perlin.h>

#include <cassert>
#include <cmath>
#include <cstdio>

#define PI 3.141592653589793

class Beat {
 public:
  Beat() {
    noise_.SetOctaveCount(1);
    noise_.SetFrequency(1.0);
    noise_.SetNoiseQuality(::noise::QUALITY_BEST);
    noise_.SetSeed(SEED);
  }

  float GetFactor(double time) {
    float factor = (1.0 + noise_.GetValue(0.0, 0.0, BEAT_FREQ * time)) / 2.0;
    if (factor < 0.0f) {
      factor = 0.0f;
    }
    return factor;
  }

 private:
  ::noise::module::Perlin noise_;
};

class VaryVolume {
 public:
  VaryVolume() {
    noise_.SetOctaveCount(1);
    noise_.SetFrequency(1.0);
    noise_.SetNoiseQuality(::noise::QUALITY_BEST);
    noise_.SetSeed(SEED + 1);
  }

  float GetFactor(double time) {
    float factor = (1.0 + noise_.GetValue(0.0, 0.0, VOLUME_CHANGE_FREQ * time)) / 2.0;
    factor = 0.3 + 0.7 * factor;
    if (factor < 0.25f) {
      factor = 0.25f;
    }
    return factor;
  }

 private:
  ::noise::module::Perlin noise_;
};

class Note {
 public:
  Note() {
    noise_.SetOctaveCount(6);
    noise_.SetFrequency(1.0);
    noise_.SetNoiseQuality(::noise::QUALITY_BEST);
    noise_.SetSeed(SEED + 2);
  }

 double GetFrequency(double time) {
   double x = (1.0 + noise_.GetValue(0.0, 0.0, 0.25 * time)) / 2.0;
   return exp2(6.0 + 7.0 * x);
 }

 private:
  ::noise::module::Perlin noise_;
};

class Harmonics {
 public:
  Harmonics() {
    noise_.SetOctaveCount(1);
    noise_.SetFrequency(1.0);
    noise_.SetNoiseQuality(::noise::QUALITY_BEST);
    noise_.SetSeed(SEED + 3);
  }

  void GetFactors(double time, double* factors) {
    factors[0] = 1.0;
    for (int n = 1; n < HARMONICS; ++n) {
      double x = (1.0 + noise_.GetValue(0.01 * n, 0.0, 0.25 * time)) / 2.0;
      if (x < 0.1) {
        x = 0.1;
      }
      factors[n] = factors[n-1] * x;
    }
  }

 private:
  ::noise::module::Perlin noise_;
};

int main() {
  Beat beat;
  VaryVolume vary_volume;
  Note note;
  Harmonics harmonics;

  double harmonic_factors[HARMONICS];

  double pos = 0.0;
  for (int64_t n = 0; n >= 0; ++n) {
    double time = (double)n / SAMPLE_RATE;
    float sample = 0.0f;

    pos += note.GetFrequency(time) * 2.0 * PI / SAMPLE_RATE;

    harmonics.GetFactors(time, harmonic_factors);
    for (int harmonic = 1; harmonic <= HARMONICS; ++harmonic) {
      sample += sin(harmonic*pos) * harmonic_factors[harmonic - 1];
    }

    sample *= beat.GetFactor(time) * vary_volume.GetFactor(time);

    size_t written = fwrite(&sample, sizeof(sample), 1, stdout);
    assert(written == 1);
  }
}
