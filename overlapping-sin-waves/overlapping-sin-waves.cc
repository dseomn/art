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
      volume_dist_(0.0f, 1.0f) {}

  float GetSample(double t) {
    if (t > stop_) {
      start_ = t;
      stop_ = start_ + duration_dist_(random_);
      if (frequency_ > 0.0) {
        frequency_ = -1.0;
      } else {
        frequency_ = 20.0 + 256.0 * frequency_dist_(random_);
      }
      volume_ = volume_dist_(random_);
    }

    if (frequency_ <= 0.0) {
      return 0.0f;
    }

    float sample = sin((t - start_) * frequency_ * 2.0 * PI);

    sample *= volume_;

    if (t - start_ < 0.01) {
      sample *= (t - start_) / 0.01;
    }
    if (stop_ - t < 0.01) {
      sample *= (stop_ -t) / 0.01;
    }

    return sample;
  }

 private:
  double start_ = -1.0;
  double stop_ = -1.0;
  double frequency_ = -1.0;
  float volume_ = 0.0f;

  ::std::mt19937 random_;
  ::std::lognormal_distribution<double> duration_dist_;
  ::std::lognormal_distribution<double> frequency_dist_;
  ::std::uniform_real_distribution<float> volume_dist_;
};

int main() {
  ::std::vector<Note> notes;
  for (int i = 0; i < 10; ++i) {
    notes.emplace_back(SEED + i);
  }

  for (int64_t n = 0; n >= 0; ++n) {
    double time = (double)n / SAMPLE_RATE;
    float sample = 0.0f;

    for (Note& note : notes) {
      sample += note.GetSample(time) / notes.size();
    }

    size_t written = fwrite(&sample, sizeof(sample), 1, stdout);
    assert(written == 1);
  }
}
