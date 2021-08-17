#!/bin/bash

echo "===== start build tsdemuxer ====="

NOW_PATH=$(cd $(dirname $0); pwd)

ROOT_PATH=$(cd $NOW_PATH/../; pwd)


TOTAL_MEMORY=33554432

source $ROOT_PATH/../emsdk/emsdk_env.sh

emcc $ROOT_PATH/src/tsdemuxer.c \
    -O3 \
    -s WASM=1 \
    -s TOTAL_MEMORY=$TOTAL_MEMORY \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_demux", "_setAudioTrack"]' \
    -s ASSERTIONS=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -o $ROOT_PATH/wasm/tsdemuxer.js

echo "===== finish build tsdemuxer ====="
