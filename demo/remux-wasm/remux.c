#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>

int main(int argc, char const *argv[])
{
    av_register_all();
    return 0;
}

uint8_t *remux(char *path){

    int stream_index = 0;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;

    AVOutputFormat *pOutputFormat = NULL;
    AVFormatContext *pInputFormatCtx = avformat_alloc_context();
    AVFormatContext *pOutputFormatCtx = avformat_alloc_context();
    AVPacket pkt;
    
    
    if (avformat_open_input(&pInputFormatCtx, path, NULL, NULL) < 0) {
        fprintf(stderr, "avformat_open_input failed\n");
        return NULL;
    }

    if ((avformat_find_stream_info(pInputFormatCtx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
    }

    avformat_alloc_output_context2(&pOutputFormatCtx, NULL, 'mp4', NULL);

    if (!pOutputFormatCtx) {
        fprintf(stderr, "Could not create output context\n");
        return NULL;        
    }

    stream_mapping_size = pInputFormatCtx->nb_streams;
    stream_mapping = av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));

    if (!stream_mapping) {
        fprintf(stderr,"Could not macllc stream_mapping");
        return NULL;
    }

    pOutputFormat = pOutputFormatCtx->oformat;

    for (int i = 0; i < pInputFormatCtx->nb_streams; i++)
    {
        AVStream *out_stream;
        AVStream *in_stream = pInputFormatCtx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;


        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
            in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            stream_mapping[i] = -1;
            continue;
        }

        stream_mapping[i] = stream_index++;


        out_stream = avformat_new_stream(pOutputFormatCtx, NULL);
        if (!out_stream) {
            fprintf(stderr, "Failed allocating output stream\n");
            return NULL;
        }

        if (avcodec_parameters_copy(out_stream->codecpar, in_codecpar) < 0)
        {
            fprintf(stderr, "Failed to copy codec parameters\n");
            return NULL;
        }

        out_stream->codecpar->codec_tag = 0;
      
        
    }

      
        if (avformat_write_header(pOutputFormatCtx, NULL) < 0)
        {
            fprintf(stderr, "Error occurred when write header\n");
            return NULL;
        }
    

}
