#include <libnoise/module/perlin.h>

#include <cassert>
#include <cinttypes>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>

#define PI 3.141592653589793

#define ASPECT_RATIO ((float)(WIDTH)/(float)(HEIGHT))
#define VALUES_PER_PIXEL \
    (OVERSAMPLE_SPATIAL * OVERSAMPLE_SPATIAL * OVERSAMPLE_TEMPORAL)
#define FRAME_LENGTH (3 * WIDTH * HEIGHT)

struct Hsl {
  float hue;
  float saturation;
  float lightness;
};

struct Rgb {
  float red;
  float green;
  float blue;

  static uint8_t ToUint8(float subpixel) {
    subpixel *= 256.0f;
    if (subpixel < 0.0f) {
      return 0;
    } else if (subpixel > 255.0f) {
      return 255;
    } else {
      return (uint8_t)subpixel;
    }
  }
};

Rgb HslToRgb(Hsl hsl) {
  // Based on https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSL.
  float h = fmod(hsl.hue, PI * 2.0f);
  while (h < 0.0f) {
    h += PI * 2.0f;
  }
  float l =
      hsl.lightness > 1.0f ? 1.0f :
      hsl.lightness < 0.0f ? 0.0f :
      hsl.lightness;
  float chroma = (1.0f - fabs(2.0f * l - 1.0)) * hsl.saturation;
  float h_prime = 3.0f * h / PI;
  float x = chroma * (1.0f - fabs(fmod(h_prime, 2.0f) - 1.0f));
  float r1, g1, b1;
  if (h_prime < 0.0f) {
    assert(false);
  } else if (h_prime < 1.0f) {
    r1 = chroma;
    g1 = x;
    b1 = 0.0f;
  } else if (h_prime < 2.0f) {
    r1 = x;
    g1 = chroma;
    b1 = 0.0f;
  } else if (h_prime < 3.0f) {
    r1 = 0.0f;
    g1 = chroma;
    b1 = x;
  } else if (h_prime < 4.0f) {
    r1 = 0.0f;
    g1 = x;
    b1 = chroma;
  } else if (h_prime < 5.0f) {
    r1 = x;
    g1 = 0.0f;
    b1 = chroma;
  } else if (h_prime < 6.0f) {
    r1 = chroma;
    g1 = 0.0f;
    b1 = x;
  } else {
    assert(false);
  }
  float m = l - (r1 + g1 + b1) / 3.0f;
  return {r1 + m, g1 + m, b1 + m};
}

class ContourLines {
 public:
  ContourLines() {
    perlin_.SetOctaveCount(1);
    perlin_.SetFrequency(1.0);
    perlin_.SetNoiseQuality(::noise::QUALITY_BEST);
    perlin_.SetSeed(SEED);
  }

  // Fill a buffer of length FRAME_LENGTH with data for one frame.
  void FillFrame(int frame_number, uint8_t *buf) {
    int buf_len = 0;
    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        Rgb values[VALUES_PER_PIXEL];
        int value_count = 0;
        for (int sub_x = 0; sub_x < OVERSAMPLE_SPATIAL; ++sub_x) {
          float real_x = x + (float)sub_x / OVERSAMPLE_SPATIAL;
          real_x = 2.0f * real_x / WIDTH - 1.0f;
          for (int sub_y = 0; sub_y < OVERSAMPLE_SPATIAL; ++sub_y) {
            float real_y = y + (float)sub_y / OVERSAMPLE_SPATIAL;
            real_y = 2.0f * real_y / HEIGHT - 1.0f;
            for (int sub_frame = 0;
                sub_frame < OVERSAMPLE_TEMPORAL;
                ++sub_frame) {
              double real_t =
                  (frame_number + (double)sub_frame / OVERSAMPLE_TEMPORAL) /
                  FRAMERATE;
              assert(value_count < VALUES_PER_PIXEL);
              values[value_count++] = PointValue(real_x, real_y, real_t);
            }
          }
        }
        assert(value_count == VALUES_PER_PIXEL);
        Rgb pixel = {0.0f, 0.0f, 0.0f};
        for (int i = 0; i < value_count; ++i) {
          pixel.red += values[i].red;
          pixel.green += values[i].green;
          pixel.blue += values[i].blue;
        }
        pixel.red /= value_count;
        pixel.green /= value_count;
        pixel.blue /= value_count;
        buf[buf_len++] = Rgb::ToUint8(pixel.red);
        buf[buf_len++] = Rgb::ToUint8(pixel.green);
        buf[buf_len++] = Rgb::ToUint8(pixel.blue);
      }
    }
    assert(buf_len == FRAME_LENGTH);
  }

 private:
  // Get the value at a single point. (x, y) are spatial coordinates in the range
  // [-1, 1]. t is the number of seconds into the video.
  Rgb PointValue(float x, float y, double t) {
    if (ASPECT_RATIO > 1.0f) {
      x *= ASPECT_RATIO;
    } else {
      y /= ASPECT_RATIO;
    }
    double elevation = 0.5 * perlin_.GetValue(x * 1.7, y * 1.7, t * 0.03) + 0.5;
    if (elevation < 0.0) {
      elevation = 0.0;
    }
    double line_elevation = (floor(num_lines_ * elevation) + 0.5) / num_lines_;
    double distance_to_line = fabs(elevation - line_elevation);
    return HslToRgb(
        {(float)(line_elevation * 2.0 * PI),
         1.0f,
         (float)pow(256.0, -2.0 * num_lines_ * distance_to_line)});
  }

  ::noise::module::Perlin perlin_;
  int num_lines_ = 7;
};

