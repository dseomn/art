#!/bin/sh

# Master the tracks for the "Waves of Sin" album.

log() { printf %s\\n "$*"; }
error() { log "ERROR: $*" >&2; }
fatal() { error "$*"; exit 1; }
try() { "$@" || fatal "Command failed: $*"; }
try_v() { log "Running command: $*"; try "$@"; }

try_v sox signals-of-sin/signals-of-sin.flac -b 16 final/track-01-signals-of-sin.flac gain 0.0
try_v sox overlapping-sin-waves/sparse-waves-of-sin.flac -b 16 final/track-02-sparse-waves-of-sin.flac gain -2.0
try_v sox overlapping-sin-waves/waves-of-sin.flac -b 16 final/track-03-waves-of-sin.flac gain -0.001
try_v sox overlapping-sin-waves/harmonic-waves-of-sin.flac -b 16 final/track-04-harmonic-waves-of-sin.flac gain -3.0
try_v sox overlapping-sin-waves/sparse-harmonic-waves-of-sin.flac -b 16 final/track-05-sparse-harmonic-waves-of-sin.flac gain -0.001
try_v sox overlapping-sin-waves/way-too-many-waves-of-sin.flac -b 16 final/track-06-way-too-many-waves-of-sin.flac gain -4.0
try_v sox overlapping-sin-waves/super-fast-waves-of-sin.flac -b 16 final/track-07-super-fast-waves-of-sin.flac gain -2.0
try_v sox overlapping-sin-waves/hyper-fast-waves-of-sin.flac -b 16 final/track-08-hyper-fast-waves-of-sin.flac gain -0.001
try_v sox overlapping-sin-waves/interfering-waves-of-sin.flac -b 16 final/track-09-interfering-waves-of-sin.flac gain -0.001
try_v sox overlapping-sin-waves/harmonic-interfering-waves-of-sin.flac -b 16 final/track-10-harmonic-interfering-waves-of-sin.flac gain -0.001
try_v sox overlapping-sin-waves/dedication-mt19937.flac -b 16 final/track-11-dedication-mt19937.flac gain -10.0
try_v sox signals-of-sin/signals-of-sin-reprise.flac -b 16 final/track-12-signals-of-sin-reprise.flac gain 0.0
