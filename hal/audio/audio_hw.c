/*
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2013-2015 The CyanogenMod Project
 *               Daniel Hillenbrand <codeworkx@cyanogenmod.com>
 *               Guillaume "XpLoDWilD" Lesniak <xplodgui@gmail.com>
 * Copyright (c) 2015-2017 Andreas Schneider <asn@cryptomilk.org>
 * Copyright (c) 2015-2017 Christopher N. Hesse <raymanfx@gmail.org>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "audio_hw_primary"
#define LOG_NDEBUG 0

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/str_parms.h>

#include <hardware/audio.h>
#include <hardware/hardware.h>

#include <linux/videodev2.h>
#include <linux/videodev2_exynos_media.h>

#include <system/audio.h>

#include <tinyalsa/asoundlib.h>

#include <audio_utils/resampler.h>
#include <audio_route/audio_route.h>

#include "routing.h"
#include "ril_interface.h"

#define PCM_CARD 0
#define PCM_CARD_SPDIF 1
#define PCM_TOTAL 2

#define PCM_DEVICE 0       /* Playback link */
#define PCM_DEVICE_VOICE 1 /* Baseband link */
#define PCM_DEVICE_SCO 2   /* Bluetooth link */
#define PCM_DEVICE_DEEP 3  /* Deep buffer */

#define MIXER_CARD 0

/* duration in ms of volume ramp applied when starting capture to remove plop */
#define CAPTURE_START_RAMP_MS 100

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define DAPM_PATH "/d/asoc/Pacific WM5110 Sound/dapm"

/*
 * Set the deep-buffer and low-latency output buffer sizes to
 * integral multiple of msec. This reduces the variations in the writes.
 */
#define DEEP_BUFFER_OUTPUT_PERIOD_SIZE 960
#define DEEP_BUFFER_OUTPUT_PERIOD_COUNT 5

#define LOW_LATENCY_OUTPUT_PERIOD_SIZE 240
#define LOW_LATENCY_OUTPUT_PERIOD_COUNT 2

#define AUDIO_CAPTURE_PERIOD_SIZE 320
#define AUDIO_CAPTURE_PERIOD_COUNT 2

#define AUDIO_CAPTURE_LOW_LATENCY_PERIOD_SIZE 240
#define AUDIO_CAPTURE_LOW_LATENCY_PERIOD_COUNT 2

#define SCO_CAPTURE_PERIOD_SIZE 240
#define SCO_CAPTURE_PERIOD_COUNT 2

#define HDMI_MULTI_PERIOD_SIZE  336
#define HDMI_MULTI_PERIOD_COUNT 8
#define HDMI_MULTI_DEFAULT_CHANNEL_COUNT 6 /* 5.1 */
#define HDMI_MULTI_DEFAULT_SAMPLING_RATE 48000
/*
 * Default sampling for HDMI multichannel output
 *
 * Maximum number of channel mask configurations supported. Currently the
 * primary output only supports 1 (stereo) and the
 * multi channel HDMI output 2 (5.1 and 7.1)
 */
#define HDMI_MAX_SUPPORTED_CHANNEL_MASKS 2


