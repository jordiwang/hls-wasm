
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int size;
  uint8_t data[];
} ElementaryStreamData;

typedef struct {
  int pps;
  int sps;
} PSData;

typedef struct {
  int len;
  int pts;
  int dts;
  uint8_t data;
} PES;

typedef struct {
  char type;
  int id;
  int pid;
  int inputTimeScale;
  int sequenceNumber;
  uint8_t samples;
  int timescale;
  char contianer;
  int dropped;
  int duration;
  char codec;
  ElementaryStreamData pesData;
} TXTTrack;

typedef struct {
  char type;
  int id;
  int pid;
  int inputTimeScale;
  int sequenceNumber;
  uint8_t samples;
  int timescale;
  char contianer;
  int dropped;
  int duration;
  char codec;
  ElementaryStreamData pesData;
} ID3Track;

typedef struct {
  int width;
  int height;
  int pixelRatio[2];
  int audFound;  // 布尔类型替代
  int naluState;
  uint8_t samples;

  char type;
  int id;
  int pid;
  int inputTimeScale;
  int sequenceNumber;
  int timescale;
  char contianer;
  int dropped;
  int duration;
  char codec;
  ElementaryStreamData pesData;
  PSData psData[];
} VideoTrack;

typedef struct {
  int width;
  int height;
  int pixelRatio[2];
  int audFound;  // 布尔类型替代
  int naluState;
  uint8_t samples;

  char type;
  int id;
  int pid;
  int inputTimeScale;
  int sequenceNumber;
  int timescale;
  char contianer;
  int dropped;
  int duration;
  char codec;
  ElementaryStreamData pesData;
  PSData psData[];
} AVCTrack;

typedef struct {
  int samplerate;
  int isAAC;  // 布尔类型替代
  int channelCount;
  char manifestCodec;
  uint8_t samples;
  char type;
  int id;
  int pid;
  int inputTimeScale;
  int sequenceNumber;
  int timescale;
  char contianer;
  int dropped;
  int duration;
  char codec;
  ElementaryStreamData pesData;
  int config[];
} AudioTrack;

typedef struct {
  uint8_t data;
  int type;
  int state;
} Unit;

typedef struct {
  int key;    // int 模拟 bool
  int frame;  // int 模拟 bool
  int pts;
  int dts;
  char debug;
  int length;
  Unit *units[];

} AVCSample;

TXTTrack *txtTrack = NULL;
ID3Track *id3Track = NULL;
VideoTrack *videoTrack = NULL;
AVCTrack *avcTrack = NULL;
AudioTrack *audioTrack = NULL;
AVCSample *avcSample = NULL;

void setAudioTrack(int pid) {
  audioTrack->pid = pid;
  return;
}

