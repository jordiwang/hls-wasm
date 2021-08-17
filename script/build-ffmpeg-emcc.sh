#!/bin/bash

echo "===== start build ffmpeg-emcc ====="

NOW_PATH=$(cd $(dirname $0); pwd)

ROOT_PATH=$(cd $NOW_PATH/../; pwd)

FFMPEG_PATH=$(cd $ROOT_PATH/../ffmpeg-3.4.8; pwd)

source $ROOT_PATH/../emsdk/emsdk_env.sh

rm -rf  $ROOT_PATH/lib/ffmpeg-emcc

mkdir $ROOT_PATH/lib/ffmpeg-emcc

cd $FFMPEG_PATH

make clean

CONFIG_ARGS=(
    --prefix=$ROOT_PATH/lib/ffmpeg-emcc
    --cc="emcc"
    --cxx="em++"
    --ar="emar"
    --cpu=generic
    --target-os=none
    --arch=x86_32
    --enable-gpl
    --enable-version3
    --enable-cross-compile
    --disable-logging
    --disable-programs
    --disable-ffmpeg
    --disable-ffplay
    --disable-ffprobe
    --disable-ffserver
    --disable-doc
    --disable-swresample
    --disable-postproc 
    --disable-avfilter
    --disable-pthreads
    --disable-w32threads
    --disable-os2threads
    --disable-network
    # --disable-everything
    # --enable-protocol=file
    # --enable-demuxer=mov
    # --enable-demuxer=matroska
    # --enable-demuxer=flv
    # --enable-demuxer=avi
    # --enable-decoder=h264
    # --enable-decoder=hevc
    # --enable-decoder=mpeg4
    # --enable-decoder=vp8
    # --enable-decoder=vp9
    # --enable-decoder=wmv3
    --disable-asm
    --disable-debug
)

emconfigure ./configure "${CONFIG_ARGS[@]}"

make

make install

echo "===== finish build ffmpeg-emcc ====="