#include <video-scaffolding/video-generator.h>

#include <libnoise/module/perlin.h>

#define PI 3.141592653589793

struct ContourLinesThreadState {
  ::noise::module::Perlin perlin;
};

class ContourLines : public VideoGenerator<ContourLinesThreadState> {
 protected:
  Rgb PointValue(
      const ContourLinesThreadState* thread_state, const NullState* time_state,
      float x, float y, double t)
      override {
    double elevation =
        0.5 * thread_state->perlin.GetValue(x * 1.7, y * 1.7, t * 0.03) + 0.5;
    if (elevation < 0.0) {
      elevation = 0.0;
    }
    double line_elevation = (floor(NUM_LINES * elevation) + 0.5) / NUM_LINES;
    double distance_to_line = fabs(elevation - line_elevation);
    return HslToRgb(
        {(float)(line_elevation * 2.0 * PI),
         1.0f,
         (float)pow(256.0, -2.0 * NUM_LINES * distance_to_line)});
  }

  ::std::unique_ptr<ContourLinesThreadState> GetThreadState() override {
    auto state = ::std::make_unique<ContourLinesThreadState>();
    state->perlin.SetOctaveCount(1);
    state->perlin.SetFrequency(1.0);
    state->perlin.SetNoiseQuality(::noise::QUALITY_BEST);
    state->perlin.SetSeed(SEED);
    return state;
  }
};

::std::unique_ptr<VideoGeneratorInterface>
VideoGeneratorInterface::MakeInstance() {
  return ::std::make_unique<ContourLines>();
}
