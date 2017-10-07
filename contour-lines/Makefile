WIDTH = 1920
HEIGHT = 1080
OVERSAMPLE_SPATIAL = 1
OVERSAMPLE_TEMPORAL = 1
FRAMERATE = 30
FRAME_COUNT = 30

contour-lines.webm: contour-lines-video Makefile
	./contour-lines-video | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format rgb24 \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    -codec:v libvpx-vp9 \
	    -lossless 1 \
	    "$@"

contour-lines-video: contour-lines-video.cc Makefile
	g++ -o "$@" contour-lines-video.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DOVERSAMPLE_SPATIAL=$(OVERSAMPLE_SPATIAL) \
	  -DOVERSAMPLE_TEMPORAL=$(OVERSAMPLE_TEMPORAL) \
	  -DFRAMERATE=$(FRAMERATE) \
	  -DFRAME_COUNT=$(FRAME_COUNT) \
	  -lnoise
