# Source directory for the video/image project (not the scaffolding). This must
# be set on the command line.
SRCDIR =

# Name of the project. This must be set on the command line, and the following
# files must exist in $(SRCDIR):
#
# $(PROJECT).cc: video code
# $(PROJECT).defaults: default value for $(PROJECT_FLAGS) below
# $(PROJECT).libs: flags for libraries to link
# $(PROJECT).tag.m4: macro to tag output files
PROJECT =

# Where to place outputs.
DSTDIR = $(SRCDIR)

# Project-specific flags, specified like "-DVAR1=value -DVAR2=value".
PROJECT_FLAGS =

# Parameters common to videos and images.
WIDTH = 1280
HEIGHT = 720
OVERSAMPLE_SPATIAL = 1

# Video parameters.
OVERSAMPLE_TEMPORAL = 1
FRAMERATE = 30
FRAME_COUNT = 300

# By default, there's no audio. If you specify a filename AUDIO_FILE, the audio
# will be included in videos. Additionally, FRAME_COUNT will be ignored, and
# the video length will match the audio. If you specify a file here, also
# change AUDIO_TAG below to something appropriate.
AUDIO_FILE =

# Encoding parameters.
ENC_VP9_QUALITY = 10
ENC_VP9_TILE_COLUMNS = 4
ENC_VP9_KEYFRAME_INTERVAL_MAX = 240
ENC_H264_QUALITY = 18
ENC_H264_PROFILE = high
ENC_H264_KEYFRAME_INTERVAL_MAX = 240

# Filename tags. These are used so that an output filename can identify the
# parameters used to generate it, and also so that it's easy to test different
# combinations of parameters without overwriting files.
VIDEO_TAG = w$(WIDTH)h$(HEIGHT)os$(OVERSAMPLE_SPATIAL)ot$(OVERSAMPLE_TEMPORAL)fr$(FRAMERATE)fc$(FRAME_COUNT)
IMAGE_TAG = w$(WIDTH)h$(HEIGHT)os$(OVERSAMPLE_SPATIAL)
AUDIO_TAG = silent
ENC_VP9_TAG = q$(ENC_VP9_QUALITY)tc$(ENC_VP9_TILE_COLUMNS)ki$(ENC_VP9_KEYFRAME_INTERVAL_MAX)
ENC_H264_TAG = q$(ENC_H264_QUALITY)p$(ENC_H264_PROFILE)ki$(ENC_H264_KEYFRAME_INTERVAL_MAX)
ENC_PNG_TAG = default

CXXFLAGS = -Wall -Werror -std=c++14 -pthread -O3

GET_PROJECT_FLAGS = \
  cat "$(SRCDIR)/$(PROJECT).defaults" && \
  printf ' %s' "$(PROJECT_FLAGS)" | sed 's/-D\([^= ]\+\)\(=[^ ]\+\)\?/-U\1 \0/g'
GET_PROJECT_TAG = \
  PROJECT_FLAGS=$$($(GET_PROJECT_FLAGS)) && \
  m4 $$PROJECT_FLAGS "$(SRCDIR)/$(PROJECT).tag.m4"

video-vp9: gen-video
	PROJECT_TAG="$$($(GET_PROJECT_TAG))" && \
	audio_args="" && \
	if test -n "$(AUDIO_FILE)"; then \
	  audio_args="-i $(AUDIO_FILE) -codec:a copy -shortest"; \
	fi && \
	$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(VIDEO_TAG).gen-video | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    $$audio_args \
	    -codec:v libvpx-vp9 \
	    -pix_fmt yuv420p \
	    -b:v 0 -crf $(ENC_VP9_QUALITY) \
	    -g $(ENC_VP9_KEYFRAME_INTERVAL_MAX) \
	    -tile-columns $(ENC_VP9_TILE_COLUMNS) \
	    -threads 6 \
	    "$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(VIDEO_TAG)-$(ENC_VP9_TAG)-$(AUDIO_TAG).webm"

video-h264: gen-video
	PROJECT_TAG="$$($(GET_PROJECT_TAG))" && \
	audio_args="" && \
	if test -n "$(AUDIO_FILE)"; then \
	  audio_args="-i $(AUDIO_FILE) -codec:a copy -shortest"; \
	fi && \
	$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(VIDEO_TAG).gen-video | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    $$audio_args \
	    -codec:v libx264 \
	    -pix_fmt yuv420p \
	    -profile:v $(ENC_H264_PROFILE) \
	    -crf $(ENC_H264_QUALITY) \
	    -g $(ENC_H264_KEYFRAME_INTERVAL_MAX) \
	    "$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(VIDEO_TAG)-$(ENC_H264_TAG)-$(AUDIO_TAG).mp4"

image-png: gen-image
	PROJECT_TAG="$$($(GET_PROJECT_TAG))" && \
	"$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(IMAGE_TAG).gen-image" | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate 1 \
	    -i pipe:0 \
	    "$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(IMAGE_TAG)-$(ENC_PNG_TAG).png"

compile:
	test -n "$(BIN_SUFFIX)" -a -n "$(GENERATOR_TAG)"
	PROJECT_FLAGS=$$($(GET_PROJECT_FLAGS)) && \
	PROJECT_TAG="$$($(GET_PROJECT_TAG))" && \
	LIBS="$$(cat "$(SRCDIR)/$(PROJECT).libs")" && \
	g++ -o "$(DSTDIR)/$(PROJECT)-$${PROJECT_TAG}-$(GENERATOR_TAG)$(BIN_SUFFIX)" \
	  -I.. -I$(SRCDIR) \
	  $(CXXFLAGS) \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DOVERSAMPLE_SPATIAL=$(OVERSAMPLE_SPATIAL) \
	  -DOVERSAMPLE_TEMPORAL=$(OVERSAMPLE_TEMPORAL) \
	  -DFRAMERATE=$(FRAMERATE) \
	  -DFRAME_COUNT=$(FRAME_COUNT) \
	  $$PROJECT_FLAGS \
	  main.cc \
	  pixel.cc \
	  "$(SRCDIR)/$(PROJECT).cc" \
	  $$LIBS

gen-video:
	frame_count=$(FRAME_COUNT); \
	if test -n "$(AUDIO_FILE)"; then \
	  frame_count="$$(soxi -T -D "$(AUDIO_FILE)" | awk '{print 1+int($(FRAMERATE)*$$1)}')" || exit 1; \
	fi; \
	$(MAKE) \
	  BIN_SUFFIX=.gen-video \
	  GENERATOR_TAG=$(VIDEO_TAG) \
	  FRAME_COUNT=$$frame_count \
	  compile

gen-image:
	$(MAKE) \
	  BIN_SUFFIX=.gen-image \
	  GENERATOR_TAG=$(IMAGE_TAG) \
	  OVERSAMPLE_TEMPORAL=1 \
	  FRAMERATE=1 \
	  FRAME_COUNT=1 \
	  compile

.PHONY: video-vp9 video-h264 image-png compile gen-video gen-image
