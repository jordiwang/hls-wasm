# hls-wasm

hls.js + wasm, 使用 Webassembly 优化 hls.js 的转封装，解密等模块，优化 hls.js 播放性能

## 参考文章
https://gist.github.com/AlexVestin/15b90d72f51ff7521cd7ce4b70056dff
https://www.jianshu.com/p/1bfe4470349b
https://juejin.cn/post/6953777965838630926
https://cconcolato.github.io/media-mime-support/
https://juejin.cn/post/6844903566394523656
https://stackoverflow.com/questions/22996665/unable-to-get-mediasource-working-with-mp4-format-in-chrome
https://stackoverflow.com/questions/42430809/different-between-fragmented-mp4-files-generated-by-ffmpeg-and-by-code
https://stackoverflow.com/questions/34429220/how-to-generate-fragmented-mp4-files-programmatically
https://stackoverflow.com/questions/33147402/ffmpeg-malformed-aac-bitstream-detected-use-the-audio-bitstream-filter-aac-ad
https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/remuxing.c
http://ffmpeg.org/doxygen/trunk/transcoding_8c-example.html
https://www.jianshu.com/p/1bff2869b47d
https://github.com/ffmpegwasm/ffmpeg.wasm

### ffmpeg 

ffmpeg -i demo.ts -movflags frag_keyframe+empty_moov+default_base_moof demo.mp4