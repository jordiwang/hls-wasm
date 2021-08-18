#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>

struct buffer_data {
  int size;
  size_t room;  ///< size left in the buffer
  uint8_t *buf;
  uint8_t *ptr;
};

struct buffer_data bd = {0};
const size_t bd_buf_size = 1024;

int main(int argc, char const *argv[]) {
  av_register_all();
  // av_log_set_level(AV_LOG_DEBUG);
  return 0;
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt,
                       const char *tag) {
  // AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

  // fprintf(
  //     stdout,
  //     "%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s
  //     " "stream_index:%d\n", tag, av_ts2str(pkt->pts),
  //     av_ts2timestr(pkt->pts, time_base), av_ts2str(pkt->dts),
  //     av_ts2timestr(pkt->dts, time_base), av_ts2str(pkt->duration),
  //     av_ts2timestr(pkt->duration, time_base), pkt->stream_index);
}

static int write_packet(void *opaque, uint8_t *buf, int buf_size) {
  struct buffer_data *bd = (struct buffer_data *)opaque;
  while (buf_size > bd->room) {
    int64_t offset = bd->ptr - bd->buf;
    bd->buf = av_realloc_f(bd->buf, 2, bd->size);
    if (!bd->buf) return AVERROR(ENOMEM);
    bd->size *= 2;
    bd->ptr = bd->buf + offset;
    bd->room = bd->size - offset;
  }

  memcpy(bd->ptr, buf, buf_size);
  bd->ptr += buf_size;
  bd->room -= buf_size;

  return buf_size;
}

static int64_t seek(void *opaque, int64_t offset, int whence) {
  struct buffer_data *bd = (struct buffer_data *)opaque;
  switch (whence) {
    case SEEK_SET:
      bd->ptr = bd->buf + offset;
      return bd->ptr;
      break;
    case SEEK_CUR:
      bd->ptr += offset;
      break;
    case SEEK_END:
      bd->ptr = (bd->buf + bd->size) + offset;
      return bd->ptr;
      break;
    case AVSEEK_SIZE:
      return bd->size;
      break;
    default:
      return -1;
  }
  return 1;
}

int remux(char *path) {
  AVFormatContext *ifmt_ctx = avformat_alloc_context();
  AVFormatContext *ofmt_ctx = avformat_alloc_context();

  AVPacket pkt;
  int ret, i;
  int stream_index = 0;
  int *stream_mapping = NULL;
  int stream_mapping_size = 0;
  int io_buffer_size = 32 * 1024;
  unsigned char *io_buffer = (unsigned char *)av_malloc(io_buffer_size);

  if ((ret = avformat_open_input(&ifmt_ctx, path, 0, 0)) < 0) {
    fprintf(stderr, "Could not open input file '%s'", path);
    goto end;
  }

  if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
    fprintf(stderr, "Failed to retrieve input stream information");
    goto end;
  }

  // av_dump_format(ifmt_ctx, 0, path, 0);

  bd.ptr = bd.buf = av_malloc(bd_buf_size);

  if (!bd.buf) {
    ret = AVERROR(ENOMEM);
    goto end;
  }

  bd.size = bd.room = bd_buf_size;

  AVIOContext *avio_ctx = avio_alloc_context(io_buffer, io_buffer_size, 1, &bd,
                                             NULL, write_packet, seek);

  if (avio_ctx == NULL) {
    fprintf(stderr, "Could not create avio_out");
    goto end;
  }

  AVOutputFormat *ofmt = av_guess_format("mp4", NULL, NULL);

  avformat_alloc_output_context2(&ofmt_ctx, ofmt, NULL, NULL);

  if (!ofmt_ctx) {
    fprintf(stderr, "Could not create output context\n");
    ret = AVERROR_UNKNOWN;
    goto end;
  }

  ofmt_ctx->pb = avio_ctx;
  // ofmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;

  stream_mapping_size = ifmt_ctx->nb_streams;
  stream_mapping =
      av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));
  if (!stream_mapping) {
    ret = AVERROR(ENOMEM);
    goto end;
  }

  for (i = 0; i < ifmt_ctx->nb_streams; i++) {
    AVStream *out_stream;
    AVStream *in_stream = ifmt_ctx->streams[i];
    AVCodecParameters *in_codecpar = in_stream->codecpar;

    if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
        in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
        in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
      stream_mapping[i] = -1;
      continue;
    }

    stream_mapping[i] = stream_index++;

    out_stream = avformat_new_stream(ofmt_ctx, NULL);
    if (!out_stream) {
      fprintf(stderr, "Failed allocating output stream\n");
      ret = AVERROR_UNKNOWN;
      goto end;
    }

    ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
    if (ret < 0) {
      fprintf(stderr, "Failed to copy codec parameters\n");
      goto end;
    }
    out_stream->codecpar->codec_tag = 0;
  }

  ret = avformat_write_header(ofmt_ctx, NULL);

  if (ret < 0) {
    fprintf(stderr, "Error occurred when write header\n");
    goto end;
  }

  while (1) {
    AVStream *in_stream, *out_stream;

    ret = av_read_frame(ifmt_ctx, &pkt);
    if (ret < 0) {
      break;
    }

    in_stream = ifmt_ctx->streams[pkt.stream_index];
    if (pkt.stream_index >= stream_mapping_size ||
        stream_mapping[pkt.stream_index] < 0) {
      av_packet_unref(&pkt);
      continue;
    }

    pkt.stream_index = stream_mapping[pkt.stream_index];
    out_stream = ofmt_ctx->streams[pkt.stream_index];
    log_packet(ifmt_ctx, &pkt, "in");

    /* copy packet */
    pkt.pts =
        av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                         AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    pkt.dts =
        av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                         AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    pkt.duration =
        av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
    pkt.pos = -1;
    log_packet(ofmt_ctx, &pkt, "out");

    ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
    if (ret < 0) {
      fprintf(stderr, "Error muxing packet\n");
      break;
    }
    av_packet_unref(&pkt);
  }

  av_write_trailer(ofmt_ctx);

end:

  avformat_close_input(&ifmt_ctx);

  /* close output */
  if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE)) {
    // avio_closep(&ofmt_ctx->pb);
  }

  avformat_free_context(ofmt_ctx);

  av_freep(&stream_mapping);

  if (ret < 0 && ret != AVERROR_EOF) {
    fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
    return 1;
  }

  int bufLength = sizeof(*bd.buf) / sizeof(uint8_t);

  fprintf(stdout, "buffer size: %d\n", bd.size);
  fprintf(stdout, "buffer room: %d\n", bd.room);
  fprintf(stdout, "buffer buf: %lu\n", sizeof(*bd.buf));
  fprintf(stdout, "buffer ptr: %lu\n", (sizeof(*bd.ptr) / sizeof(uint8_t)));

  return &bd;
}
