SAMPLE_RATE = 48000
SEED = 0

overlapping-sin-waves.flac: overlapping-sin-waves Makefile
	./overlapping-sin-waves | \
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
	  -DDURATION=60.0 \
	  -DMEAN_NOTE_COUNT=12.0 \
	  -DMIN_NOTE_ADJUST_INTERVAL=1.0 \
	  -DMAX_NOTE_ADJUST_INTERVAL=15.0 \
	  -DHARMONIC_FACTOR=0.5 \
	  -DHARMONIC_THRESHOLD=0.001
