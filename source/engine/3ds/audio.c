#ifdef __3DS__
#include "../engine.h"

#include <3ds.h>
#include <opusfile.h>
#include <stdio.h>
#include <string.h>

struct _sound {
    s16 *samples;
    u32 count;
};

struct _music {
    OggOpusFile *opusFile;
};

#define SOUND_CHANNEL_MIN 15
#define SOUND_CHANNEL_MAX 23
#define MUSIC_CHANNEL 10

// sound channel data
ndspWaveBuf *channelBuf;

static const int SAMPLE_RATE = 48000;
static const int SAMPLES_PER_BUF = SAMPLE_RATE * 120 / 1000; // 120ms buffer
static const int CHANNELS_PER_SAMPLE = 1;

static const size_t WAVEBUF_SIZE = SAMPLES_PER_BUF * CHANNELS_PER_SAMPLE * sizeof(int16_t); // Size of NDSP wavebufs

ndspWaveBuf s_waveBufs[3];
int16_t *streamBuffer = NULL;
static volatile MMusic currentMusic;

LightEvent s_event;
static volatile bool runMusicThread;
static Thread musicThread;

// Main audio decoding logic
// This function pulls and decodes audio samples from opusFile_ to fill waveBuf_
void fillBuffer(OggOpusFile *opusFile_, ndspWaveBuf *waveBuf_) {
    // Decode samples until our waveBuf is full
    int totalSamples = 0;
    while (totalSamples < SAMPLES_PER_BUF) {
        int16_t *buffer = waveBuf_->data_pcm16 + (totalSamples * CHANNELS_PER_SAMPLE);
        const size_t bufferSize = (SAMPLES_PER_BUF - totalSamples) * CHANNELS_PER_SAMPLE;

        // Decode bufferSize samples from opusFile_ into buffer,
        // storing the number of samples that were decoded (or error)
        const int samples = op_read(opusFile_, buffer, bufferSize, NULL);
        if (samples <= 0) {
            if (samples == 0)
                break; // No error here
            break;
        }

        totalSamples += samples;
    }

    // reached end of file
    if (totalSamples == 0) {
        op_pcm_seek(opusFile_, 0);
    }

    // Pass samples to NDSP
    waveBuf_->nsamples = totalSamples;
    ndspChnWaveBufAdd(MUSIC_CHANNEL, waveBuf_);
    DSP_FlushDataCache(waveBuf_->data_pcm16, totalSamples * CHANNELS_PER_SAMPLE * sizeof(int16_t));
}

// NDSP audio frame callback
// This signals the audioThread to decode more things
// once NDSP has played a sound frame, meaning that there should be
// one or more available waveBufs to fill with more data.
void audioCallback(void *const nul_) {
    if (!runMusicThread) {
        return;
    }

    LightEvent_Signal(&s_event);
}

// Audio thread
// This handles calling the decoder function to fill NDSP buffers as necessary
void musicStreamThreadMain() {
    while (runMusicThread) {
        if (currentMusic != NULL) {
            // search our waveBufs and fill any that aren't currently
            // queued for playback (i.e, those that are 'done')
            for (size_t i = 0; i < 3; ++i) {
                if (s_waveBufs[i].status != NDSP_WBUF_DONE) {
                    continue;
                }

                fillBuffer(currentMusic->opusFile, &s_waveBufs[i]);
            }
        }

        // Wait for a signal that we're needed again before continuing,
        // so that we can yield to other things that want to run
        // (Note that the 3DS uses cooperative threading)
        LightEvent_Wait(&s_event);
    }
}

void initAudio() {
    ndspInit();
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);

    channelBuf = linearAlloc(sizeof(ndspWaveBuf) * (SOUND_CHANNEL_MAX + 1));
    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        ndspChnSetRate(c, 44100);
        ndspChnSetFormat(c, NDSP_FORMAT_MONO_PCM16);
    }
    ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_POLYPHASE);
    ndspChnSetRate(MUSIC_CHANNEL, SAMPLE_RATE);
    ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_MONO_PCM16);

    // Setup LightEvent for synchronisation of musicThread
    LightEvent_Init(&s_event, RESET_ONESHOT);

    // Allocate music buffer and ndsp buffers for streaming
    const size_t bufferSize = WAVEBUF_SIZE * 3;
    streamBuffer = linearAlloc(bufferSize);

    memset(&s_waveBufs, 0, sizeof(s_waveBufs));
    int16_t *buffer = streamBuffer;

    for (size_t i = 0; i < 3; ++i) {
        s_waveBufs[i].data_vaddr = buffer;
        s_waveBufs[i].status = NDSP_WBUF_DONE;

        buffer += WAVEBUF_SIZE / sizeof(buffer[0]);
    }
    currentMusic = NULL;

    // Set the ndsp sound frame callback which signals our audioThread
    ndspSetCallback(audioCallback, NULL);

    // start streaming thread (with lower priority (->higher value) than main and importantly network thread)
    runMusicThread = true;

    // Set the thread priority to the main thread's priority ...
    s32 priority = 0x30;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    // ... then subtract 1, as lower number => higher actual priority ...
    priority -= 1;
    // ... finally, clamp it between 0x18 and 0x3F to guarantee that it's valid.
    priority = priority < 0x18 ? 0x18 : priority;
    priority = priority > 0x3F ? 0x3F : priority;

    musicThread = threadCreate(musicStreamThreadMain, NULL, 32 * 1024, priority, -1, false);
}

void exitAudio() {
    // stop streaming thread
    if (runMusicThread) {
        runMusicThread = false;
        LightEvent_Signal(&s_event);
        threadJoin(musicThread, U64_MAX);
        threadFree(musicThread);
    }

    // stop all sounds
    ndspChnWaveBufClear(MUSIC_CHANNEL);
    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        ndspChnWaveBufClear(c);
    }

    // free buffers
    linearFree(streamBuffer);

    linearFree(channelBuf);

    ndspExit();
}

MSound loadSound(char *name) {
    MSound sound = linearAlloc(sizeof(_sound));

    FILE *file = fopen(name, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        sound->count = ftell(file) / 2;
        fseek(file, 0, SEEK_SET);
        sound->samples = linearAlloc(sound->count * 2);
        fread(sound->samples, 2, sound->count, file);
    }
    fclose(file);

    return sound;
}

void playSound(MSound sound) {
    if (sound == NULL)
        return;
    if (sound->samples == NULL)
        return;

    for (int c = SOUND_CHANNEL_MIN; c <= SOUND_CHANNEL_MAX; c++) {
        if (!ndspChnIsPlaying(c)) {
            channelBuf[c].data_pcm16 = sound->samples;
            channelBuf[c].nsamples = sound->count;
            ndspChnWaveBufAdd(c, channelBuf + c);
            return;
        }
    }
}

void unloadSound(MSound sound) {
    if (sound == NULL)
        return;
    if (sound->samples != NULL) {
        linearFree(sound->samples);
        sound->samples = NULL;
    }
    linearFree(sound);
}

MMusic loadMusic(char *name) {
    MMusic music = linearAlloc(sizeof(_music));
    music->opusFile = op_open_file(name, NULL);
    return music;
}

void playMusic(MMusic music) {
    if (currentMusic == music)
        return;
    stopMusic();
    currentMusic = music;
    LightEvent_Signal(&s_event);
}

void stopMusic() {
    currentMusic = NULL;
    ndspChnWaveBufClear(MUSIC_CHANNEL);
}

void unloadMusic(MMusic music) {
    if (music == NULL)
        return;
    op_free(music->opusFile);
    linearFree(music);
}

#endif
