# Parameters common to videos and images.
WIDTH = 3840
HEIGHT = 2160
OVERSAMPLE_SPATIAL = 1
SEED = 0

# Video parameters.
OVERSAMPLE_TEMPORAL = 1
FRAMERATE = 60
FRAME_COUNT = 900

# Encoding parameters.
QUALITY_VP9 = 10
TILE_COLUMNS_VP9 = 4
KEYFRAME_INTERVAL_MAX_VP9 = 240
QUALITY_H264 = 18
PROFILE_H264 = high
KEYFRAME_INTERVAL_MAX_H264 = 240

# By default, there's no audio. If you specify a filename here, the audio will
# be included. Additionally, FRAME_COUNT will be ignored, and the video length
# will match the audio. If you specify a file here, also change the VIDEO_TAG
# below.
AUDIO_FILE =

# For controlling output.
DSTDIR = .
VIDEO_TAG = $(WIDTH)x$(HEIGHT)os$(OVERSAMPLE_SPATIAL)ot$(OVERSAMPLE_TEMPORAL)fr$(FRAMERATE)fc$(FRAME_COUNT)s$(SEED)
VIDEO_TAG_VP9 = q$(QUALITY_VP9)tc$(TILE_COLUMNS_VP9)ki$(KEYFRAME_INTERVAL_MAX_VP9)
VIDEO_FILENAME_VP9 = contour-lines-$(VIDEO_TAG)-$(VIDEO_TAG_VP9).webm
VIDEO_TAG_H264 = q$(QUALITY_H264)p$(PROFILE_H264)ki$(KEYFRAME_INTERVAL_MAX_H264)
VIDEO_FILENAME_H264 = contour-lines-$(VIDEO_TAG)-$(VIDEO_TAG_H264).mp4
IMAGE_TAG = $(WIDTH)x$(HEIGHT)os$(OVERSAMPLE_SPATIAL)s$(SEED)
IMAGE_FILENAME_PNG = contour-lines-$(IMAGE_TAG).png

video-vp9: $(DSTDIR)/$(VIDEO_FILENAME_VP9)
$(DSTDIR)/$(VIDEO_FILENAME_VP9): $(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video $(AUDIO_FILE) Makefile
	audio_args=""; \
	if test -n "$(AUDIO_FILE)"; then \
	  audio_args="-i $(AUDIO_FILE) -codec:a copy -shortest"; \
	fi; \
	$(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video | \
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
	    -g $(KEYFRAME_INTERVAL_MAX_VP9) \
	    -tile-columns $(TILE_COLUMNS_VP9) \
	    -threads 6 \
	    "$@"

video-h264: $(DSTDIR)/$(VIDEO_FILENAME_H264)
$(DSTDIR)/$(VIDEO_FILENAME_H264): $(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video $(AUDIO_FILE) Makefile
	audio_args=""; \
	if test -n "$(AUDIO_FILE)"; then \
	  audio_args="-i $(AUDIO_FILE) -codec:a copy -shortest"; \
	fi; \
	$(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    $$audio_args \
	    -codec:v libx264 \
	    -pix_fmt yuv420p \
	    -profile:v $(PROFILE_H264) \
	    -crf $(QUALITY_H264) \
	    -g $(KEYFRAME_INTERVAL_MAX_H264) \
	    "$@"

image-png: $(DSTDIR)/$(IMAGE_FILENAME_PNG)
$(DSTDIR)/$(IMAGE_FILENAME_PNG): $(DSTDIR)/contour-lines-$(IMAGE_TAG).gen-image Makefile
	$(DSTDIR)/contour-lines-$(IMAGE_TAG).gen-image | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate 1 \
	    -i pipe:0 \
	    "$@"

gen-video: $(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video
$(DSTDIR)/contour-lines-$(VIDEO_TAG).gen-video: contour-lines.cc $(AUDIO_FILE) Makefile
	frame_count=$(FRAME_COUNT); \
	if test -n "$(AUDIO_FILE)"; then \
	  frame_count="$$(soxi -T -D "$(AUDIO_FILE)" | awk '{print 1+int($(FRAMERATE)*$$1)}')" || exit 1; \
	fi; \
	g++ -o "$@" contour-lines.cc \
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

gen-image: $(DSTDIR)/contour-lines-$(IMAGE_TAG).gen-image
$(DSTDIR)/contour-lines-$(IMAGE_TAG).gen-image: contour-lines.cc Makefile
	g++ -o "$@" contour-lines.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DOVERSAMPLE_SPATIAL=$(OVERSAMPLE_SPATIAL) \
	  -DOVERSAMPLE_TEMPORAL=1 \
	  -DFRAMERATE=1 \
	  -DFRAME_COUNT=1 \
	  -DSEED=$(SEED) \
	  -lnoise \
	  -pthread

.PHONY: video-vp9 video-h264 image-png gen-video gen-image
