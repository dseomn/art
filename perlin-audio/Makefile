SAMPLE_RATE = 48000
BEAT_FREQ = 4.0
VOLUME_CHANGE_FREQ = 0.02
HARMONICS = 32
SEED = 0

perlin-audio.flac: perlin-audio Makefile
	./perlin-audio | \
	  head -c $$(($(SAMPLE_RATE) * 4 * 60)) | \
	  ffmpeg -y \
	    -f f32le \
	    -ac 1 \
	    -ar $(SAMPLE_RATE) \
	    -i pipe:0 \
	    "$@"

perlin-audio: perlin-audio.cc Makefile
	g++ -o "$@" perlin-audio.cc \
	  -Wall -Werror \
	  -std=c++14 \
	  -DSAMPLE_RATE=$(SAMPLE_RATE) \
	  -DBEAT_FREQ=$(BEAT_FREQ) \
	  -DVOLUME_CHANGE_FREQ=$(VOLUME_CHANGE_FREQ) \
	  -DHARMONICS=$(HARMONICS) \
	  -DSEED=$(SEED) \
	  -lnoise
