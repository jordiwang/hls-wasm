gcc transmux.c \
    -L/media/ninja/Data/Lib/ffmpeg-3.4.8/lib \
    -I/media/ninja/Data/Lib/ffmpeg-3.4.8/include \
    -lavutil \
    -lavformat \
    -lavcodec \
    -lavutil \
    -lswscale \
    -lm \
    -lpthread \
    -lswresample \
    -ldl \
    -o transmux \