class FrameBuffer {
 public:
  FrameBuffer() {
    thread_count_ = ::std::thread::hardware_concurrency();
    if (thread_count_ < 1) {
      thread_count_ = 1;
    }
    frame_count_ = 2 * thread_count_;

    for (unsigned i = 0; i < frame_count_; ++i) {
      frames_.push_back(::std::make_unique<uint8_t[]>(FRAME_LENGTH));
      frames_done_.push_back(false);
    }

    next_frame_to_output_ = 0;
    next_frame_to_generate_ = 0;
  }

  void GenerateAndPrint() {
    ::std::vector<::std::thread> generate_threads;
    for (unsigned i = 0; i < thread_count_; ++i) {
      generate_threads.emplace_back(&FrameBuffer::FillFrames, ::std::ref(*this));
    }

    OutputFrames();

    for (::std::thread& thread : generate_threads) {
      thread.join();
    }
  }

 private:
  void FillFrames() {
    ContourLines contour_lines;

    while (true) {
      int frame_number;
      uint8_t *frame_buf;
      {
        ::std::unique_lock<::std::mutex> l(m_);
        while (next_frame_to_generate_ - next_frame_to_output_ >=
            (int)frame_count_) {
          if (next_frame_to_generate_ >= FRAME_COUNT) {
            return;
          }
          next_frame_ready_to_generate_.wait(l);
        }
        if (next_frame_to_generate_ >= FRAME_COUNT) {
          return;
        }
        frame_number = next_frame_to_generate_++;
        frame_buf = frames_[frame_number % frame_count_].get();
        assert(!frames_done_[frame_number % frame_count_]);
      }

      contour_lines.FillFrame(frame_number, frame_buf);

      {
        ::std::unique_lock<::std::mutex> l(m_);
        frames_done_[frame_number % frame_count_] = true;
        next_frame_ready_to_output_.notify_one();
      }
    }
  }

  void OutputFrames() {
    while (true) {
      int frame_number;
      uint8_t *frame_buf;
      {
        ::std::unique_lock<::std::mutex> l(m_);
        if (next_frame_to_output_ >= FRAME_COUNT) {
          return;
        }
        while (!frames_done_[next_frame_to_output_ % frame_count_]) {
          next_frame_ready_to_output_.wait(l);
        }
        frame_number = next_frame_to_output_;
        frame_buf = frames_[frame_number % frame_count_].get();
      }

      size_t written = fwrite(frame_buf, 1, FRAME_LENGTH, stdout);
      assert(written == FRAME_LENGTH);

      {
        ::std::unique_lock<::std::mutex> l(m_);
        frames_done_[next_frame_to_output_++ % frame_count_] = false;
        next_frame_ready_to_generate_.notify_one();
      }
    }
  }

  // Number of FillFrames threads to run at a time, and number of frames to
  // keep in memory at a time.
  unsigned thread_count_;
  unsigned frame_count_;

  // Everything below is guarded by this mutex.
  ::std::mutex m_;

  // Parallel arrays of frame buffers, and whether they're ready to be written
  // to stdout.
  ::std::vector<::std::unique_ptr<uint8_t[]>> frames_;
  ::std::vector<bool> frames_done_;

  int next_frame_to_output_;
  ::std::condition_variable next_frame_ready_to_output_;

  int next_frame_to_generate_;
  ::std::condition_variable next_frame_ready_to_generate_;
};

int main() {
  FrameBuffer fb;
  fb.GenerateAndPrint();
  return 0;
}
