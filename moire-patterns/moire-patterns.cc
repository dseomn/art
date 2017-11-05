#include <video-scaffolding/video-generator.h>

#include <libnoise/module/perlin.h>

#include <algorithm>
#include <cmath>

#define PI 3.141592653589793

struct MoirePatternsThreadState {
  ::noise::module::Perlin perlin;
};

struct MoirePatternsTimeState {
  double unit_x[LAYER_COUNT];
  double unit_y[LAYER_COUNT];
  double offset[LAYER_COUNT];
  double period[LAYER_COUNT];

  float hue;
};

class MoirePatterns :
    public VideoGenerator<MoirePatternsThreadState, MoirePatternsTimeState> {
 protected:
  Rgb PointValue(
      const MoirePatternsThreadState* thread_state,
      const MoirePatternsTimeState* time_state,
      float x, float y, double t)
      override {
    float alpha = 1.0f;
    for (int layer_num = 0; layer_num < LAYER_COUNT; ++layer_num) {
      double d =
          x * time_state->unit_x[layer_num] + y * time_state->unit_y[layer_num];
      alpha *= GetAlpha(
          d,
          time_state->offset[layer_num],
          time_state->period[layer_num]);
    }

    return HslToRgb({time_state->hue, 1.0f, alpha});
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
    constexpr double kSeparation = 9.1;

    auto state = ::std::make_unique<MoirePatternsTimeState>();

    for (int layer_num = 0; layer_num < LAYER_COUNT; ++layer_num) {
      double theta =
          PI * thread_state->perlin.GetValue(
              0.0, kSeparation * layer_num, 0.05 * t);
      state->unit_x[layer_num] = cos(theta);
      state->unit_y[layer_num] = sin(theta);

      state->offset[layer_num] = thread_state->perlin.GetValue(
          kSeparation, kSeparation * layer_num, 0.05 * t);

      state->period[layer_num] =
          ::std::max(
              0.01,
              0.3 + 0.3 * thread_state->perlin.GetValue(
                  2 * kSeparation, kSeparation * layer_num, 0.05 * t));
    }

    state->hue =
        10.0f * (float)PI *
        (float)thread_state->perlin.GetValue(3 * kSeparation, 0.0, 0.05 * t);

    return state;
  }

 private:
  float GetAlpha(double d, double offset, double period) {
    float x = fmod(fabs(d + offset), period) / period;
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
