CXXFLAGS = -Wall -Werror -std=c++14
DATADIR=/home/dseomn/Music/archive/homemade/exponential-vuvuzelas

all: exponential-vuvuzelas

step-3:
	mkdir "$(DATADIR)/step-3-one-blow-per-file-processed"
	cd "$(DATADIR)/step-2-one-blow-per-file" && \
	for f in *.flac; do \
	  offset="$$(sox "$$f" -n stats 2>&1 | grep '^DC offset' | awk '{print -$$NF}')" || exit 1; \
	  sox "$$f" -b 24 "../step-3-one-blow-per-file-processed/$$f" dcshift "$$offset" gain -0.15 highpass 50 || exit 1; \
	done

step-4: exponential-vuvuzelas
	mkdir "$(DATADIR)/step-4-code-output"
	tmpdir="$$(mktemp -d)" && \
	prog="$$(pwd)/exponential-vuvuzelas" && \
	cd "$(DATADIR)/step-3-one-blow-per-file-processed" && \
	for f in *.flac; do \
	  ffmpeg -i "$$f" -f f32le -c:a pcm_f32le "$${tmpdir}/$${f}.f32le" || exit 1; \
	done && \
	cd "$$tmpdir" && \
	"$$prog" && \
	for f in track-*.f32le; do \
	  ffmpeg -f f32le -ac 2 -ar 44100 -i "$$f" "$(DATADIR)/step-4-code-output/$${f%.f32le}.flac" || exit 1; \
	done && \
	rm -rf "$$tmpdir"

step-5:
	mkdir "$(DATADIR)/step-5-final-audio"
	cd "$(DATADIR)/step-4-code-output" && \
	split_cmd="sox -p -b 16 ../step-5-final-audio/track-%2n.flac" && \
	newfile= && \
	for f in track-*.flac; do \
	  test -n "$$newfile" && split_cmd="$$split_cmd : newfile :"; \
	  newfile=yes; \
	  samples="$$(soxi -s "$$f")" || exit 1; \
	  split_cmd="$$split_cmd trim 0 $${samples}s"; \
	done && \
	sox track-*.flac -p gain -0.001 treble -10 2.5k 1s | $$split_cmd
	sox -n -r 44100 -c 2 -b 16 "$(DATADIR)/step-5-final-audio/track-13.flac" trim 0.0 60.0
	sox "$(DATADIR)/step-3-one-blow-per-file-processed/vuvuzela-*.flac" -c 2 -b 16 "$(DATADIR)/step-5-final-audio/track-14.flac" gain -0.001

step-6:
	mkdir "$(DATADIR)/step-6-music-videos"
	sox "$(DATADIR)/step-5-final-audio/"track-{01,02,03,04,05,06,07,08,09,10,11,12,13}.flac -t vorbis -C 10 "$(DATADIR)/step-6-music-videos/exponential-vuvuzelas.vorbis"
	sox "$(DATADIR)/step-5-final-audio/track-14.flac" -t vorbis -C 10 "$(DATADIR)/step-6-music-videos/all-37-samples.vorbis"
	$(MAKE) -C ../contour-lines \
	    DSTDIR="$(DATADIR)/step-6-music-videos" \
	    TAG=exponential-vuvuzelas \
	    AUDIO_FILE="$(DATADIR)/step-6-music-videos/exponential-vuvuzelas.vorbis" \
	    WIDTH=3840 \
	    HEIGHT=2160 \
	    FRAMERATE=60 \
	    SEED=0
	$(MAKE) -C ../contour-lines \
	    DSTDIR="$(DATADIR)/step-6-music-videos" \
	    TAG=all-37-samples \
	    AUDIO_FILE="$(DATADIR)/step-6-music-videos/all-37-samples.vorbis" \
	    WIDTH=3840 \
	    HEIGHT=2160 \
	    FRAMERATE=60 \
	    SEED=1

score:
	mkdir "$(DATADIR)/score"
	abcm2ps -j0 -N2 -v "exponential-vuvuzelas.abc" -O "$(DATADIR)/score/page-.svg"
	for page in "$(DATADIR)/score/page"-*.svg; do \
	  inkscape --export-pdf="$${page%.svg}.pdf" "$$page" || exit 1; \
	done
	pdftk "$(DATADIR)/score/page"-*.pdf cat output "$(DATADIR)/score/exponential-vuvuzelas.pdf"


$(DATADIR)/cover-art/vuvuzelas.png: exponential-vuvuzelas.pov
	povray +A +AM2 +J +H2048 +W4096 +UA +O"$@" exponential-vuvuzelas.pov

.PHONY: all step-3 step-4 step-5 step-6 score
