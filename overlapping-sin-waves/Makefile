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
	  -DSILENCE_PRE=0.0 \
	  -DSILENCE_POST=2.0 \
	  -DMEAN_NOTE_COUNT=12.0 \
	  -DMIN_NOTE_ADJUST_INTERVAL=1.0 \
	  -DMAX_NOTE_ADJUST_INTERVAL=15.0 \
	  -DFREQ_SPREAD_COUNT=1 \
	  -DFREQ_SPREAD_FACTOR=1.002 \
	  -DHARMONIC_FACTOR=0.5 \
	  -DHARMONIC_THRESHOLD=0.001

dedication-mt19937.flac: dedication-mt19937 Makefile
	./dedication-mt19937 | \
	  ffmpeg -y \
	    -f f32le \
	    -ac 2 \
	    -ar $(SAMPLE_RATE) \
	    -i pipe:0 \
	    "$@"

dedication-mt19937: dedication-mt19937.cc Makefile
	g++ -o "$@" dedication-mt19937.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DSAMPLE_RATE=$(SAMPLE_RATE) \
	  -DSEED=$(SEED) \
	  -DFADE_IN_DURATION=0.1 \
	  -DFADE_OUT_START=2.0 \
	  -DFADE_OUT_ERF_OFFSET=-2.4 \
	  -DFADE_OUT_ERF_FACTOR=0.35 \
	  -DSILENCE_THRESHOLD=1e-6 \
	  -DSILENCE_POST_MIN=2.0
