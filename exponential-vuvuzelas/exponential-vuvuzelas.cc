#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

constexpr int kSampleRate = 44100;
constexpr int kInputChannels = 1;
constexpr int kOutputChannels = 2;

// How many samples to wait before starting each new vuvuzela.
constexpr int kStartDelta = 53;

// How many samples to wait between ending one vuvuzela and replacing it with
// another.
constexpr int kInterDelta = 22050;

// Quantization for how much one output channel is emphasized relative to the
// others, per vuvuzela.
constexpr int kChannelEmphasisSteps = 11;

// The first output track will have 1 vuvuzela. The second to last will have
// 2**kMaxExponent vuvuzelas. The last will be a fade-out track.
constexpr int kMaxExponent = 10;

// The length of each track, except the fade-out at the end.
constexpr int kTrackSamples = kSampleRate * 120;

class ExponentialVuvuzelas {
 public:
  void LoadInputAudio() {
    const char* const files[] = {
      "vuvuzela-1.f32le",
      "vuvuzela-2.f32le",
      "vuvuzela-3.f32le",
      "vuvuzela-4.f32le",
    };
    for (const char* file : files) {
      int ret;

      FILE* fh = fopen(file, "rb");
      assert(fh != nullptr);

      ret = fseek(fh, 0L, SEEK_END);
      assert(ret == 0);
      long int length = ftell(fh);
      assert(length >= 0L);
      assert(length % (sizeof(float) * kInputChannels) == 0);
      rewind(fh);

      auto raw_audio = ::std::make_unique<float[]>(length / sizeof(float));
      size_t read_count =
          fread(raw_audio.get(), sizeof(float) * kInputChannels,
                length / (sizeof(float) * kInputChannels), fh);
      assert(read_count == length / (sizeof(float) * kInputChannels));

      input_audio_.emplace_back(read_count, ::std::move(raw_audio));

      ret = fclose(fh);
      assert(ret == 0);
    }
  }

  // Increment the exponent, for the next track. Make sure to call this at
  // least once before calling Advance().
  void IncrementExponent() {
    ++exponent_;
    assert(exponent_ <= kMaxExponent);

    int silent_samples = 0;
    while (in_play_.size() < 1ULL << exponent_) {
      float mix[kOutputChannels];
      const float emphasized_channel_min = 1.0f / kOutputChannels;
      for (int channel = 0; channel < kOutputChannels; ++channel) {
        const float emphasized_channel =
            emphasized_channel_min +
            (1.0f - emphasized_channel_min) *
            (in_play_.size() % kChannelEmphasisSteps) /
            (kChannelEmphasisSteps - 1.0f);
        if (in_play_.size() % kOutputChannels == (size_t)channel) {
          mix[channel] = emphasized_channel;
        } else {
          mix[channel] = (1.0f - emphasized_channel) / (kOutputChannels - 1);
        }
      }

      Vuvuzela v;
      v.input = -1;
      v.samples_remaining = silent_samples;
      for (int i = 0; i < kOutputChannels; ++i) {
        v.mix[i] = mix[i];
      }
      in_play_.push_back(v);

      silent_samples += kStartDelta;
    }
  }

  // Advance by the given number of samples, and return the generated audio.
  // Pass -1 to disable re-creating vuvuzelas as they finish, and advance until
  // all existing ones finish.
  ::std::vector<float> Advance(int samples) {
    ::std::vector<float> audio;

    for (int sample_number = 0;
         samples < 0 || sample_number < samples;
         ++sample_number) {
      bool anything_in_play = false;
      float sample[kOutputChannels] = {};

      for (Vuvuzela& vuvuzela : in_play_) {
        // Re-fill the audio/silence if needed.
        while (vuvuzela.samples_remaining <= 0 && samples >= 0) {
          if (vuvuzela.input < 0) {
            // The silence ended, fill with sound again.
            vuvuzela.input = next_input_++ % input_audio_.size();
            vuvuzela.samples_remaining = input_audio_[vuvuzela.input].first;
          } else {
            // The sound ended, fill with a silence buffer.
            vuvuzela.input = -1;
            vuvuzela.samples_remaining = kInterDelta;
          }
        }

        if (vuvuzela.samples_remaining <= 0) {
          continue;
        }

        anything_in_play = true;

        if (vuvuzela.input < 0) {
          --vuvuzela.samples_remaining;
          continue;
        }

        assert(kInputChannels == 1);
        float input_sample = input_audio_[vuvuzela.input].second[
            input_audio_[vuvuzela.input].first - vuvuzela.samples_remaining];
        for (int channel = 0; channel < kOutputChannels; ++channel) {
          sample[channel] += vuvuzela.mix[channel] * input_sample;
        }
        --vuvuzela.samples_remaining;
      }

      if (anything_in_play) {
        for (const float& s : sample) {
          audio.push_back(s * GetGainFactor((float)sample_number / samples));
        }
      } else {
        break;
      }
    }

    return audio;
  }

 private:
  // Get a multiplier for all output channels, given a progress in [0, 1)
  // within the track. This is based on a model of linearly increasing distance
  // from the virtual audio sources.
  float GetGainFactor(float progress) {
    constexpr float kInitialDistance = 1.0f;
    constexpr float kDistancePerExponent = 1.0f;

    // Values when the sum of all mixes is maximal.
    constexpr float factor_at_max_sum = 1.0f / (1ULL << kMaxExponent);
    constexpr float distance_at_max_sum =
        kMaxExponent * kDistancePerExponent + kInitialDistance;

    // Additional factor for computations, to ensure that the factor at the end
    // is factor_at_max_sum.
    constexpr float normalization_factor =
        factor_at_max_sum * distance_at_max_sum * distance_at_max_sum;

    if (exponent_ == kMaxExponent) {
      return factor_at_max_sum;
    }

    float distance =
        (exponent_ + progress) * kDistancePerExponent + kInitialDistance;
    assert(distance > 0.0f);
    assert(distance <= distance_at_max_sum);

    float factor = normalization_factor / (distance * distance);
    assert(factor > 0.0f);
    assert(factor <= 1.0f);

    return factor;
  }

  // (number of samples, raw data)
  ::std::vector<::std::pair<int, ::std::unique_ptr<float[]>>> input_audio_;

  struct Vuvuzela {
    int input;  // Index into input_audio_, or -1 for silence.
    int samples_remaining;
    float mix[kOutputChannels];
  };
  ::std::vector<Vuvuzela> in_play_;

  int exponent_ = -1;

  int next_input_ = 0;
};

void WriteAudio(
    const ::std::string& filename, const ::std::vector<float>& audio) {
  FILE* fh = fopen(filename.c_str(), "wb");
  assert(fh != nullptr);
  for (const float& sample : audio) {
    size_t written = fwrite(&sample, sizeof(sample), 1, fh);
    assert(written == 1);
  }
  int ret = fclose(fh);
  assert(ret == 0);
}

int main() {
  ExponentialVuvuzelas exponential_vuvuzelas;
  exponential_vuvuzelas.LoadInputAudio();

  for (int exponent = 0; exponent <= kMaxExponent; ++exponent) {
    exponential_vuvuzelas.IncrementExponent();
    WriteAudio(
        ::std::string("exponent-") + ::std::to_string(exponent) + ".f32le",
        exponential_vuvuzelas.Advance(kTrackSamples));
  }

  WriteAudio("fade-out.f32le", exponential_vuvuzelas.Advance(-1));

  return 0;
}