int demux(uint8_t *data, int syncOffset, int length) {
  PES *pes = NULL;

  for (int start = syncOffset; start < length; start += 188) {
    if (data[start] == 0x47) {
      int stt = !!(data[start + 1] & 0x40);
      // pid is a 13-bit field starting at the last bit of TS[1]
      int pid = ((data[start + 1] & 0x1f) << 8) + data[start + 2];
      int atf = (data[start + 3] & 0x30) >> 4;

      // if an adaption field is present, its length is specified by the fifth
      // byte of the TS packet header.
      int offset = 0;
      if (atf > 1) {
        offset = start + 5 + data[start + 4];
        // continue if there is only adaptation field
        if (offset == start + 188) {
          continue;
        }
      } else {
        offset = start + 4;
      }

      if (pid == avcTrack->pid) {
        if (stt) {
          if (avcTrack->pesData.size > 0) {
            pes = parsePES(&avcTrack->pesData);
            parseAVCPES(pes, 0);
          }

          free(avcTrack->pesData.data);
          avcTrack->pesData.size = 0;
        }

        if (avcTrack->pesData.size > 0) {
          avcTrack->pesData.push(data.subarray(offset, start + 188));
          avcTrack->pesData.size += start + 188 - offset;
        }
      } else if (pid == audioTrack->pid) {
        if (stt) {
          if (audioTrack->pesData.size > 0) {
            pes = parsePES(&audioTrack->pesData);
          }
        }
      }

      // if (pid == audioTrack->pid) {
      //   if (stt) {
      //     if (audioTrack->pesData.size > 0) {
      //       /* code */
      //     }

      //     if (avcData && (pes = parsePES(avcData))) {
      //       this.parseAVCPES(pes, false);
      //     }

      //     avcData = {data : [], size : 0};
      //   }
      //   if (avcData) {
      //     avcData.data.push(data.subarray(offset, start + 188));
      //     avcData.size += start + 188 - offset;
      //   }
      // }

      // switch (pid) {

      //   case audioId:
      //     if (stt) {
      //       if (audioData && (pes = parsePES(audioData))) {
      //         if (audioTrack.isAAC) {
      //           this.parseAACPES(pes);
      //         } else {
      //           this.parseMPEGPES(pes);
      //         }
      //       }
      //       audioData = {data : [], size : 0};
      //     }
      //     if (audioData) {
      //       audioData.data.push(data.subarray(offset, start + 188));
      //       audioData.size += start + 188 - offset;
      //     }
      //     break;
      // case id3Id:
      //   if (stt) {
      //     if (id3Data && (pes = parsePES(id3Data))) {
      //       this.parseID3PES(pes);
      //     }

      //     id3Data = {data : [], size : 0};
      //   }
      //   if (id3Data) {
      //     id3Data.data.push(data.subarray(offset, start + 188));
      //     id3Data.size += start + 188 - offset;
      //   }
      //   break;
      // case 0:
      //   if (stt) {
      //     offset += data[offset] + 1;
      //   }

      //   pmtId = this._pmtId = parsePAT(data, offset);
      //   break;
      // case pmtId: {
      //   if (stt) {
      //     offset += data[offset] + 1;
      //   }

      //   const parsedPIDs =
      //       parsePMT(data, offset,
      //                this.typeSupported.mpeg ==
      //                = true || this.typeSupported.mp3 == = true,
      //                isSampleAes);

      // only update track id if track PID found while parsing PMT
      // this is to avoid resetting the PID to -1 in case
      // track PID transiently disappears from the stream
      // this could happen in case of transient missing audio samples for
      // example NOTE this is only the PID of the track as found in TS, but
      // we are not using this for MP4 track IDs.
      // avcId = parsedPIDs.avc;
      // if (avcId > 0) {
      //   avcTrack.pid = avcId;
      // }

      // audioId = parsedPIDs.audio;
      // if (audioId > 0) {
      //   audioTrack.pid = audioId;
      //   audioTrack.isAAC = parsedPIDs.isAAC;
      // }
      // id3Id = parsedPIDs.id3;
      // if (id3Id > 0) {
      //   id3Track.pid = id3Id;
      // }

      // if (unknownPIDs && !pmtParsed) {
      //   logger.log('reparse from beginning');
      //   unknownPIDs = false;
      //   // we set it to -188, the += 188 in the for loop will reset start to
      //   // 0
      //   start = syncOffset - 188;
      // }
      // pmtParsed = this.pmtParsed = true;
      // break;
      // }
      // case 17:
      // case 0x1fff:
      //   break;
      // default:
      //   unknownPIDs = true;
      //   break;
      // }
      // }
      // }

      // return 0;
    }
  }
  return 2;
}

