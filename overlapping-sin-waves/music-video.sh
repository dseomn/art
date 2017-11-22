#!/bin/sh

# Create music videos from audio.

log() { printf %s\\n "$*"; }
error() { log "ERROR: $*" >&2; }
fatal() { error "$*"; exit 1; }
try() { "$@" || fatal "Command failed: $*"; }
try_v() { log "Running command: $*"; try "$@"; }

for f in final/track-??-*.flac; do
  try_v ffmpeg -y \
    -i "$f" \
    -filter_complex '
        [0:a] pan=1c|c0=c0 [left];
        [0:a] pan=1c|c0=c1 [right];
        [left] showspectrum=size=1820x465:slide=scroll:color=intensity, pad=w=1920:h=540:x=50:y=50:color=0x1f1f1f [vleft];
        [right] showspectrum=size=1820x465:slide=scroll:color=intensity, pad=w=1920:h=540:x=50:y=25:color=0x1f1f1f [vright];
        [vleft][vright] vstack [video]
        ' \
    -c:a libvorbis \
    -q:a 10 \
    -c:v libvpx-vp9 \
    -pix_fmt yuv444p \
    -b:v 0 -crf 10 \
    -g 240 \
    -tile-columns 2 \
    -threads 6 \
    -map '[video]' -map 0:a "${f}.webm"
done
