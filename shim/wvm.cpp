/*
 * Copyright (C) 2016 The CyanogenMod Project
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

#include <stdint.h>
#include <signal.h>

#include <media/stagefright/MediaBuffer.h>

extern "C" {

/* MediaBufferGroup::MediaBufferGroup */
int _ZN7android16MediaBufferGroupC1Ej(size_t growthLimit);
size_t _ZN7android16MediaBufferGroup14acquire_bufferEPPNS_11MediaBufferEbj(android::MediaBuffer **buffer, bool nonBlock, size_t reqSize);
bool _ZNK7android12IMediaSource11ReadOptions9getSeekToEPxPNS1_8SeekModeE(int64_t *time_us, int32_t *mode);
int64_t _ZNK7android12IMediaSource11ReadOptions9getLateByEv(void);
bool _ZNK7android12IMediaSource11ReadOptions14getNonBlockingEv(void);

/* libstatefright_foundation */
int _ZN7android16MediaBufferGroupC1Ev(void)
{
    return _ZN7android16MediaBufferGroupC1Ej(0); /* default: growthLimit = 0 */
}

size_t _ZN7android16MediaBufferGroup14acquire_bufferEPPNS_11MediaBufferEb(android::MediaBuffer **buffer, bool nonBlock, size_t reqSize)
{
    return _ZN7android16MediaBufferGroup14acquire_bufferEPPNS_11MediaBufferEbj(buffer, nonBlock, reqSize);
}

/* libmedia */
bool _ZNK7android11MediaSource11ReadOptions9getSeekToEPxPNS1_8SeekModeE(int64_t *time_us, int32_t *mode)
{
    return _ZNK7android12IMediaSource11ReadOptions9getSeekToEPxPNS1_8SeekModeE(time_us, mode);
}

int64_t _ZNK7android11MediaSource11ReadOptions9getLateByEv(void)
{
    return _ZNK7android12IMediaSource11ReadOptions9getLateByEv();
}

bool _ZNK7android11MediaSource11ReadOptions14getNonBlockingEv(void)
{
    return _ZNK7android12IMediaSource11ReadOptions14getNonBlockingEv();
}

sighandler_t bsd_signal(int signum, sighandler_t handler)
{
    return signal(signum, handler);
}

} /* extern */