PES *parsePES(ElementaryStreamData *stream) {
  int i = 0;
  uint8_t *frag;
  int pesLen = 0;
  int pesHdrLen = 0;
  int pesPts = 0;
  int pesDts = 0;
  uint8_t *data = &stream->data;

  PES *pes = NULL;

  // safety check
  if (!stream || stream->size == 0) {
    return NULL;
  }

  // we might need up to 19 bytes to read PES header
  // if first chunk of data is less than 19 bytes, let's merge it with following
  // ones until we get 19 bytes usually only one merge is needed (and this is
  // rare ...)

  while (sizeof(data[0]) < 19 && sizeof(*data) > 1) {
    uint8_t newData[sizeof(data[0]) + sizeof(data[1])] = data[0];
    strcat(newData, data[1]);
    data[0] = newData;

    int len = sizeof(data);
    for (int i = 1; i < len; i++) {
      data[i] = data[i + 1];
    }
  }

  frag = data[0];
  int pesPrefix = (frag[0] << 16) + (frag[1] << 8) + frag[2];

  if (pesPrefix == 1) {
    pesLen = (frag[4] << 8) + frag[5];
    // if PES parsed length is not zero and greater than total received length,
    // stop parsing. PES might be truncated
    // minus 6 : PES header size
    if (pesLen && pesLen > stream->size - 6) {
      return NULL;
    }

    uint8_t pesFlags = frag[7];
    if (pesFlags & 0xc0) {
      /* PES header described here :
         http://dvd.sourceforge.net/dvdinfo/pes-hdr.html as PTS / DTS is 33 bit
         we cannot use bitwise operator in JS,
         as Bitwise operators treat their operands as a sequence of 32 bits */
      pesPts = (frag[9] & 0x0e) * 536870912 +  // 1 << 29
               (frag[10] & 0xff) * 4194304 +   // 1 << 22
               (frag[11] & 0xfe) * 16384 +     // 1 << 14
               (frag[12] & 0xff) * 128 +       // 1 << 7
               (frag[13] & 0xfe) / 2;

      if (pesFlags & 0x40) {
        pesDts = (frag[14] & 0x0e) * 536870912 +  // 1 << 29
                 (frag[15] & 0xff) * 4194304 +    // 1 << 22
                 (frag[16] & 0xfe) * 16384 +      // 1 << 14
                 (frag[17] & 0xff) * 128 +        // 1 << 7
                 (frag[18] & 0xfe) / 2;

        if (pesPts - pesDts > 60 * 90000) {
          pesPts = pesDts;
        }
      } else {
        pesDts = pesPts;
      }
    }

    pesHdrLen = frag[8];
    // 9 bytes : 6 bytes for PES header + 3 bytes for PES extension
    int payloadStartOffset = pesHdrLen + 9;
    if (stream->size <= payloadStartOffset) {
      return NULL;
    }
    stream->size -= payloadStartOffset;

    uint8_t pesData[stream->size];

    int dataLen = sizeof(data);
    for (int j = 0; j < dataLen; j++) {
      frag = data[j];
      int len = sizeof(frag);
      if (payloadStartOffset) {
        if (payloadStartOffset > len) {
          // trim full frag if PES header bigger than frag
          payloadStartOffset -= len;
          continue;
        } else {
          // trim partial frag if PES header smaller than frag
          frag = frag.subarray(payloadStartOffset);
          len -= payloadStartOffset;
          payloadStartOffset = 0;
        }
      }
      pesData[i] = frag;
      i += len;
    }

    if (pesLen) {
      // payload size : remove PES header + PES extension
      pesLen -= pesHdrLen + 3;
    }

    pes->data = pesData;
    pes->pts = pesPts;
    pes->dts = pesDts;
    pes->len = pesLen;

    return pes;
  }

  return NULL;
}

void parseAVCPES(PES *pes, int last) {
  AVCTrack *track = &avcTrack;
  Unit *units = parseAVCNALu(pes->data);
  int debug = 0;
  int push;
  int spsfound = 0;

  // free pes.data to save up some memory
  free(pes->data);

  // if new NAL units found and last sample still there, let's push ...
  // this helps parsing streams with missing AUD (only do this if AUD never
  // found)
  if (avcSample && sizeof(units) > 0 && track->audFound == 1) {
    pushAccessUnit(avcSample, track);
    avcSample = createAVCSample(0, pes->pts, pes->dts, "");
  }

  int length = sizeof(units);
  for (int i = 0; i < length; i++) {
    if (units[i].type == 1) {
      push = 1;

      if (!avcSample) {
        avcSample = createAVCSample(1, pes->pts, pes->dts, "");
      }

      if (debug == 1) {
        strcat(avcSample->debug, "NDR ");
      }

      avcSample->frame = 1;

      if (spsfound && sizeof(units[i].data) > 4) {
        // retrieve slice type by parsing beginning of NAL unit (follow H264
        // spec, slice_header definition) to detect keyframe embedded in NDR
        const sliceType = new ExpGolomb(data).readSliceType();
        // 2 : I slice, 4 : SI slice, 7 : I slice, 9: SI slice
        // SI slice : A slice that is coded using intra prediction only and
        // using quantisation of the prediction samples. An SI slice can be
        // coded such that its decoded samples can be constructed identically to
        // an SP slice. I slice: A slice that is not an SI slice that is decoded
        // using intra prediction only. if (sliceType === 2 || sliceType === 7)
        // {
      }
    }
  }
}

