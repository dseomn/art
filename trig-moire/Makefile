WIDTH = 3840
HEIGHT = 2160
SEED = 0
FACTOR_COUNT = 3
FRAMERATE = 60
FRAME_COUNT = 900

trig-moire.webm: trig-moire Makefile
	./trig-moire | \
	  ffmpeg -y \
	    -f rawvideo \
	    -pixel_format gray \
	    -video_size $(WIDTH)x$(HEIGHT) \
	    -framerate $(FRAMERATE) \
	    -i pipe:0 \
	    -codec:v libvpx-vp9 \
	    -b:v 0 -crf 15 \
	    -g 240 \
	    -tile-columns 4 \
	    -threads 6 \
	    "$@"

trig-moire: trig-moire.cc Makefile
	g++ -o "$@" trig-moire.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DWIDTH=$(WIDTH) \
	  -DHEIGHT=$(HEIGHT) \
	  -DSEED=$(SEED) \
	  -DFACTOR_COUNT=$(FACTOR_COUNT)\
	  -DFRAMERATE=$(FRAMERATE) \
	  -DFRAME_COUNT=$(FRAME_COUNT) \
	  -lnoise
