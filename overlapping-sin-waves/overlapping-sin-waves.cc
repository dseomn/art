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
      start_ = t;
      stop_ = start_ + duration_dist_(random_);
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

    float sample = sin((t - start_) * frequency_ * 2.0 * PI);

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

 private:
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
  ::std::vector<Note> notes;
  for (int i = 0; i < 10; ++i) {
    notes.emplace_back(SEED + i);
  }

  for (int64_t n = 0; n >= 0; ++n) {
    double time = (double)n / SAMPLE_RATE;
    float sample[2] = {0.0f, 0.0f};

    for (Note& note : notes) {
      float note_sample[2];
      note.GetSample(time, note_sample);
      sample[0] += note_sample[0] / notes.size();
      sample[1] += note_sample[1] / notes.size();
    }

    size_t written = fwrite(&sample, sizeof(sample[0]), 2, stdout);
    assert(written == 2);
  }
}
