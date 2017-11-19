#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

#define PI 3.141592653589793

class Note {
 public:
  Note(unsigned seed) :
      random_(seed),
      duration_dist_(0.0, 1.0),
      frequency_dist_(0.0, 1.0),
      volume_dist_(0.0f, 1.0f),
      pan_dist_(0.0f, 1.0f) {}

  void GetSample(double t, float out[2]) {
    if (t > stop_) {
      if (!respawn_) {
        out[0] = 0.0f;
        out[1] = 0.0f;
        return;
      }

      start_ = t;
      stop_ = start_ + duration_dist_(random_);
      if (stop_ > DURATION) {
        stop_ = DURATION;
      }

      if (frequency_ > 0.0) {
        frequency_ = -1.0;
      } else {
        frequency_ = 20.0 + 256.0 * frequency_dist_(random_);
      }

      volume_ = volume_dist_(random_);
      pan_ = pan_dist_(random_);
    }

    if (frequency_ <= 0.0) {
      out[0] = 0.0f;
      out[1] = 0.0f;
      return;
    }

    float sample = 0.0f;
    float harmonic_factor = 1.0f;
    for (int harmonic = 1; harmonic_factor >= HARMONIC_THRESHOLD; ++harmonic) {
      sample +=
          harmonic_factor *
          sin(harmonic * (t - start_) * frequency_ * 2.0 * PI);

      float spread_factor = 1.0f;
      for (int spread = 1; spread < FREQ_SPREAD_COUNT; ++spread) {
          spread_factor *= FREQ_SPREAD_FACTOR;
          sample +=
              harmonic_factor *
              sin(harmonic * (t - start_) * frequency_ * 2.0 * PI * spread_factor);
          sample +=
              harmonic_factor *
              sin(harmonic * (t - start_) * frequency_ * 2.0 * PI / spread_factor);
      }

      harmonic_factor *= HARMONIC_FACTOR;
    }

    sample *= volume_;

    if (t - start_ < 0.02) {
      sample *= (t - start_) / 0.02;
    }
    if (stop_ - t < 0.02) {
      sample *= (stop_ - t) / 0.02;
    }

    out[0] = sample * pan_;
    out[1] = sample * (1.0f - pan_);
    return;
  }

  void StopGracefully() {
    respawn_ = false;
  }

  bool IsInUse(double t) {
    return respawn_ || t <= stop_;
  }

 private:
  bool respawn_ = true;
  double start_ = -1.0;
  double stop_ = -1.0;
  double frequency_ = -1.0;
  float volume_ = 0.0f;
  float pan_ = 0.5f;

  ::std::mt19937 random_;
  ::std::lognormal_distribution<double> duration_dist_;
  ::std::lognormal_distribution<double> frequency_dist_;
  ::std::uniform_real_distribution<float> volume_dist_;
  ::std::uniform_real_distribution<float> pan_dist_;
};

int main() {
  ::std::mt19937 random(SEED);

  ::std::vector<Note> notes;
  double adjust_notes_at = -1.0;
  ::std::uniform_real_distribution<double> adjust_notes_dist(
      MIN_NOTE_ADJUST_INTERVAL, MAX_NOTE_ADJUST_INTERVAL);
  ::std::poisson_distribution<int> note_count_dist(MEAN_NOTE_COUNT);

  ::std::vector<float> audio;
  for (int64_t n = 0; n >= 0; ++n) {
    double time = (double)n / SAMPLE_RATE;
    if (time > DURATION) {
      break;
    }

    if (time >= adjust_notes_at) {
      int note_count = 0;
      for (Note& note : notes) {
        if (note.IsInUse(time)) {
          ++note_count;
        }
      }

      int note_count_next = note_count_dist(random);
      for (Note& note : notes) {
        if (note_count == note_count_next) {
          break;
        } else if (note_count > note_count_next) {
          if (note.IsInUse(time)) {
            note.StopGracefully();
            --note_count;
          }
        } else {
          if (!note.IsInUse(time)) {
            note = Note(random());
            ++note_count;
          }
        }
      }
      while (note_count < note_count_next) {
        notes.emplace_back(random());
        ++note_count;
      }

      adjust_notes_at = time + adjust_notes_dist(random);
    }

    float sample[2] = {0.0f, 0.0f};

    for (Note& note : notes) {
      float note_sample[2];
      note.GetSample(time, note_sample);
      sample[0] += note_sample[0];
      sample[1] += note_sample[1];
    }

    audio.push_back(sample[0]);
    audio.push_back(sample[1]);
  }

  float peak = 0.0f;
  for (float sample : audio) {
    peak = ::std::max(peak, ::std::abs(sample));
  }

  for (float sample : audio) {
    sample /= peak;
    size_t written = fwrite(&sample, sizeof(sample), 1, stdout);
    assert(written == 1);
  }
}