struct pcm_config pcm_config_fast = {
    .channels = 2,
    .rate = 48000,
    .period_size = LOW_LATENCY_OUTPUT_PERIOD_SIZE,
    .period_count = LOW_LATENCY_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_deep = {
    .channels = 2,
    .rate = 48000,
    .period_size = DEEP_BUFFER_OUTPUT_PERIOD_SIZE,
    .period_count = DEEP_BUFFER_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_in = {
    .channels = 2,
    .rate = 48000,
    .period_size = AUDIO_CAPTURE_PERIOD_SIZE,
    .period_count = AUDIO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .stop_threshold = INT_MAX,
    .avail_min = 0,
};

struct pcm_config pcm_config_in_low_latency = {
    .channels = 2,
    .rate = 48000,
    .period_size = AUDIO_CAPTURE_LOW_LATENCY_PERIOD_SIZE,
    .period_count = AUDIO_CAPTURE_LOW_LATENCY_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .stop_threshold = INT_MAX,
    .avail_min = 0,
};

struct pcm_config pcm_config_sco = {
    .channels = 1,
    .rate = 8000,
    .period_size = SCO_CAPTURE_PERIOD_SIZE,
    .period_count = SCO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_voice = {
    .channels = 2,
    .rate = 8000,
    .period_size = AUDIO_CAPTURE_PERIOD_SIZE,
    .period_count = AUDIO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_voice_wide = {
    .channels = 2,
    .rate = 16000,
    .period_size = AUDIO_CAPTURE_PERIOD_SIZE,
    .period_count = AUDIO_CAPTURE_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

struct pcm_config pcm_config_hdmi_multi = {
    .channels = HDMI_MULTI_DEFAULT_CHANNEL_COUNT,
    .rate = HDMI_MULTI_DEFAULT_SAMPLING_RATE,
    .period_size = HDMI_MULTI_PERIOD_SIZE,
    .period_count = HDMI_MULTI_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
};

enum output_type {
    OUTPUT_DEEP_BUF,      // deep PCM buffers output stream
    OUTPUT_LOW_LATENCY,   // low latency output stream
    OUTPUT_HDMI,          // HDMI multi channel
    OUTPUT_TOTAL
};

struct audio_device {
    struct audio_hw_device hw_device;

    pthread_mutex_t lock;       /* see note below on mutex acquisition order */
    audio_devices_t out_device; /* "or" of stream_out.device for all active output streams */
    audio_devices_t in_device;
    bool mic_mute;
    audio_source_t input_source;
    int cur_route_id;     /* current route ID: combination of input source
                           * and output device IDs */
    audio_mode_t mode;

    struct audio_route *audio_route;
    struct {
        const char *device;
        const char *route;
        int dev_id;
    } active_output;
    struct {
        const char *device;
        const char *route;
    } active_input;

    /* Call audio */
    struct pcm *pcm_voice_rx;
    struct pcm *pcm_voice_tx;

    /* SCO audio */
    struct pcm *pcm_sco_rx;
    struct pcm *pcm_sco_tx;

    float voice_volume;
    bool in_call;
    bool tty_mode;
    bool bluetooth_nrec;
    bool wb_amr;
    bool two_mic_control;

    int hdmi_drv_fd;
    audio_channel_mask_t in_channel_mask;

    /* RIL */
    struct ril_handle ril;

    struct stream_out *outputs[OUTPUT_TOTAL];
    pthread_mutex_t lock_outputs; /* see note below on mutex acquisition order */
};

struct stream_out {
    struct audio_stream_out stream;

    pthread_mutex_t lock; /* see note below on mutex acquisition order */
    struct pcm *pcm[PCM_TOTAL];
    struct pcm_config config;
    unsigned int pcm_device;
    bool standby; /* true if all PCMs are inactive */
    audio_devices_t device;
    /* FIXME: when HDMI multichannel output is active, other outputs must be disabled as
     * HDMI and WM1811 share the same I2S. This means that notifications and other sounds are
     * silent when watching a 5.1 movie. */
    bool disabled;

    audio_channel_mask_t channel_mask;
    /* Array of supported channel mask configurations. +1 so that the last entry is always 0 */
    audio_channel_mask_t supported_channel_masks[HDMI_MAX_SUPPORTED_CHANNEL_MASKS + 1];
    bool muted;
    uint64_t written; /* total frames written, not cleared when entering standby */
    int64_t last_write_time_us;

    struct audio_device *dev;
};

struct stream_in {
    struct audio_stream_in stream;

    pthread_mutex_t lock; /* see note below on mutex acquisition order */
    struct pcm *pcm;
    bool standby;

    unsigned int requested_rate;
    struct resampler_itfe *resampler;
    struct resampler_buffer_provider buf_provider;
    int16_t *buffer;
    size_t frames_in;
    int64_t frames_read; /* total frames read, not cleared when entering standby */
    int64_t last_read_time_us;
    int read_status;

    audio_source_t input_source;
    audio_io_handle_t io_handle;
    audio_devices_t device;

    uint16_t ramp_vol;
    uint16_t ramp_step;
    size_t ramp_frames;

    audio_channel_mask_t channel_mask;
    audio_input_flags_t flags;
    struct pcm_config *config;

    struct audio_device *dev;
};

#define STRING_TO_ENUM(string) { #string, string }

struct string_to_enum {
    const char *name;
    uint32_t value;
};

const struct string_to_enum out_channels_name_to_enum_table[] = {
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_STEREO),
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_5POINT1),
    STRING_TO_ENUM(AUDIO_CHANNEL_OUT_7POINT1),
};

static int get_output_device_id(audio_devices_t device)
{
    if (device == AUDIO_DEVICE_NONE)
        return OUT_DEVICE_NONE;

    if (popcount(device) == 2) {
        if ((device == (AUDIO_DEVICE_OUT_SPEAKER |
                        AUDIO_DEVICE_OUT_WIRED_HEADSET)) ||
            (device == (AUDIO_DEVICE_OUT_SPEAKER |
                        AUDIO_DEVICE_OUT_WIRED_HEADPHONE))) {
            return OUT_DEVICE_SPEAKER_AND_HEADSET;
        } else if (device == (AUDIO_DEVICE_OUT_SPEAKER |
                              AUDIO_DEVICE_OUT_EARPIECE)) {
            return OUT_DEVICE_SPEAKER_AND_EARPIECE;
        } else {
            return OUT_DEVICE_NONE;
        }
    }

    if (popcount(device) != 1)
        return OUT_DEVICE_NONE;

    switch (device) {
    case AUDIO_DEVICE_OUT_SPEAKER:
        return OUT_DEVICE_SPEAKER;
    case AUDIO_DEVICE_OUT_EARPIECE:
        return OUT_DEVICE_EARPIECE;
    case AUDIO_DEVICE_OUT_WIRED_HEADSET:
        return OUT_DEVICE_HEADSET;
    case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
        return OUT_DEVICE_HEADPHONES;
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        return OUT_DEVICE_BT_SCO;
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        return OUT_DEVICE_BT_SCO_HEADSET_OUT;
    case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
        return OUT_DEVICE_BT_SCO_CARKIT;
    default:
        return OUT_DEVICE_NONE;
    }
}

static int get_input_source_id(audio_source_t source, bool wb_amr)
{
    switch (source) {
    case AUDIO_SOURCE_DEFAULT:
        return IN_SOURCE_NONE;
    case AUDIO_SOURCE_MIC:
        return IN_SOURCE_MIC;
    case AUDIO_SOURCE_CAMCORDER:
        return IN_SOURCE_CAMCORDER;
    case AUDIO_SOURCE_VOICE_RECOGNITION:
        return IN_SOURCE_VOICE_RECOGNITION;
    case AUDIO_SOURCE_VOICE_COMMUNICATION:
        return IN_SOURCE_VOICE_COMMUNICATION;
    case AUDIO_SOURCE_VOICE_CALL:
        if (wb_amr) {
            return IN_SOURCE_VOICE_CALL_WB;
        }
        return IN_SOURCE_VOICE_CALL;
    default:
        return IN_SOURCE_NONE;
    }
}

static void do_out_standby(struct stream_out *out);
static void adev_set_call_audio_path(struct audio_device *adev);
static int adev_set_voice_volume(struct audio_hw_device *dev, float volume);
static void start_ril_call(struct audio_device *adev);

/**
 * NOTE: when multiple mutexes have to be acquired, always respect the
 * following order: hw device > in stream > out stream
 */

/* Helper functions */

static int open_hdmi_driver(struct audio_device *adev)
{
    if (adev->hdmi_drv_fd < 0) {
        adev->hdmi_drv_fd = open("/dev/video16", O_RDWR);
        if (adev->hdmi_drv_fd < 0)
            ALOGE("%s cannot open video16 - error: %s\n",
                  __func__, strerror(errno));
    }
    return adev->hdmi_drv_fd;
}

/* must be called with hw device mutex locked */
static int enable_hdmi_audio(struct audio_device *adev, int enable)
{
    int ret;
    struct v4l2_control ctrl;

    ret = open_hdmi_driver(adev);
    if (ret < 0) {
        return ret;
    }

    ctrl.id = V4L2_CID_TV_ENABLE_HDMI_AUDIO;
    ctrl.value = !!enable;
    ret = ioctl(adev->hdmi_drv_fd, VIDIOC_S_CTRL, &ctrl);

    if (ret < 0) {
        ALOGE("V4L2_CID_TV_ENABLE_HDMI_AUDIO ioctl error (%d)", errno);
    }

    return ret;
}

/* must be called with hw device mutex locked */
static int read_hdmi_channel_masks(struct audio_device *adev, struct stream_out *out) {
    int ret;
    struct v4l2_control ctrl;

    ret = open_hdmi_driver(adev);
    if (ret < 0)
        return ret;

    ctrl.id = V4L2_CID_TV_MAX_AUDIO_CHANNELS;
    ret = ioctl(adev->hdmi_drv_fd, VIDIOC_G_CTRL, &ctrl);
    if (ret < 0) {
        ALOGE("V4L2_CID_TV_MAX_AUDIO_CHANNELS ioctl error (%d)", errno);
        return ret;
    }

    ALOGV("%s ioctl %d got %d max channels", __func__, ret, ctrl.value);

    if (ctrl.value != 6 && ctrl.value != 8)
        return -ENOSYS;

    out->supported_channel_masks[0] = AUDIO_CHANNEL_OUT_5POINT1;
    if (ctrl.value == 8)
        out->supported_channel_masks[1] = AUDIO_CHANNEL_OUT_7POINT1;

    return ret;
}

/* must be called with hw device mutex locked */
static int set_hdmi_channels(struct audio_device *adev, int channels) {
    int ret;
    struct v4l2_control ctrl;

    ret = open_hdmi_driver(adev);
    if (ret < 0)
        return ret;

    ctrl.id = V4L2_CID_TV_SET_NUM_CHANNELS;
    ctrl.value = channels;
    ret = ioctl(adev->hdmi_drv_fd, VIDIOC_S_CTRL, &ctrl);
    if (ret < 0)
        ALOGE("V4L2_CID_TV_SET_NUM_CHANNELS ioctl error (%d)", errno);

    return ret;
}

static const char *audio_device_to_dapm(int out_device_id)
{
    switch (out_device_id) {
    case OUT_DEVICE_SPEAKER_AND_EARPIECE:
    case OUT_DEVICE_SPEAKER_AND_HEADSET:
    case OUT_DEVICE_EARPIECE:
        return "RCV";
    case OUT_DEVICE_SPEAKER:
        return "SPK";
    case OUT_DEVICE_HEADSET:
    case OUT_DEVICE_HEADPHONES:
        return "HP";
    }

    return NULL;
}

static void output_device_off(int out_device_id)
{
    char *state = "Off";
    const char *device;
    char dapm[64] = {0};
    bool ok = false;
    int i;

    device = audio_device_to_dapm(out_device_id);
    if (device == NULL) {
        goto out;
    }
    snprintf(dapm, sizeof(dapm), "%s/%s", DAPM_PATH, device);

    ALOGV("%s: Check if %s is turned off\n", __func__, device);

    for (i = 0; i < 20; i++) {
        const char *p;
        char line[32] = {0};
        FILE *fp;

        fp = fopen(dapm, "r");
        if (fp == NULL) {
            ALOGE("%s: Failed to open %s\n", __func__, dapm);
            break;
        }

        p = fgets(line, sizeof(line), fp);
        fclose(fp);
        if (p == NULL) {
            break;
        }

        p = strstr(line, state);
        if (p != NULL) {
            ok = true;
            break;
        }
        usleep(5);
    }
out:
    if (ok) {
        ALOGV("%s: Output device %s turned off!\n", __func__, device);
    } else {
        ALOGE("%s: Failed to wait for %s to turn off", __func__, device);
        usleep(50);
    }
}

const char *audio_inputs[] = {
    "Headset Mic",
    "Main Mic",
    "Sub Mic",
    "Third Mic"
};

static void input_devices_off(void)
{
    char *state = "Off";
    bool ok = false;
    size_t i;

    for (i = 0; i < ARRAY_SIZE(audio_inputs); i++) {
        char dapm[64] = {0};
        int j;

        snprintf(dapm, sizeof(dapm), "%s/%s", DAPM_PATH, audio_inputs[i]);

        for (j = 0; j < 20; j++) {
            const char *p;
            char line[32] = {0};
            FILE *fp;

            fp = fopen(dapm, "r");
            if (fp == NULL) {
                ALOGE("%s: Failed to open %s\n", __func__, dapm);
                break;
            }

            p = fgets(line, sizeof(line), fp);
            fclose(fp);
            if (p == NULL) {
                break;
            }

            p = strstr(line, state);
            if (p != NULL) {
                ok = true;
                break;
            }
            usleep(5);
        }
    }
out:
    if (ok) {
        ALOGV("%s: Input devices turned off!\n", __func__);
    } else {
        ALOGE("%s: Failed to wait for device to turn off", __func__);
        usleep(50);
    }
}

static void enable_audio_route(struct audio_device *adev,
                               const char *mixer_path)
{
    ALOGV("%s: %s\n", __func__, mixer_path);

    audio_route_apply_and_update_path(adev->audio_route, mixer_path);
}

static void disable_audio_route(struct audio_device *adev,
                                const char *mixer_path)
{
    ALOGV("%s: %s\n", __func__, mixer_path);

    audio_route_reset_and_update_path(adev->audio_route, mixer_path);
}

static void enable_audio_device(struct audio_device *adev,
                                 const char *device)
{
    ALOGV("%s: %s\n", __func__, device);

    audio_route_apply_and_update_path(adev->audio_route, device);
}

static void disable_audio_device(struct audio_device *adev,
                                 const char *device)
{
    ALOGV("%s: %s\n", __func__, device);

    audio_route_reset_and_update_path(adev->audio_route, device);
}

static bool route_changed(struct audio_device *adev)
{
    int output_device_id = get_output_device_id(adev->out_device);
    int input_source_id = get_input_source_id(adev->input_source, adev->wb_amr);
    int new_route_id;

    new_route_id = (1 << (input_source_id + OUT_DEVICE_CNT)) + (1 << output_device_id);
    return new_route_id != adev->cur_route_id;
}

static void select_devices(struct audio_device *adev)
{
    int output_device_id = get_output_device_id(adev->out_device);
    int input_source_id = get_input_source_id(adev->input_source, adev->wb_amr);
    const char *output_route = NULL;
    const char *output_device = NULL;
    const char *input_route = NULL;
    const char *input_device = NULL;
    char current_device[64] = {0};
    int new_route_id;

#ifndef HDMI_INCAPABLE
    enable_hdmi_audio(adev, adev->out_device & AUDIO_DEVICE_OUT_AUX_DIGITAL);
#endif

    new_route_id = (1 << (input_source_id + OUT_DEVICE_CNT)) + (1 << output_device_id);
    if (new_route_id == adev->cur_route_id) {
        ALOGV("*** %s: Routing hasn't changed, leaving function.", __func__);
        return;
    }

    adev->cur_route_id = new_route_id;

    if (input_source_id != IN_SOURCE_NONE) {
        if (output_device_id != OUT_DEVICE_NONE) {
            input_route =
                    route_configs[input_source_id][output_device_id]->input_route;
            input_device =
                    route_configs[input_source_id][output_device_id]->input_device;
            output_route =
                    route_configs[input_source_id][output_device_id]->output_route;
            output_device =
                    route_configs[input_source_id][output_device_id]->output_device;
        } else {
            switch (adev->in_device) {
            case AUDIO_DEVICE_IN_WIRED_HEADSET & ~AUDIO_DEVICE_BIT_IN:
                output_device_id = OUT_DEVICE_HEADSET;
                break;
            case AUDIO_DEVICE_IN_BLUETOOTH_SCO_HEADSET & ~AUDIO_DEVICE_BIT_IN:
                output_device_id = OUT_DEVICE_BT_SCO_HEADSET_OUT;
                break;
            default:
                if (adev->input_source == AUDIO_SOURCE_VOICE_CALL) {
                    output_device_id = OUT_DEVICE_EARPIECE;
                } else {
                    output_device_id = OUT_DEVICE_SPEAKER;
                }
                break;
            }

            input_route =
                (route_configs[input_source_id][output_device_id])->input_route;
            input_device =
                (route_configs[input_source_id][output_device_id])->input_device;
        }
    } else {
        if (output_device_id != OUT_DEVICE_NONE) {
            output_route =
                    (route_configs[IN_SOURCE_MIC][output_device_id])->output_route;
            output_device =
                    (route_configs[IN_SOURCE_MIC][output_device_id])->output_device;
        }
    }

    ALOGV("***** %s: devices=%#x, input src=%d -> "
          "output route: %s, input route: %s",
          __func__,
          adev->out_device, adev->input_source,
          output_route ? output_route : "none",
          input_route ? input_route : "none");

    /*
     * The Arizona driver documentation describes firmware loading this way:
     *
     * To load a firmware, or to reboot the ADSP with different firmware you
     * must:
     * - Disconnect the ADSP from any active audio path so that it will be
     *   powered-down
     * - Set the firmware control to the firmware you want to load
     * - Connect the ADSP to an active audio path so it will be powered-up
     */

    /*
     * Disable the output and input device
     */
    if (adev->active_output.route != NULL) {
        disable_audio_route(adev, adev->active_output.route);
    }
    if (adev->active_output.device != NULL) {
        disable_audio_device(adev, adev->active_output.device);
        output_device_off(adev->active_output.dev_id);
    }

    if (adev->active_input.route != NULL) {
        disable_audio_route(adev, adev->active_input.route);
    }
    if (adev->active_input.device != NULL) {
        disable_audio_device(adev, adev->active_input.device);
        input_devices_off();
    }

    /*
     * Already tell the modem that we are in a call. This should make it
     * faster to accept an incoming call.
     */
    if (adev->in_call) {
        start_ril_call(adev);
    }

    /*
     * Apply the new audio routes
     */

    /* OUTPUT */
    if (output_route != NULL) {
        enable_audio_route(adev, output_route);
        adev->active_output.route = output_route;
    } else {
        adev->active_output.route = NULL;
    }

    if (output_device != NULL) {
        enable_audio_device(adev, output_device);
        adev->active_output.device = output_device;
        adev->active_output.dev_id = output_device_id;
    } else {
        adev->active_output.device = NULL;
        adev->active_output.dev_id = -1;
    }

    /* INPUT */
    if (input_route != NULL) {
        enable_audio_route(adev, input_route);
        adev->active_input.route = input_route;
    } else {
        adev->active_input.route = NULL;
    }

    if (input_device != NULL) {
        enable_audio_device(adev, input_device);
        adev->active_input.device = input_device;
    } else {
        adev->active_input.device = NULL;
    }
}

static void force_non_hdmi_out_standby(struct audio_device *adev)
{
    enum output_type type;
    struct stream_out *out;

    for (type = 0; type < OUTPUT_TOTAL; ++type) {
        out = adev->outputs[type];
        if (type == OUTPUT_HDMI || !out)
            continue;
        pthread_mutex_lock(&out->lock);
        do_out_standby(out);
        pthread_mutex_unlock(&out->lock);
    }
}

/**********************************************************
 * BT SCO functions
 **********************************************************/

/* must be called with the hw device mutex locked, OK to hold other mutexes */
static void start_bt_sco(struct audio_device *adev)
{
    if (adev->pcm_sco_rx != NULL || adev->pcm_sco_tx != NULL) {
        ALOGW("%s: SCO PCMs already open!\n", __func__);
        return;
    }

    ALOGV("%s: Opening SCO PCMs", __func__);

    adev->pcm_sco_rx = pcm_open(PCM_CARD,
                                PCM_DEVICE_SCO,
                                PCM_OUT | PCM_MONOTONIC,
                                &pcm_config_sco);
    if (adev->pcm_sco_rx != NULL && !pcm_is_ready(adev->pcm_sco_rx)) {
        ALOGE("%s: cannot open PCM SCO RX stream: %s",
              __func__, pcm_get_error(adev->pcm_sco_rx));
        goto err_sco_rx;
    }

    adev->pcm_sco_tx = pcm_open(PCM_CARD,
                                PCM_DEVICE_SCO,
                                PCM_IN | PCM_MONOTONIC,
                                &pcm_config_sco);
    if (adev->pcm_sco_tx && !pcm_is_ready(adev->pcm_sco_tx)) {
        ALOGE("%s: cannot open PCM SCO TX stream: %s",
              __func__, pcm_get_error(adev->pcm_sco_tx));
        goto err_sco_tx;
    }

    pcm_start(adev->pcm_sco_rx);
    pcm_start(adev->pcm_sco_tx);

    return;

err_sco_tx:
    pcm_close(adev->pcm_sco_tx);
    adev->pcm_sco_tx = NULL;
err_sco_rx:
    pcm_close(adev->pcm_sco_rx);
    adev->pcm_sco_rx = NULL;
}

/* must be called with the hw device mutex locked, OK to hold other mutexes */
static void stop_bt_sco(struct audio_device *adev) {
    ALOGV("%s: Closing SCO PCMs", __func__);

    if (adev->pcm_sco_rx != NULL) {
        pcm_stop(adev->pcm_sco_rx);
        pcm_close(adev->pcm_sco_rx);
        adev->pcm_sco_rx = NULL;
    }

    if (adev->pcm_sco_tx != NULL) {
        pcm_stop(adev->pcm_sco_tx);
        pcm_close(adev->pcm_sco_tx);
        adev->pcm_sco_tx = NULL;
    }
}

/**********************************************************
 * Samsung RIL functions
 **********************************************************/

/*
 * This function must be called with hw device mutex locked, OK to hold other
 * mutexes
 */
static int start_voice_call(struct audio_device *adev)
{
    struct pcm_config *voice_config;

    if (adev->pcm_voice_rx != NULL || adev->pcm_voice_tx != NULL) {
        ALOGW("%s: Voice PCMs already open!\n", __func__);
        return 0;
    }

    ALOGV("%s: Opening voice PCMs", __func__);

    if (adev->wb_amr) {
        voice_config = &pcm_config_voice_wide;
    } else {
        voice_config = &pcm_config_voice;
    }

    /* Open modem PCM channels */
    adev->pcm_voice_rx = pcm_open(PCM_CARD,
                                  PCM_DEVICE_VOICE,
                                  PCM_OUT | PCM_MONOTONIC,
                                  voice_config);
    if (adev->pcm_voice_rx != NULL && !pcm_is_ready(adev->pcm_voice_rx)) {
        ALOGE("%s: cannot open PCM voice RX stream: %s",
              __func__, pcm_get_error(adev->pcm_voice_rx));
        goto err_voice_rx;
    }

    adev->pcm_voice_tx = pcm_open(PCM_CARD,
                                  PCM_DEVICE_VOICE,
                                  PCM_IN | PCM_MONOTONIC,
                                  voice_config);
    if (adev->pcm_voice_tx != NULL && !pcm_is_ready(adev->pcm_voice_tx)) {
        ALOGE("%s: cannot open PCM voice TX stream: %s",
              __func__, pcm_get_error(adev->pcm_voice_tx));
        goto err_voice_tx;
    }

    pcm_start(adev->pcm_voice_rx);
    pcm_start(adev->pcm_voice_tx);

    /* start SCO stream if needed */
    if (adev->out_device & AUDIO_DEVICE_OUT_ALL_SCO) {
        start_bt_sco(adev);
    }

    return 0;

err_voice_tx:
    pcm_close(adev->pcm_voice_tx);
    adev->pcm_voice_tx = NULL;
err_voice_rx:
    pcm_close(adev->pcm_voice_rx);
    adev->pcm_voice_rx = NULL;

    return -ENOMEM;
}

/*
 * This function must be called with hw device mutex locked, OK to hold other
 * mutexes
 */
static void stop_voice_call(struct audio_device *adev)
{
    int status = 0;

    ALOGV("%s: Closing active PCMs", __func__);

    if (adev->pcm_voice_rx) {
        pcm_stop(adev->pcm_voice_rx);
        pcm_close(adev->pcm_voice_rx);
        adev->pcm_voice_rx = NULL;
        status++;
    }

    if (adev->pcm_voice_tx) {
        pcm_stop(adev->pcm_voice_tx);
        pcm_close(adev->pcm_voice_tx);
        adev->pcm_voice_tx = NULL;
        status++;
    }

    /* End SCO stream if needed */
    if (adev->out_device & AUDIO_DEVICE_OUT_ALL_SCO) {
        stop_bt_sco(adev);
        status++;
    }

    ALOGV("%s: Successfully closed %d active PCMs", __func__, status);
}

static void start_ril_call(struct audio_device *adev)
{
    switch (adev->out_device) {
    case AUDIO_DEVICE_OUT_EARPIECE:
    case AUDIO_DEVICE_OUT_SPEAKER:
        adev->two_mic_control = true;
        break;
    default:
        adev->two_mic_control = false;
        break;
    }

    if (adev->two_mic_control) {
        ALOGV("%s: enabling two mic control", __func__);
        ril_set_two_mic_control(&adev->ril, AUDIENCE, TWO_MIC_SOLUTION_ON);
    } else {
        ALOGV("%s: disabling two mic control", __func__);
        ril_set_two_mic_control(&adev->ril, AUDIENCE, TWO_MIC_SOLUTION_OFF);
    }

    adev_set_call_audio_path(adev);
    adev_set_voice_volume(&adev->hw_device, adev->voice_volume);

    ril_set_call_clock_sync(&adev->ril, SOUND_CLOCK_START);
}

static void start_call(struct audio_device *adev)
{
    if (adev->in_call) {
        return;
    }

    adev->in_call = true;

    if (adev->out_device == AUDIO_DEVICE_NONE &&
        adev->in_device == AUDIO_DEVICE_NONE) {
        ALOGV("%s: No device selected, use earpiece as the default",
              __func__);
        adev->out_device = AUDIO_DEVICE_OUT_EARPIECE;
    }
    adev->input_source = AUDIO_SOURCE_VOICE_CALL;

    select_devices(adev);
    start_voice_call(adev);
}

static void stop_call(struct audio_device *adev)
{
    if (!adev->in_call) {
        return;
    }

    ril_set_call_clock_sync(&adev->ril, SOUND_CLOCK_STOP);
    stop_voice_call(adev);

    /* Do not change devices if we are switching to WB */
    if (adev->mode != AUDIO_MODE_IN_CALL) {
        /* Use speaker as the default. We do not want to stay in earpiece mode */
        if (adev->out_device == AUDIO_DEVICE_NONE ||
            adev->out_device == AUDIO_DEVICE_OUT_EARPIECE) {
            adev->out_device = AUDIO_DEVICE_OUT_SPEAKER;
        }
        adev->input_source = AUDIO_SOURCE_DEFAULT;

        ALOGV("*** %s: Reset route to out devices=%#x, input src=%#x",
              __func__,
              adev->out_device,
              adev->input_source);

        adev->in_call = false;
        select_devices(adev);
    }

    adev->in_call = false;
}

static void adev_set_wb_amr_callback(void *data, int enable)
{
    struct audio_device *adev = (struct audio_device *)data;

    pthread_mutex_lock(&adev->lock);

    if (adev->wb_amr != enable) {
        adev->wb_amr = enable;

        /* reopen the modem PCMs at the new rate */
        if (adev->in_call && route_changed(adev)) {
            ALOGV("%s: %s Incall Wide Band support",
                  __func__,
                  enable ? "Turn on" : "Turn off");

            stop_call(adev);
            start_call(adev);
        }
    }

    pthread_mutex_unlock(&adev->lock);
}

static void adev_set_call_audio_path(struct audio_device *adev)
{
    enum _AudioPath device_type;

    switch(adev->out_device) {
        case AUDIO_DEVICE_OUT_SPEAKER:
            device_type = SOUND_AUDIO_PATH_SPEAKER;
            break;
        case AUDIO_DEVICE_OUT_EARPIECE:
            device_type = SOUND_AUDIO_PATH_HANDSET;
            break;
        case AUDIO_DEVICE_OUT_WIRED_HEADSET:
            device_type = SOUND_AUDIO_PATH_HEADSET;
            break;
        case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
            device_type = SOUND_AUDIO_PATH_HEADPHONE;
            break;
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
            device_type = SOUND_AUDIO_PATH_BLUETOOTH;
            break;
        default:
            /* if output device isn't supported, use handset by default */
            device_type = SOUND_AUDIO_PATH_HANDSET;
            break;
    }

    ALOGV("%s: ril_set_call_audio_path(%d)", __func__, device_type);

    ril_set_call_audio_path(&adev->ril, device_type);
}

/* must be called with hw device outputs list, output stream, and hw device mutexes locked */
static int start_output_stream(struct stream_out *out)
{
    struct audio_device *adev = out->dev;

    ALOGV("%s: starting stream", __func__);

    if (out == adev->outputs[OUTPUT_HDMI]) {
        force_non_hdmi_out_standby(adev);
    } else if (adev->outputs[OUTPUT_HDMI] && !adev->outputs[OUTPUT_HDMI]->standby) {
        out->disabled = true;
        return 0;
    }

    out->disabled = false;

    if (out->device & (AUDIO_DEVICE_OUT_SPEAKER |
                       AUDIO_DEVICE_OUT_WIRED_HEADSET |
                       AUDIO_DEVICE_OUT_WIRED_HEADPHONE |
                       AUDIO_DEVICE_OUT_AUX_DIGITAL |
                       AUDIO_DEVICE_OUT_ALL_SCO)) {
        out->pcm[PCM_CARD] = pcm_open(PCM_CARD,
                                      out->pcm_device,
                                      PCM_OUT | PCM_MONOTONIC,
                                      &out->config);
        if (out->pcm[PCM_CARD] && !pcm_is_ready(out->pcm[PCM_CARD])) {
            ALOGE("pcm_open(PCM_CARD) failed: %s",
                  pcm_get_error(out->pcm[PCM_CARD]));
            pcm_close(out->pcm[PCM_CARD]);
            return -ENOMEM;
        }
    }

    if (out->device & AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET) {
        out->pcm[PCM_CARD_SPDIF] = pcm_open(PCM_CARD_SPDIF,
                                            out->pcm_device,
                                            PCM_OUT | PCM_MONOTONIC,
                                            &out->config);
        if (out->pcm[PCM_CARD_SPDIF] &&
                !pcm_is_ready(out->pcm[PCM_CARD_SPDIF])) {
            ALOGE("pcm_open(PCM_CARD_SPDIF) failed: %s",
                  pcm_get_error(out->pcm[PCM_CARD_SPDIF]));
            pcm_close(out->pcm[PCM_CARD_SPDIF]);
            return -ENOMEM;
        }
    }

    /* in call routing must go through set_parameters */
    if (!adev->in_call) {
        adev->out_device |= out->device;
        select_devices(adev);
    }

    if (out->device & AUDIO_DEVICE_OUT_AUX_DIGITAL) {
        set_hdmi_channels(adev, out->config.channels);
    }

    ALOGV("%s: stream out device: %d, actual: %d",
          __func__, out->device, adev->out_device);

    return 0;
}

/* must be called with input stream and hw device mutexes locked */
static int start_input_stream(struct stream_in *in)
{
    struct audio_device *adev = in->dev;

    in->pcm = pcm_open(PCM_CARD,
                       PCM_DEVICE,
                       PCM_IN | PCM_MONOTONIC,
                       in->config);
    if (in->pcm && !pcm_is_ready(in->pcm)) {
        ALOGE("pcm_open() failed: %s", pcm_get_error(in->pcm));
        pcm_close(in->pcm);
        return -ENOMEM;
    }

    /* if no supported sample rate is available, use the resampler */
    if (in->resampler) {
        in->resampler->reset(in->resampler);
    }

    in->frames_in = 0;
    /* in call routing must go through set_parameters */
    if (!adev->in_call) {
        adev->input_source = in->input_source;
        adev->in_device = in->device;
        adev->in_channel_mask = in->channel_mask;

        select_devices(adev);
    }

    /* initialize volume ramp */
    in->ramp_frames = (CAPTURE_START_RAMP_MS * in->requested_rate) / 1000;
    in->ramp_step = (uint16_t)(USHRT_MAX / in->ramp_frames);
    in->ramp_vol = 0;

    return 0;
}

static size_t get_input_buffer_size(unsigned int sample_rate,
                                    audio_format_t format,
                                    unsigned int channel_count,
                                    bool is_low_latency)
{
    const struct pcm_config *config = is_low_latency ?
            &pcm_config_in_low_latency : &pcm_config_in;
    size_t size;

    /*
     * take resampling into account and return the closest majoring
     * multiple of 16 frames, as audioflinger expects audio buffers to
     * be a multiple of 16 frames
     */
    size = (config->period_size * sample_rate) / config->rate;
    size = ((size + 15) / 16) * 16;

    return size * channel_count * audio_bytes_per_sample(format);
}

static int get_next_buffer(struct resampler_buffer_provider *buffer_provider,
                                   struct resampler_buffer* buffer)
{
    struct stream_in *in;
    size_t i;

    if (buffer_provider == NULL || buffer == NULL) {
        return -EINVAL;
    }

    in = (struct stream_in *)((char *)buffer_provider -
                                   offsetof(struct stream_in, buf_provider));

    if (in->pcm == NULL) {
        buffer->raw = NULL;
        buffer->frame_count = 0;
        in->read_status = -ENODEV;
        return -ENODEV;
    }

    if (in->frames_in == 0) {
        in->read_status = pcm_read(in->pcm,
                                   (void*)in->buffer,
                                   pcm_frames_to_bytes(in->pcm, in->config->period_size));
        if (in->read_status != 0) {
            ALOGE("get_next_buffer() pcm_read error %d", in->read_status);
            buffer->raw = NULL;
            buffer->frame_count = 0;
            return in->read_status;
        }

        in->frames_in = in->config->period_size;

        /* Do stereo to mono conversion in place by discarding right channel */
        if (in->channel_mask == AUDIO_CHANNEL_IN_MONO)
            for (i = 1; i < in->frames_in; i++)
                in->buffer[i] = in->buffer[i * 2];
    }

    buffer->frame_count = (buffer->frame_count > in->frames_in) ?
                                in->frames_in : buffer->frame_count;
    buffer->i16 = in->buffer +
            (in->config->period_size - in->frames_in) *
                audio_channel_count_from_in_mask(in->channel_mask);

    return in->read_status;

}

static void release_buffer(struct resampler_buffer_provider *buffer_provider,
                                  struct resampler_buffer* buffer)
{
    struct stream_in *in;

    if (buffer_provider == NULL || buffer == NULL)
        return;

    in = (struct stream_in *)((char *)buffer_provider -
                                   offsetof(struct stream_in, buf_provider));

    in->frames_in -= buffer->frame_count;
}

/* read_frames() reads frames from kernel driver, down samples to capture rate
 * if necessary and output the number of frames requested to the buffer specified */
static ssize_t read_frames(struct stream_in *in, void *buffer, ssize_t frames)
{
    ssize_t frames_wr = 0;
    size_t frame_size = audio_stream_in_frame_size(&in->stream);

    while (frames_wr < frames) {
        size_t frames_rd = frames - frames_wr;
        if (in->resampler != NULL) {
            in->resampler->resample_from_provider(in->resampler,
                    (int16_t *)((char *)buffer +
                            frames_wr * frame_size),
                    &frames_rd);
        } else {
            struct resampler_buffer buf = {
                .raw = NULL,
                .frame_count = frames_rd,
            };
            get_next_buffer(&in->buf_provider, &buf);
            if (buf.raw != NULL) {
                memcpy((char *)buffer +
                           frames_wr * frame_size,
                        buf.raw,
                        buf.frame_count * frame_size);
                frames_rd = buf.frame_count;
            }
            release_buffer(&in->buf_provider, &buf);
        }
        /* in->read_status is updated by getNextBuffer() also called by
         * in->resampler->resample_from_provider() */
        if (in->read_status != 0)
            return in->read_status;

        frames_wr += frames_rd;
    }
    return frames_wr;
}

/* API functions */

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->config.rate;
}

static int out_set_sample_rate(struct audio_stream *stream __unused,
                               uint32_t rate __unused)
{
    return -ENOSYS;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->config.period_size *
           audio_stream_out_frame_size((const struct audio_stream_out *)stream);
}

static audio_channel_mask_t out_get_channels(const struct audio_stream *stream)
{
    struct stream_out *out = (struct stream_out *)stream;

    return out->channel_mask;
}

static audio_format_t out_get_format(const struct audio_stream *stream __unused)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_format(struct audio_stream *stream __unused,
                          audio_format_t format __unused)
{
    return -ENOSYS;
}

/* Return the set of output devices associated with active streams
 * other than out.  Assumes out is non-NULL and out->dev is locked.
 */
static audio_devices_t output_devices(struct stream_out *out)
{
    struct audio_device *dev = out->dev;
    enum output_type type;
    audio_devices_t devices = AUDIO_DEVICE_NONE;

   
