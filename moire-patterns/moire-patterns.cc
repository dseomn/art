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
    for (int layer_num = 0;
         layer_num < LAYER_COUNT && alpha > 0.0f;
         ++layer_num) {
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
    auto state = ::std::make_unique<MoirePatternsTimeState>();

    int noise_track = 0;

    for (int layer_num = 0; layer_num < LAYER_COUNT; ++layer_num) {
      double theta =
          PI * GetNoiseValue(thread_state, noise_track++, ROTATION_SPEED, t);
      state->unit_x[layer_num] = cos(theta);
      state->unit_y[layer_num] = sin(theta);

      state->offset[layer_num] =
          GetNoiseValue(thread_state, noise_track++, TRANSLATION_SPEED, t);

      state->period[layer_num] =
          0.05 + 0.6 * pow(
              NoiseValueToRange(
                  0.0, 1.0,
                  GetNoiseValue(thread_state, noise_track++, SCALE_SPEED, t)),
              SCALE_EXP);
    }

    state->hue =
        10.0f * (float)PI *
        (float)GetNoiseValue(thread_state, noise_track++, HUE_SPEED, t);

    return state;
  }

 private:
  double GetNoiseValue(
      const MoirePatternsThreadState* thread_state,
      int noise_track,
      double speed,
      double t) {
    constexpr double kTrackSeparation = 9.45;

    double displacement =
        TIME_DISPLACEMENT_AMPLITUDE *
        thread_state->perlin.GetValue(
            2 * noise_track * kTrackSeparation,
            0.0,
            TIME_DISPLACEMENT_FREQ * t);

    return thread_state->perlin.GetValue(
        (2 * noise_track + 1) * kTrackSeparation,
        0.0,
        speed * t + displacement);
  }

  // Given a value returned by GetNoiseValue, return a value in the range
  // [lower, upper].
  double NoiseValueToRange(double lower, double upper, double val) {
    val = lower + (0.5 + 0.5 * val) * (upper - lower);
    return ::std::max(lower, ::std::min(upper, val));
  }

  float GetAlpha(double d, double offset, double period) {
    float x = fmod(fabs(d + offset), period) / period;
    if (x < 0.0f) {
      assert(false);
    } else if (x < 3.0f/16.0f) {
      return 1.0f;
    } else if (x < 5.0f/16.0f) {
      return 1.0f - 8.0f * (x - 3.0f/16.0f);
    } else if (x < 11.0f/16.0f) {
      return 0.0f;
    } else if (x < 13.0f/16.0f) {
      return 8.0f * (x - 11.0f/16.0f);
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
