gcc transmux.c \
    -L/media/ninja/Data/ffmpeg-3.4.8/lib \
    -I/media/ninja/Data/ffmpeg-3.4.8/include \
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

