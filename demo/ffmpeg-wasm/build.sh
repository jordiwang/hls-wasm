NOW_PATH=$(cd $(dirname $0); pwd)

ROOT_PATH=$(cd $NOW_PATH/../../; pwd)

FFMPEG_PATH=$(cd $ROOT_PATH/lib/ffmpeg-emcc; pwd)

source $ROOT_PATH/../emsdk/emsdk_env.sh

ARGS=(
  -I. -I./fftools # Add directory to include search path
  -Llibavcodec -Llibavdevice -Llibavfilter -Llibavformat -Llibavresample -Llibavutil -Llibpostproc -Llibswscale -Llibswresample # Add directory to library search path
  -Qunused-arguments # Don't emit warning for unused driver arguments.
  -o wasm/dist/ffmpeg-core.js fftools/ffmpeg_opt.c fftools/ffmpeg_filter.c fftools/ffmpeg_hw.c fftools/cmdutils.c fftools/ffmpeg.c # output
  -lavdevice -lavfilter -lavformat -lavcodec -lswresample -lswscale -lavutil -lm # library
  -s USE_SDL=2 # use SDL2
  -s MODULARIZE=1 # use modularized version to be more flexible
  -s EXPORT_NAME="createFFmpegCore" # assign export name for browser
  # -s PROXY_TO_WORKER=1                         # uses a plain Web Worker to run your main program
  # -s ENVIRONMENT='web,worker'
  -s EXPORTED_FUNCTIONS="[_main]" # export main and proxy_main funcs
  -s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, setValue, writeAsciiToMemory]" # export extra runtime methods
  -s INITIAL_MEMORY=33554432 # 33554432 bytes = 32MB
  -s ALLOW_MEMORY_GROWTH=1 # allows the total amount of memory used to change depending on the demands of the application
  --post-js wasm/post-js.js # emits a file after the emitted code. use to expose exit function
  -O3 # optimize code and reduce code size
)
emcc "${ARGS[@]}"