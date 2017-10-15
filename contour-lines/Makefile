# Video parameters.
WIDTH = 3840
HEIGHT = 2160
OVERSAMPLE_SPATIAL = 1
OVERSAMPLE_TEMPORAL = 1
FRAMERATE = 60
FRAME_COUNT = 900
SEED = 0

# Encoding parameters.
QUALITY_VP9 = 10

# By default, there's no audio. If you specify a filename here, the audio will
# be included. Additionally, FRAME_COUNT will be ignored, and the video length
# will match the audio. If you specify a file here, also change the TAG below.
AUDIO_FILE =

# For controlling output.
DSTDIR = .
TAG = $(WIDTH)x$(HEIGHT)os$(OVERSAMPLE_SPATIAL)ot$(OVERSAMPLE_TEMPORAL)fr$(FRAMERATE)fc$(FRAME_COUNT)s$(SEED)
VIDEO_FILENAME = contour-lines-$(TAG)-q$(QUALITY_VP9).webm

$(DSTDIR)/$(VIDEO_FILENAME): $(DSTDIR)/contour-lines-$(TAG).gen-video $(AUDIO_FILE) Makefile
	audio_args=""; \
	if test -n "$(AUDIO_FILE)"; then \
	  audio_args="-i $(AUDIO_FILE) -codec:a copy -shortest"; \
	fi; \
	$(DSTDIR)/contour-lines-$(TAG).gen-video | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    $$audio_args \
	    -codec:v libvpx-vp9 \
	    -pix_fmt yuv420p \
	    -b:v 0 -crf $(QUALITY_VP9) \
	    -tile-columns 4 \
	    -frame-parallel 1 \
	    -threads 6 \
	    "$@"

$(DSTDIR)/contour-lines-$(TAG).gen-video: contour-lines-video.cc $(AUDIO_FILE) Makefile
	frame_count=$(FRAME_COUNT); \
	if test -n "$(AUDIO_FILE)"; then \
	  frame_count="$$(soxi -T -D "$(AUDIO_FILE)" | awk '{print 1+int($(FRAMERATE)*$$1)}')" || exit 1; \
	fi; \
	g++ -o "$@" contour-lines-video.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DOVERSAMPLE_SPATIAL=$(OVERSAMPLE_SPATIAL) \
	  -DOVERSAMPLE_TEMPORAL=$(OVERSAMPLE_TEMPORAL) \
	  -DFRAMERATE=$(FRAMERATE) \
	  -DFRAME_COUNT=$$frame_count \
	  -DSEED=$(SEED) \
	  -lnoise \
	  -pthread
