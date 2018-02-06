BASE = piano-postulates
REFS = 1 2 3 4 5

midi:
	for ref in $(REFS); do \
	  abc2midi "$(BASE).abc" "$$ref" -o "$(BASE)-$$ref.mid" || exit 1; \
	done

svg:
	abcm2ps -v "$(BASE).abc" -O "$(BASE)-.svg"

wave: midi
	for ref in $(REFS); do \
	  $(MAKE) "$(BASE)-$$ref.wav" || exit 1; \
	done

.mid.wav:
	#timidity -Ow -o "$@" --output-24bit "$<"
	fluidsynth -F "$@" -O s24 /usr/share/sounds/sf2/FluidR3_GM.sf2 "$<"


.SUFFIXES: .abc .mid .ps .svg .wav
.PHONY: midi svg wave