Unit *parseAVCNALu(uint8_t *array) {
  int len = sizeof(array);
  AVCTrack *track = &avcTrack;
  int state = track->naluState || 0;
  int lastState = state;

  struct Unit *units = (Unit *)calloc(3, sizeof(Unit));

  int i = 0;
  int value;
  int overflow;
  int unitType;
  int lastUnitStart = -1;
  int lastUnitType = 0;

  if (state == -1) {
    // special use case where we found 3 or 4-byte start codes exactly at the
    // end of previous PES packet
    lastUnitStart = 0;
    // NALu type is value read from offset 0
    lastUnitType = array[0] & 0x1f;
    state = 0;
    i = 1;
  }

  while (i < len) {
    value = array[i++];
    // optimization. state 0 and 1 are the predominant case. let's handle them
    // outside of the switch/case
    if (!state) {
      state = value ? 0 : 1;
      continue;
    }
    if (state == 1) {
      state = value ? 0 : 2;
      continue;
    }

    // here we have state either equal to 2 or 3
    if (!value) {
      state = 3;

    } else if (value == 1) {
      if (lastUnitStart >= 0) {
        // const unit = {
        //   data : array.subarray(lastUnitStart, i - state - 1),
        //   type : lastUnitType,
        // };
        // // logger.log('pushing NALU, type/size:' + unit.type + '/' +
        // // unit.data.byteLength);
        // units.push(unit);

        Unit *unit = NULL;

        unit->data = array.subarray(lastUnitStart, i - state - 1);
        unit->type = lastUnitType;

        units.push(unit);
      } else {
        // lastUnitStart is undefined => this is the first start code found in
        // this PES packet first check if start code delimiter is overlapping
        // between 2 PES packets, ie it started in last packet (lastState not
        // zero) and ended at the beginning of this PES packet (i <= 4 -
        // lastState)
        Unit *lastUnit = getLastNalUnit();

        if (lastUnit) {
          if (lastState && i <= 4 - lastState) {
            // start delimiter overlapping between PES packets
            // strip start delimiter bytes from the end of last NAL unit
            // check if lastUnit had a state different from zero
            if (lastUnit->state) {
              // strip last bytes
              lastUnit->data = lastUnit->data.subarray(
                  0, sizeof(lastUnit->data) - lastState);
            }
          }

          // If NAL units are not starting right at the beginning of the PES
          // packet, push preceding data into previous NAL unit.
          overflow = i - state - 1;
          if (overflow > 0) {
            // logger.log('first NALU found with overflow:' + overflow);

            uint8_t tmp[sizeof(&lastUnit->data) + overflow];

            tmp.set(lastUnit.data, 0);
            tmp.set(array.subarray(0, overflow), lastUnit.data.byteLength);

            lastUnit->data = tmp;
          }
        }
      }

      // check if we can read unit type
      if (i < len) {
        unitType = array[i] & 0x1f;
        // logger.log('find NALU @ offset:' + i + ',type:' + unitType);
        lastUnitStart = i;
        lastUnitType = unitType;
        state = 0;
      } else {
        // not enough byte to read unit type. let's read it on next PES parsing
        state = -1;
      }
    }

    else {
      state = 0;
    }
  }

  if (lastUnitStart >= 0 && state >= 0) {
    Unit *unit = NULL;

    unit->data = array.subarray(lastUnitStart, len);
    unit->type = lastUnitType;
    unit->state = state;

    units.push(unit);
  }

  // no NALu found
  if (sizeof(units) == 0) {
    // append pes.data to previous NAL unit
    Unit *lastUnit = getLastNalUnit();
    if (lastUnit) {
      uint8_t tmp[sizeof(&lastUnit->data) + sizeof(array)];

      tmp.set(lastUnit.data, 0);
      tmp.set(array.subarray(0, overflow), lastUnit.data.byteLength);

      lastUnit->data = tmp;
    }
  }

  track->naluState = state;

  return units;
}

Unit *getLastNalUnit() {
  Unit *lastUnit = NULL;

  if (!avcSample || sizeof(*avcSample->units) == 0) {
    const *samples = avcTrack->samples;
    avcSample = samples[sizeof(samples) - 1];
  }

  if (sizeof(*avcSample->units) > 0) {
    const *units = avcSample->units;
    lastUnit = units[sizeof(units) - 1];
  }

  return lastUnit;
}

AVCSample *createAVCSample(int key, int pts, int dts, char debug) {
  AVCSample *avcSample = NULL;

  avcSample->key = key;
  avcSample->frame = 0;
  avcSample->pts = pts;
  avcSample->dts = dts;
  avcSample->debug = debug;
  avcSample->length = 0;

  return avcSample;
}