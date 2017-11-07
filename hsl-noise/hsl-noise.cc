#include <video-scaffolding/video-generator.h>

#include <libnoise/module/perlin.h>

struct HslNoiseThreadState {
  ::noise::module::Perlin hue;
  ::noise::module::Perlin saturation;
  ::noise::module::Perlin lightness;
};

class HslNoise : public VideoGenerator<HslNoiseThreadState> {
 protected:
  Rgb PointValue(
      const HslNoiseThreadState* thread_state, const NullState* time_state,
      float x, float y,
      double t)
      override {
    Hsl hsl;
    hsl.hue = 30.0f * (float)thread_state->hue.GetValue(0.5 * x, 0.5 * y, 0.03 * t);
    hsl.saturation = 0.5f + 0.5f * (float)thread_state->saturation.GetValue(2 * x, 2 * y, 0.05 * t);
    if (hsl.saturation < 0.0f) hsl.saturation = 0.0f;
    if (hsl.saturation > 1.0f) hsl.saturation = 1.0f;
    hsl.lightness = 0.5f + 0.5f * (float)thread_state->lightness.GetValue(4 * x, 4 * y, 0.06 * t);
    if (hsl.lightness < 0.0f) hsl.lightness = 0.0f;
    if (hsl.lightness > 1.0f) hsl.lightness = 1.0f;
    return HslToRgb(hsl);
  }

  ::std::unique_ptr<HslNoiseThreadState> GetThreadState() override {
    auto state = ::std::make_unique<HslNoiseThreadState>();

    int seed = 0;

    state->hue.SetOctaveCount(1);
    state->hue.SetFrequency(1.0);
    state->hue.SetNoiseQuality(::noise::QUALITY_BEST);
    state->hue.SetSeed(seed++);

    state->saturation.SetOctaveCount(1);
    state->saturation.SetFrequency(1.0);
    state->saturation.SetNoiseQuality(::noise::QUALITY_BEST);
    state->saturation.SetSeed(seed++);

    state->lightness.SetOctaveCount(5);
    state->lightness.SetLacunarity(1.5);
    state->lightness.SetPersistence(0.75);
    state->lightness.SetFrequency(1.0);
    state->lightness.SetNoiseQuality(::noise::QUALITY_BEST);
    state->lightness.SetSeed(seed++);

    return state;
  }
};

::std::unique_ptr<VideoGeneratorInterface>
VideoGeneratorInterface::MakeInstance() {
  return ::std::make_unique<HslNoise>();
}
