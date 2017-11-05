#include <video-scaffolding/video-generator.h>

#include <libnoise/module/perlin.h>

#include <algorithm>
#include <cmath>

#define PI 3.141592653589793

struct MoirePatternsThreadState {
  ::noise::module::Perlin perlin;
};

struct MoirePatternsTimeState {
  double unit_x[FACTOR_COUNT];
  double unit_y[FACTOR_COUNT];
  double period[FACTOR_COUNT];
};

class MoirePatterns :
    public VideoGenerator<MoirePatternsThreadState, MoirePatternsTimeState> {
 protected:
  Rgb PointValue(
      const MoirePatternsThreadState* thread_state,
      const MoirePatternsTimeState* time_state,
      float x, float y, double t)
      override {
    float luma = 1.0f;
    for (int factor_number = 0; factor_number < FACTOR_COUNT; ++factor_number) {
      double d =
          x * time_state->unit_x[factor_number] +
          y * time_state->unit_y[factor_number];
      luma *= GetFactor(d, time_state->period[factor_number]);
    }
    return {luma, luma, luma};
  }

  ::std::unique_ptr<MoirePatternsThreadState> GetThreadState() override {
    auto state = ::std::make_unique<MoirePatternsThreadState>();

    state->perlin.SetOctaveCount(1);
    state->perlin.SetFrequency(1.0);
    state->perlin.SetNoiseQuality(::noise::QUALITY_BEST);
    state->perlin.SetSeed(SEED);

    return state;
  }

  ::std::unique_ptr<MoirePatternsTimeState> GetTimeState(
      const MoirePatternsThreadState* thread_state, double t) override {
    auto state = ::std::make_unique<MoirePatternsTimeState>();

    for (int factor_number = 0; factor_number < FACTOR_COUNT; ++factor_number) {
      double theta =
          PI * thread_state->perlin.GetValue(factor_number, 0.0, 0.05 * t);
      state->unit_x[factor_number] = cos(theta);
      state->unit_y[factor_number] = sin(theta);

      state->period[factor_number] =
          0.5 * thread_state->perlin.GetValue(factor_number, 1.0, 0.05 * t) +
          0.5;
      state->period[factor_number] =
          ::std::max(0.01, state->period[factor_number]);
    }

    return state;
  }

 private:
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

};

::std::unique_ptr<VideoGeneratorInterface>
VideoGeneratorInterface::MakeInstance() {
  return ::std::make_unique<MoirePatterns>();
}
