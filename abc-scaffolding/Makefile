# Source directory for the ABC project. This must be set on the command line.
SRCDIR =

# Name of the project. This must be set on the command line, and the following
# files must exist in $(SRCDIR):
#
# $(PROJECT).abc: ABC Notation main file
# $(PROJECT).cover.tex: (optional) Front cover page. See cover-page.cls.
PROJECT =

# Where to place outputs.
DSTDIR = $(SRCDIR)

# Font for converting MIDI to WAV.
SOUND_FONT = /usr/share/sounds/sf2/FluidR3_GM.sf2


# Internal-use variables, to control whether sheet music is generated for paper
# versus for use in video.
SHEET_TAG =
COVER_PAGE_GEO = letter
ABCM2PS_EXTRA =


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
	  printf "\t%s\n" "abc2abc $(SRCDIR)/$(PROJECT).abc -V $$voice_num | sed 's/^w:|/w: |/' > $(DSTDIR)/$(PROJECT)-V$${voice_num}.abc" && \
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

pdf: $(DSTDIR)/$(PROJECT)$(SHEET_TAG).pdf
$(DSTDIR)/$(PROJECT)$(SHEET_TAG).pdf: $(SRCDIR)/$(PROJECT).abc $(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp
	cover= && \
	if test -f "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf"; then \
	  cover="$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf"; \
	fi && \
	tmpdir="$$(mktemp -d)" && \
	abcm2ps -1 -j0 -N2 -v $(ABCM2PS_EXTRA) "$(SRCDIR)/$(PROJECT).abc" -O "$$tmpdir/page-.svg" && \
	for page in "$$tmpdir/page"-*.svg; do \
	  inkscape --export-pdf="$${page%.svg}.pdf" "$$page" || exit 1; \
	done && \
	pdftk $$cover "$$tmpdir/page"-*.pdf cat output "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).pdf" && \
	rm -rf "$$tmpdir"

# Stamp file for $(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf, or its
# non-existence.
$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp: force-rebuild
	if test -f "$(SRCDIR)/$(PROJECT).cover.tex"; then \
	  $(MAKE) "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf" || exit 1; \
	elif test -f "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf"; then \
	  rm "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf" && \
	  touch "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp" && \
	  true || exit 1; \
	elif test ! -f "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp"; then \
	  touch "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp" || exit 1; \
	fi

# Target for cover.pdf itself, assuming cover.tex exists. Other targets should
# depend on $(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp instead, and test
# whether or not $(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf exists after the
# stamp is up-to-date.
$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf: $(SRCDIR)/$(PROJECT).cover.tex cover-page.cls cover-page-geo-$(COVER_PAGE_GEO).sty
	tmpdir="$$(mktemp -d)" && \
	cp "$(SRCDIR)/$(PROJECT).cover.tex" cover-page.cls "$$tmpdir" && \
	cp "cover-page-geo-$(COVER_PAGE_GEO).sty" "$$tmpdir/cover-page-geo.sty" && \
	cd "$$tmpdir" && \
	xelatex "$(PROJECT).cover.tex" && \
	cd - && \
	mv "$$tmpdir/$(PROJECT).cover.pdf" "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.pdf" && \
	rm -rf "$$tmpdir"
	touch "$(DSTDIR)/$(PROJECT)$(SHEET_TAG).cover.stamp"

# Make a PDF of the sheet music, suitable for inclusion as stills in a video
# (16:9).
pdf-video:
	$(MAKE) SHEET_TAG=.video COVER_PAGE_GEO=video ABCM2PS_EXTRA="--pageheight 12in --pagewidth 6.75in --landscape 1" pdf

# Create a directory of images, with one page of sheet music per image. Fail
# fast if the directory already exists, to avoid needing to delete old stills
# or risk having stale stills in the high page numbers.
video-stills: pdf-video
	mkdir "$(DSTDIR)/$(PROJECT).video-stills"
	convert -density 960 "$(DSTDIR)/$(PROJECT).video.pdf" -resize '3840x2160>' "$(DSTDIR)/$(PROJECT).video-stills/still-%03d.png"


.PHONY: all midi wave midi-per-voice wave-per-voice pdf pdf-video video-stills force-rebuild
