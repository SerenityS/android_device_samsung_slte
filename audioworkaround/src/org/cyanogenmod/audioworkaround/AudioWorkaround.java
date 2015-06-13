/*
 * Copyright (C) 2015 The CyanogenMod Project
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
package org.cyanogenmod.audioworkaround;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.media.MediaPlayer;
import android.util.Log;

import java.lang.reflect.Method;

public class AudioWorkaround extends Activity {

    private static final String LOG_TAG = "AudioWorkaround";
    private static final int AUDIO_TIMEOUT = 3000;

	/**
	 * Get the value for the given key.
	 *
	 * @return if the key isn't found, return def if it isn't null,
	 * or an empty string otherwise
	 * @throws IllegalArgumentException if the key exceeds 32 characters
	 */
	private static String getSystemProperties(String key, String def)
	    throws IllegalArgumentException
	{
		String ret = def;
		try {
			Class<?> SystemProperties = Class.forName("android.os.SystemProperties");
			//Parameters Types
			@SuppressWarnings("rawtypes")
			Class[] paramTypes = { String.class, String.class };
			Method get = SystemProperties.getMethod("get", paramTypes);
			//Parameters
			Object[] params = { key, def };
			ret = (String) get.invoke(SystemProperties, params);
		} catch (IllegalArgumentException iAE) {
			throw iAE;
		} catch (Exception e) {
			ret = def;
			//TODO
		}
		return ret;
	}


    private MediaPlayer mMediaPlayer;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            final String prop = getSystemProperties("slte.audio.workaround",
                                                    "silent");
            int resId = R.raw.silent;

            Log.d(LOG_TAG, "try to play Audio(" + prop + ")");

            if ("no".equals(prop) || "false".equals(prop))
                return;
            if ("debug".equals(prop))
                resId = R.raw.pixiedust;

            mMediaPlayer = MediaPlayer.create(this, resId);
            mMediaPlayer.start();
            new Handler().postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (mMediaPlayer != null) {
                        mMediaPlayer.release();
                        mMediaPlayer = null;
                    }
                    finishAudioWorkaround();
                }
            }, AUDIO_TIMEOUT);
        } catch (Exception e) {
            Log.e(LOG_TAG, "failed to play Audio", e);
            finishAudioWorkaround();
        }
    }

    @Override
    public void onPause() {
        super.onPause();

        if (mMediaPlayer != null) {
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
    }

    private void finishAudioWorkaround() {
        finish();
    }
}
