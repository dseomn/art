WIDTH = 3840
HEIGHT = 2160
OVERSAMPLE_SPATIAL = 1
OVERSAMPLE_TEMPORAL = 1
FRAMERATE = 60
FRAME_COUNT = 900

# For controlling output.
DSTDIR = .
TAG = $(WIDTH)x$(HEIGHT)os$(OVERSAMPLE_SPATIAL)ot$(OVERSAMPLE_TEMPORAL)fr$(FRAMERATE)fc$(FRAME_COUNT)

$(DSTDIR)/contour-lines-$(TAG).webm: $(DSTDIR)/contour-lines-video-$(TAG) Makefile
	$(DSTDIR)/contour-lines-video-$(TAG) | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    -codec:v libvpx-vp9 \
	    -pix_fmt yuv420p \
	    -b:v 0 -crf 10 \
	    -tile-columns 4 \
	    -frame-parallel 1 \
	    -threads 6 \
	    "$@"

$(DSTDIR)/contour-lines-video-$(TAG): contour-lines-video.cc Makefile
	g++ -o "$@" contour-lines-video.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DOVERSAMPLE_SPATIAL=$(OVERSAMPLE_SPATIAL) \
	  -DOVERSAMPLE_TEMPORAL=$(OVERSAMPLE_TEMPORAL) \
	  -DFRAMERATE=$(FRAMERATE) \
	  -DFRAME_COUNT=$(FRAME_COUNT) \
	  -lnoise \
	  -pthread
