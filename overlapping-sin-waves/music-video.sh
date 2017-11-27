#!/bin/sh

# Create music videos from audio.

log() { printf %s\\n "$*"; }
error() { log "ERROR: $*" >&2; }
fatal() { error "$*"; exit 1; }
try() { "$@" || fatal "Command failed: $*"; }
try_v() { log "Running command: $*"; try "$@"; }

for f in final/track-??-*.flac; do
  tag="${f#final/}"
  tag="${tag%.flac}"
  try_v ffmpeg -y \
    -f image2 -loop 1 -framerate 60 -i "video/${tag}.background.png" \
    -i "final/${tag}.flac" \
    -filter_complex '
        [1:a] showspectrum=size=1193x1363:slide=scroll:color=channel:mode=combined, fps=fps=60, scale=size=2386x1363 [spectrum];
        [1:a] showwaves=size=1600x1600:mode=p2p:rate=60:split_channels=1, dilation, dilation, dilation, dilation, scale=size=1024x1024 [waves];
        [0:v][spectrum] overlay=x=1408:y=742:shortest=1 [tmpa];
        [tmpa][waves] overlay=x=43:y=1082 [video];
        [video] split [video0][video1]
        ' \
    -c:a libvorbis \
    -q:a 10 \
    -c:v libvpx-vp9 \
    -pix_fmt yuv420p \
    -b:v 0 -crf 10 \
    -g 240 \
    -tile-columns 4 \
    -threads 6 \
    -map '[video0]' -map 1:a "video/${tag}.webm" \
    -c:a libmp3lame \
    -q:a 0 \
    -c:v libx264 \
    -preset slower \
    -crf 17 \
    -g 240 \
    -map '[video1]' -map 1:a "video/${tag}.mp4"
  try_v ln -s "../video/${tag}.mp4" final/
  try_v ln -s "../video/${tag}.webm" final/
done
