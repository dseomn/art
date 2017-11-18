#include <video-scaffolding/video-generator.h>

#include <libnoise/module/cache.h>
#include <libnoise/module/perlin.h>

#include <cmath>

struct TurbulentCellsThreadState {
  ::noise::module::Perlin turbulence_dx;
  ::noise::module::Perlin turbulence_dy;
  ::noise::module::Perlin turbulence_dz;

  ::noise::module::Perlin cell_lightness;
  ::noise::module::Cache cell_lightness_cache;

  ::noise::module::Perlin hue;
};

struct TurbulentCellsTimeState {
  float hue;
};

class TurbulentCells :
    public VideoGenerator<TurbulentCellsThreadState, TurbulentCellsTimeState> {
 protected:
  Rgb PointValue(
      const TurbulentCellsThreadState* thread_state,
      const TurbulentCellsTimeState* time_state,
      float x, float y,
      double t)
      override {
    Hsl hsl;

    double cell_x = GetCellCenter(
        VIEWPORT_FACTOR * x,
        thread_state->turbulence_dx, TURBULENCE_POWER,
        x, y, t);
    double cell_y = GetCellCenter(
        VIEWPORT_FACTOR * y,
        thread_state->turbulence_dy, TURBULENCE_POWER,
        x, y, t);
    double cell_z = GetCellCenter(
        CELL_SPEED * t,
        thread_state->turbulence_dz, TURBULENCE_POWER_T,
        x, y, t);

    hsl.lightness =
        (float)LIGHTNESS_MIDPOINT +
        (float)LIGHTNESS_DELTA *
        (float)thread_state->cell_lightness_cache.GetValue(
            cell_x, cell_y, cell_z);
    if (hsl.lightness < 0.0f) hsl.lightness = 0.0f;
    if (hsl.lightness > 1.0f) hsl.lightness = 1.0f;

    hsl.hue = time_state->hue;

    hsl.saturation = 1.0f;

    return HslToRgb(hsl);
  }

  ::std::unique_ptr<TurbulentCellsThreadState> GetThreadState() override {
    auto state = ::std::make_unique<TurbulentCellsThreadState>();

    int seed = SEED;

    for (::noise::module::Perlin* turbulence :
        {&state->turbulence_dx, &state->turbulence_dy, &state->turbulence_dz}) {
      turbulence->SetOctaveCount(TURBULENCE_ROUGHNESS);
      turbulence->SetFrequency(TURBULENCE_FREQUENCY);
      turbulence->SetSeed(seed++);
    }

    state->cell_lightness.SetOctaveCount(1);
    state->cell_lightness.SetFrequency(1.0);
    state->cell_lightness.SetSeed(seed++);
    state->cell_lightness_cache.SetSourceModule(0, state->cell_lightness);

    state->hue.SetOctaveCount(1);
    state->hue.SetFrequency(1.0);
    state->hue.SetNoiseQuality(::noise::QUALITY_BEST);
    state->hue.SetSeed(seed++);

    return state;
  }

  ::std::unique_ptr<TurbulentCellsTimeState> GetTimeState(
      const TurbulentCellsThreadState* thread_state, double t) {
    auto state = ::std::make_unique<TurbulentCellsTimeState>();

    state->hue =
        30.0f * (float)thread_state->hue.GetValue(0.0, 0.0, HUE_SPEED * t);

    return state;
  }

 private:
  double GetCellCenter(
      double initial,
      const ::noise::module::Perlin& turbulence, double power,
      float x, float y, double t) {
    double offset = power * turbulence.GetValue(
        VIEWPORT_FACTOR * x, VIEWPORT_FACTOR * y, CELL_SPEED * t);
    return 0.5 + floor(initial + offset);
  }
};

::std::unique_ptr<VideoGeneratorInterface>
VideoGeneratorInterface::MakeInstance() {
  return ::std::make_unique<TurbulentCells>();
}
