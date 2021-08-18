#!/bin/bash

echo "===== start build remux ====="

NOW_PATH=$(cd $(dirname $0); pwd)

ROOT_PATH=$(cd $NOW_PATH/../../; pwd)

FFMPEG_PATH=$(cd $ROOT_PATH/lib/ffmpeg-emcc; pwd)

TOTAL_MEMORY=33554432

source $ROOT_PATH/../emsdk/emsdk_env.sh

emcc $ROOT_PATH/demo/remux-wasm/remux.c $FFMPEG_PATH/lib/libavformat.a $FFMPEG_PATH/lib/libavcodec.a $FFMPEG_PATH/lib/libswscale.a $FFMPEG_PATH/lib/libavutil.a \
    -O3 \
    -lworkerfs.js \
    -I "$FFMPEG_PATH/include" \
    -s WASM=1 \
    -s TOTAL_MEMORY=$TOTAL_MEMORY \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s EXPORTED_FUNCTIONS='["_main", "_free", "_remux"]' \
    -s ASSERTIONS=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -o $ROOT_PATH/demo/remux-wasm/remux.js

echo "===== finish build remux ====="
