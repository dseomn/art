# Source directory for the ABC project. This must be set on the command line.
SRCDIR =

# Name of the project. This must be set on the command line, and the following
# files must exist in $(SRCDIR):
#
# $(PROJECT).abc: ABC Notation main file
PROJECT =

# Where to place outputs.
DSTDIR = $(SRCDIR)

# Font for converting MIDI to WAV.
SOUND_FONT = /usr/share/sounds/sf2/FluidR3_GM.sf2


all: midi wave midi-per-voice wave-per-voice pdf

# Generated makefile that does much of the real work.
MKFILE = $(DSTDIR)/$(PROJECT).mk
midi: $(MKFILE)
	$(MAKE) -f $(MKFILE) midi
wave: $(MKFILE)
	$(MAKE) -f $(MKFILE) wave
midi-per-voice: $(MKFILE)
	$(MAKE) -f $(MKFILE) midi-per-voice
wave-per-voice: $(MKFILE)
	$(MAKE) -f $(MKFILE) wave-per-voice
$(MKFILE): $(SRCDIR)/$(PROJECT).abc
	tmp="$$(mktemp)" && \
	ref= && \
	voice_count=0 && \
	while read -r line; do \
	  case "$$line" in \
	    X:*) \
	      ref="$${line#X:}" && \
	      ref_voices="" && \
	      voice_num=0 && \
	      printf "%s\n" "midi: $(DSTDIR)/$(PROJECT)-X$${ref}.mid" && \
	      printf "%s\n" "wave: $(DSTDIR)/$(PROJECT)-X$${ref}.wav" && \
	      printf "%s\n" "$(DSTDIR)/$(PROJECT)-X$${ref}.mid: $(SRCDIR)/$(PROJECT).abc" && \
	      printf "\t%s\n" "abc2midi $(SRCDIR)/$(PROJECT).abc $$ref -o $(DSTDIR)/$(PROJECT)-X$${ref}.mid" && \
	      true || exit 1; \
	      ;; \
	    V:*) \
	      test -n "$$ref" || exit 1; \
	      voice="$${line#V:}" && \
	      voice="$${voice%% *}" && \
	      case "$$ref_voices" in \
	        *" $$voice "*) \
	          ;; \
	        *) \
	          voice_num=$$((voice_num + 1)) && \
	          if test $$voice_num -gt $$voice_count; then \
	            voice_count=$$voice_num; \
	          fi && \
	          printf "%s\n" "midi-per-voice: $(DSTDIR)/$(PROJECT)-X$${ref}-V$${voice}.mid" && \
	          printf "%s\n" "wave-per-voice: $(DSTDIR)/$(PROJECT)-X$${ref}-V$${voice}.wav" && \
	          printf "%s\n" "$(DSTDIR)/$(PROJECT)-X$${ref}-V$${voice}.mid: $(DSTDIR)/$(PROJECT)-V$${voice_num}.abc" && \
	          printf "\t%s\n" "abc2midi $(DSTDIR)/$(PROJECT)-V$${voice_num}.abc $$ref -o $(DSTDIR)/$(PROJECT)-X$${ref}-V$${voice}.mid" && \
	          ref_voices="$$ref_voices $$voice " && \
	          true || exit 1; \
	          ;; \
	      esac || exit 1; \
	      ;; \
	  esac || exit 1; \
	done < "$(SRCDIR)/$(PROJECT).abc" > "$$tmp" && \
	for voice_num in $$(seq 1 $$voice_count); do \
	  printf "%s\n" "$(DSTDIR)/$(PROJECT)-V$${voice_num}.abc: $(SRCDIR)/$(PROJECT).abc" && \
	  printf "\t%s\n" "abc2abc $(SRCDIR)/$(PROJECT).abc -V $$voice_num > $(DSTDIR)/$(PROJECT)-V$${voice_num}.abc" && \
	  true || exit 1; \
	done >> "$$tmp" && \
	{ \
	  printf "%s\n" ".mid.wav:" && \
	  printf "\t%s\n" "fluidsynth -F \$$@ -O s24 -r 96000 $(SOUND_FONT) \$$<" && \
	  printf "%s\n" ".SUFFIXES: .mid .wav" && \
	  printf "%s\n" ".PHONY: midi wave midi-per-voice wave-per-voice" && \
	  true || exit 1; \
	} >> "$$tmp" && \
	mv "$$tmp" "$(MKFILE)"

pdf: $(DSTDIR)/$(PROJECT).pdf
$(DSTDIR)/$(PROJECT).pdf: $(SRCDIR)/$(PROJECT).abc
	tmpdir="$$(mktemp -d)" && \
	abcm2ps -1 -j0 -N2 -v "$(SRCDIR)/$(PROJECT).abc" -O "$$tmpdir/page-.svg" && \
	for page in "$$tmpdir/page"-*.svg; do \
	  inkscape --export-pdf="$${page%.svg}.pdf" "$$page" || exit 1; \
	done && \
	pdftk "$$tmpdir/page"-*.pdf cat output "$(DSTDIR)/$(PROJECT).pdf" && \
	rm -rf "$$tmpdir"

.PHONY: all midi wave midi-per-voice wave-per-voice pdf
