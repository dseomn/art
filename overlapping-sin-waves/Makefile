SAMPLE_RATE = 48000
SEED = 0
DURATION = 60.0
SILENCE_PRE = 0.0
SILENCE_POST = 2.0
MEAN_NOTE_COUNT = 12.0
MIN_NOTE_ADJUST_INTERVAL = 1.0
MAX_NOTE_ADJUST_INTERVAL = 15.0
FREQ_SPREAD_COUNT = 1
FREQ_SPREAD_FACTOR = 1.002
HARMONIC_FACTOR = 0.5
HARMONIC_THRESHOLD = 0.001

TAG = sr$(SAMPLE_RATE)s$(SEED)d$(DURATION)spre$(SILENCE_PRE)spost$(SILENCE_POST)mnc$(MEAN_NOTE_COUNT)naimin$(MIN_NOTE_ADJUST_INTERVAL)naimax$(MAX_NOTE_ADJUST_INTERVAL)fsc$(FREQ_SPREAD_COUNT)fsf$(FREQ_SPREAD_FACTOR)hf$(HARMONIC_FACTOR)ht$(HARMONIC_THRESHOLD)

DSTDIR = .

overlapping-sin-waves-flac: $(DSTDIR)/overlapping-sin-waves-$(TAG).flac
$(DSTDIR)/overlapping-sin-waves-$(TAG).flac: $(DSTDIR)/overlapping-sin-waves-$(TAG).gen-audio Makefile
	$(DSTDIR)/overlapping-sin-waves-$(TAG).gen-audio | \
	  ffmpeg -y \
	    -f f32le \
	    -ac 2 \
	    -ar $(SAMPLE_RATE) \
	    -i pipe:0 \
	    "$@"

$(DSTDIR)/overlapping-sin-waves-$(TAG).gen-audio: overlapping-sin-waves.cc Makefile
	g++ -o "$@" overlapping-sin-waves.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DSAMPLE_RATE=$(SAMPLE_RATE) \
	  -DSEED=$(SEED) \
	  -DDURATION=$(DURATION) \
	  -DSILENCE_PRE=$(SILENCE_PRE) \
	  -DSILENCE_POST=$(SILENCE_POST) \
	  -DMEAN_NOTE_COUNT=$(MEAN_NOTE_COUNT) \
	  -DMIN_NOTE_ADJUST_INTERVAL=$(MIN_NOTE_ADJUST_INTERVAL) \
	  -DMAX_NOTE_ADJUST_INTERVAL=$(MAX_NOTE_ADJUST_INTERVAL) \
	  -DFREQ_SPREAD_COUNT=$(FREQ_SPREAD_COUNT) \
	  -DFREQ_SPREAD_FACTOR=$(FREQ_SPREAD_FACTOR) \
	  -DHARMONIC_FACTOR=$(HARMONIC_FACTOR) \
	  -DHARMONIC_THRESHOLD=$(HARMONIC_THRESHOLD)

dedication-mt19937-flac: $(DSTDIR)/dedication-mt19937.flac
$(DSTDIR)/dedication-mt19937.flac: $(DSTDIR)/dedication-mt19937.gen-audio Makefile
	./dedication-mt19937 | \
	  ffmpeg -y \
	    -f f32le \
	    -ac 2 \
	    -ar $(SAMPLE_RATE) \
	    -i pipe:0 \
	    "$@"

$(DSTDIR)/dedication-mt19937.gen-audio: dedication-mt19937.cc Makefile
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

.PHONY: overlapping-sin-waves-flac dedication-mt19937-flac
