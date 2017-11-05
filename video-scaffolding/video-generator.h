#ifndef VIDEO_SCAFFOLDING_VIDEO_GENERATOR_H
#define VIDEO_SCAFFOLDING_VIDEO_GENERATOR_H

// Video generation, pixel-by-pixel. See VideoGenerator::PointValue for the
// main thing to override when using this. Also, you must implement
// VideoGeneratorInterface::MakeInstance to return an instance of your
// subclass.
//
// The following compile-time constants must be defined externally:
//
// WIDTH
// HEIGHT
// OVERSAMPLE_SPATIAL
// OVERSAMPLE_TEMPORAL
// FRAMERATE
// FRAME_COUNT

#include "pixel.h"

#include <cassert>
#include <cinttypes>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <thread>
#include <vector>

class VideoGeneratorInterface {
 public:
  // Each project file must separately implement this to return an instance of
  // the correct subclass.
  static ::std::unique_ptr<VideoGeneratorInterface> MakeInstance();

  // Generate the video/image data, and write it to stdout.
  virtual void GenerateAndPrint() = 0;
};

namespace internal {

constexpr float kAspectRatio = (float)(WIDTH) / (float)(HEIGHT);
constexpr int kValuesPerPixel =
    OVERSAMPLE_SPATIAL * OVERSAMPLE_SPATIAL * OVERSAMPLE_TEMPORAL;
constexpr int kFrameLength = 3 * WIDTH * HEIGHT;

}

struct NullState {};

// Main class to generate a video (or still frame). Subclass this. Use
// ThreadState for any information local to a thread, and TimeState for
// anything local to a single point in time. (There could be multiple
// TimeStates per frame if temporal oversampling is used.)
template <typename ThreadState = NullState, typename TimeState = NullState>
class VideoGenerator : public VideoGeneratorInterface {
 public:
  VideoGenerator() {
    thread_count_ = ::std::thread::hardware_concurrency();
    if (thread_count_ < 1) {
      thread_count_ = 1;
    }
    frame_count_ = 2 * thread_count_;

    for (unsigned i = 0; i < frame_count_; ++i) {
      frames_.push_back(::std::make_unique<uint8_t[]>(internal::kFrameLength));
      frames_done_.push_back(false);
    }

    next_frame_to_output_ = 0;
    next_frame_to_generate_ = 0;
  }

  void GenerateAndPrint() override {
    ::std::vector<::std::thread> generate_threads;
    for (unsigned i = 0; i < thread_count_; ++i) {
      generate_threads.emplace_back(
          &VideoGenerator::FillFramesThread, ::std::ref(*this));
    }

    OutputFramesThread();

    for (::std::thread& thread : generate_threads) {
      thread.join();
    }
  }

 protected:
  // Get the color value at a single point in space and time. (x,y) are spatial
  // coordinates, with the smaller dimension in the range [-1,1]. For a 2:1
  // aspect ratio, x would be in the range [-2,2] and y in [-1,1]; for 1:2, x
  // would be in [-1,1] and y in [-2,2]. t is in seconds.
  virtual Rgb PointValue(
      const ThreadState* thread_state, const TimeState* time_state,
      float x, float y, double t) = 0;

  // Override this if the per-thread state shouldn't be null.
  virtual ::std::unique_ptr<ThreadState> GetThreadState() {
    return nullptr;
  }

  // Override this if the per-point-in-time state shouldn't be null.
  virtual ::std::unique_ptr<TimeState> GetTimeState(double t) {
    return nullptr;
  }

 private:
  // Fill a single frame.
  void FillFrame(
      const ThreadState* thread_state, int frame_number, uint8_t* buf) {
    double subframe_times[OVERSAMPLE_TEMPORAL];
    ::std::unique_ptr<TimeState> time_states[OVERSAMPLE_TEMPORAL];
    for (int sub_frame = 0; sub_frame < OVERSAMPLE_TEMPORAL; ++sub_frame) {
      subframe_times[sub_frame] =
          (frame_number + (double)sub_frame / OVERSAMPLE_TEMPORAL) / FRAMERATE;
      time_states[sub_frame] = GetTimeState(subframe_times[sub_frame]);
    }

    int buf_len = 0;
    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        Rgb values[internal::kValuesPerPixel];
        int value_count = 0;

        for (int sub_x = 0; sub_x < OVERSAMPLE_SPATIAL; ++sub_x) {
          float real_x = x + (float)sub_x / OVERSAMPLE_SPATIAL;
          real_x = 2.0f * real_x / WIDTH - 1.0f;
          if (internal::kAspectRatio > 1.0f) {
            real_x *= internal::kAspectRatio;
          }

          for (int sub_y = 0; sub_y < OVERSAMPLE_SPATIAL; ++sub_y) {
            float real_y = y + (float)sub_y / OVERSAMPLE_SPATIAL;
            real_y = 2.0f * real_y / HEIGHT - 1.0f;
            if (internal::kAspectRatio < 1.0f) {
              real_y /= internal::kAspectRatio;
            }

            for (int sub_frame = 0;
                sub_frame < OVERSAMPLE_TEMPORAL;
                ++sub_frame) {
              assert(value_count < internal::kValuesPerPixel);
              values[value_count++] = PointValue(
                  thread_state, time_states[sub_frame].get(),
                  real_x, real_y, subframe_times[sub_frame]);
            }
          }
        }

        assert(value_count == internal::kValuesPerPixel);
        Rgb pixel = {0.0f, 0.0f, 0.0f};
        for (int i = 0; i < value_count; ++i) {
          pixel.red += values[i].red;
          pixel.green += values[i].green;
          pixel.blue += values[i].blue;
        }
        pixel.red /= value_count;
        pixel.green /= value_count;
        pixel.blue /= value_count;
        buf[buf_len++] = SubpixelToUint8(pixel.red);
        buf[buf_len++] = SubpixelToUint8(pixel.green);
        buf[buf_len++] = SubpixelToUint8(pixel.blue);
      }
    }

    assert(buf_len == internal::kFrameLength);
  }

  // Fill one frame at a time, until there's nothing left to do.
  void FillFramesThread() {
    ::std::unique_ptr<ThreadState> thread_state = GetThreadState();

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

      FillFrame(thread_state.get(), frame_number, frame_buf);

      {
        ::std::unique_lock<::std::mutex> l(m_);
        frames_done_[frame_number % frame_count_] = true;
        next_frame_ready_to_output_.notify_one();
      }
    }
  }

  // Output one frame at a time, until there's nothing left to do.
  void OutputFramesThread() {
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

      size_t written = fwrite(frame_buf, 1, internal::kFrameLength, stdout);
      assert(written == internal::kFrameLength);

      {
        ::std::unique_lock<::std::mutex> l(m_);
        frames_done_[next_frame_to_output_++ % frame_count_] = false;
        next_frame_ready_to_generate_.notify_one();
      }
    }
  }

  // Number of FillFramesThreads to run at a time, and number of frames to keep
  // in memory at a time.
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

#endif
