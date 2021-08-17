#include <libavformat/avformat.h>
#include <libavutil/timestamp.h>

int main(int argc, char const *argv[]) {
  av_register_all();
  return 0;
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt,
                       const char *tag) {
  AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

  printf(
      stdout,
      "%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s "
      "stream_index:%d\n",
      tag, av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
      av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
      av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
      pkt->stream_index);
}

// Write File
int write_buffer(void *opaque, uint8_t *buf, int buf_size) {
  // if (!feof(fp_write)) {
  //   int true_size = fwrite(buf, 1, buf_size, fp_write);
  //   return true_size;
  // } else {
  //   return -1;
  // }
  return 0;
}

int remux(char *path) {
  AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
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

  AVOutputFormat *ofmt = av_guess_format("mp4", NULL, NULL);

  avformat_alloc_output_context2(&ofmt_ctx, ofmt, NULL, NULL);

  if (!ofmt_ctx) {
    fprintf(stderr, "Could not create output context\n");
    ret = AVERROR_UNKNOWN;
    goto end;
  }

  AVIOContext *avio =
      avio_alloc_context(io_buffer, io_buffer_size, 1, NULL, NULL, NULL, NULL);

  if (avio == NULL) {
    fprintf(stderr, "Could not create avio_out");
    goto end;
  }

  ofmt_ctx->pb = avio;
  ofmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;

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
    if (ret < 0) break;

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

  return 0;
}
