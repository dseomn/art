#include <video-scaffolding/video-generator.h>

#include <libnoise/module/perlin.h>
#include <libnoise/module/turbulence.h>
#include <libnoise/module/voronoi.h>

struct TurbulentCellsThreadState {
  ::noise::module::Voronoi cells;
  ::noise::module::Turbulence cells_turb;

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

    hsl.lightness =
        (float)LIGHTNESS_MIDPOINT +
        (float)LIGHTNESS_DELTA *
        (float)thread_state->cells_turb.GetValue(
            VIEWPORT_FACTOR * x, VIEWPORT_FACTOR * y, CELL_SPEED * t);
    if (hsl.lightness < 0.0f) hsl.lightness = 0.0f;
    if (hsl.lightness > 1.0f) hsl.lightness = 1.0f;

    hsl.hue = time_state->hue;

    hsl.saturation = 1.0f;

    return HslToRgb(hsl);
  }

  ::std::unique_ptr<TurbulentCellsThreadState> GetThreadState() override {
    auto state = ::std::make_unique<TurbulentCellsThreadState>();

    int seed = SEED;

    state->cells.SetFrequency(1.0);
    state->cells.SetDisplacement(1.0);
    state->cells.SetSeed(seed++);
    state->cells_turb.SetSourceModule(0, state->cells);
    state->cells_turb.SetRoughness(TURBULENCE_ROUGHNESS);
    state->cells_turb.SetFrequency(TURBULENCE_FREQUENCY);
    state->cells_turb.SetPower(TURBULENCE_POWER);

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
};

::std::unique_ptr<VideoGeneratorInterface>
VideoGeneratorInterface::MakeInstance() {
  return ::std::make_unique<TurbulentCells>();
}
