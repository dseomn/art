SAMPLE_RATE = 48000
SEED = 0

overlapping-sin-waves.flac: overlapping-sin-waves Makefile
	./overlapping-sin-waves | \
	  head -c $$(($(SAMPLE_RATE) * 2 * 4 * 60)) | \
	  ffmpeg -y \
	    -f f32le \
	    -ac 2 \
	    -ar $(SAMPLE_RATE) \
	    -i pipe:0 \
	    "$@"

overlapping-sin-waves: overlapping-sin-waves.cc Makefile
	g++ -o "$@" overlapping-sin-waves.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DSAMPLE_RATE=$(SAMPLE_RATE) \
	  -DSEED=$(SEED) \
	  -DMIN_NOTE_COUNT=1 \
	  -DMAX_NOTE_COUNT=64 \
	  -DMIN_NOTE_ADJUST_INTERVAL=1.0 \
	  -DMAX_NOTE_ADJUST_INTERVAL=15.0
