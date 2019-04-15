package com.zego.videotalk.utils;

import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;
import android.util.Base64;
import android.view.View;

import com.zego.videotalk.VideoTalkApplication;
import com.zego.zegoliveroom.constants.ZegoAvConfig;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.HashMap;

/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 24/10/2017.
 */

public class PrefUtil {
    static private PrefUtil sInstance;

    static private String KEY_USER_ID = "_zego_user_id";
    static private String KEY_USER_NAME = "_zego_user_name";

    static private String KEY_APP_FLAVOR = "_zego_app_flavor_index";

    static private String ZEGO_APP_WEB_RTC = "zego_app_web_rtc";
    static private String KEY_SDK_APP_ID = "_zego_app_id";
    static private String KEY_SDK_APP_SIGN_KEY = "_zego_app_sign_key";
    static private String ZEGO_SDK_APP_BUSINESS_TYPE = "_zego_app_business_type";

    static private String ZEGO_APP_INTERNATIONAL = "zego_app_international";

    static private String KEY_LIVE_QUALITY = "_zego_live_quality_index";
    static private String KEY_LIVE_QUALITY_RESOLUTION = "_zego_live_resolution";
    static private String KEY_LIVE_QUALITY_FPS = "_zego_live_fps";
    static private String KEY_LIVE_QUALITY_BITRATE = "_zego_live_bitrate";

    static private String KEY_HARDWARE_ENCODE = "_zego_hardware_encode";
    static private String KEY_TEST_ENCODE = "_zego_test_encode";
    static private String KEY_HARDWARE_DECODE = "_zego_hardware_decode";

    static private String KEY_LOG_DATA = "_zego_log_data";

    private SharedPreferences mPref;

    private PrefUtil() {
        mPref = VideoTalkApplication.getAppContext().getSharedPreferences("__global_pref_v3", Context.MODE_PRIVATE);
    }

    static public PrefUtil getInstance() {
        if (sInstance == null) {
            synchronized (PrefUtil.class) {
                if (sInstance == null) {
                    sInstance = new PrefUtil();
                }
            }
        }
        return sInstance;
    }

    private PrefUtil setInt(String key, int value) {
        SharedPreferences.Editor editor = mPref.edit();
        editor.putInt(key, value);
        editor.apply();
        return this;
    }

    private PrefUtil setBoolean(String key, boolean value) {
        SharedPreferences.Editor editor = mPref.edit();
        editor.putBoolean(key, value);
        editor.apply();
        return this;
    }

    private PrefUtil setLong(String key, long value) {
        SharedPreferences.Editor editor = mPref.edit();
        editor.putLong(key, value);
        editor.apply();
        return this;
    }

    private PrefUtil setString(String key, String value) {
        SharedPreferences.Editor editor = mPref.edit();
        editor.putString(key, value);
        editor.apply();
        return this;
    }

    private PrefUtil setObject(String key, Object value) {
        ByteArrayOutputStream baos = null;
        try {
            baos = new ByteArrayOutputStream();
            ObjectOutputStream oos = new ObjectOutputStream(baos);
            oos.writeObject(value);
            String textData = new String(Base64.encodeToString(baos.toByteArray(), Base64.DEFAULT));

            setString(key, textData);
        } catch (IOException e) {
            e.printStackTrace();
            if (baos != null) {
                try {
                    baos.close();
                } catch (IOException e2) {
                    e2.printStackTrace();
                }
            }
        }
        return this;
    }

    private Object getObject(String key) {
        Object value = null;
        ByteArrayInputStream bais = null;
        try {
            String rawValue = mPref.getString(key, null);
            if (rawValue != null) {
                byte[] rawBytes = Base64.decode(rawValue, Base64.DEFAULT);
                bais = new ByteArrayInputStream(rawBytes);
                ObjectInputStream ois = new ObjectInputStream(bais);
                value = ois.readObject();
            }
        } catch (Exception e) {
            e.printStackTrace();
            if (bais != null) {
                try {
                    bais.close();
                } catch (IOException e2) {
                    e2.printStackTrace();
                }
            }
        }
        return value;
    }

    public String getUserId() {
        return mPref.getString(KEY_USER_ID, "");
    }

    public void setUserId(String userId) {
        setString(KEY_USER_ID, userId);
    }

    public String getUserName() {
        return mPref.getString(KEY_USER_NAME, "");
    }

    public void setUserName(String userName) {
        setString(KEY_USER_NAME, userName);
    }

    public long getAppId() {
        return mPref.getLong(KEY_SDK_APP_ID, 0);
    }

    public void setAppId(long appId) {
        setLong(KEY_SDK_APP_ID, appId);
    }

    public String getAppSignKey() {
        return mPref.getString(KEY_SDK_APP_SIGN_KEY, "");
    }

    public void setAppSignKey(String appSignKey) {
        setString(KEY_SDK_APP_SIGN_KEY, appSignKey);
    }

    public void setLogData(Object logData) {
        if (logData == null) {
            setString(KEY_LOG_DATA, "");
        } else {
            setObject(KEY_LOG_DATA, logData);
        }
    }

    public void setAppFlavor(int appFlavorIndex) {
        setInt(KEY_APP_FLAVOR, appFlavorIndex);
    }

    public int getCurrentAppFlavor() {
        return mPref.getInt(KEY_APP_FLAVOR, 0);
    }

    public void setLiveQuality(int liveQualityIndex) {
        setInt(KEY_LIVE_QUALITY, liveQualityIndex);
    }

    public int getLiveQuality() {
        return mPref.getInt(KEY_LIVE_QUALITY, ZegoAvConfig.Level.High);
    }

    public void setLiveQualityResolution(int resolutionIndex) {
        setInt(KEY_LIVE_QUALITY_RESOLUTION, resolutionIndex);
    }

    public int getLiveQualityResolution() {
        return mPref.getInt(KEY_LIVE_QUALITY_RESOLUTION, ZegoAvConfig.Level.High);
    }

    public void setLiveQualityFps(int fps) {
        setInt(KEY_LIVE_QUALITY_FPS, fps);
    }

    public int getLiveQualityFps() {
        return mPref.getInt(KEY_LIVE_QUALITY_FPS, 15);
    }

    public void setLiveQualityBitrate(int bitrate) {
        setInt(KEY_LIVE_QUALITY_BITRATE, bitrate);
    }

    public int getLiveQualityBitrate() {
        return mPref.getInt(KEY_LIVE_QUALITY_BITRATE, 600);
    }

    public void setHardwareEncode(boolean value) {
        setBoolean(KEY_HARDWARE_ENCODE, value);
    }

    public boolean getHardwareEncode() {
        return mPref.getBoolean(KEY_HARDWARE_ENCODE, false);
    }

    public void setHardwareDecode(boolean value) {
        setBoolean(KEY_HARDWARE_DECODE, value);
    }

    public boolean getHardwareDecode() {
        return mPref.getBoolean(KEY_HARDWARE_DECODE, false);
    }

    public Object getLogData() {
        return getObject(KEY_LOG_DATA);
    }

    private HashMap<Integer, SharedPreferences.OnSharedPreferenceChangeListener> mLogListeners;

    public synchronized void registerLogChangeListener(final OnLogChangeListener listener) {
        if (listener == null) return;

        if (mLogListeners == null) mLogListeners = new HashMap<>();
        SharedPreferences.OnSharedPreferenceChangeListener _listener = new SharedPreferences.OnSharedPreferenceChangeListener() {

            @Override
            public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
                if (TextUtils.equals(KEY_LOG_DATA, key)) {
                    listener.onLogDataChanged();
                }
            }
        };
        mLogListeners.put(listener.hashCode(), _listener);
        mPref.registerOnSharedPreferenceChangeListener(_listener);
    }

    public synchronized void unregisterLogChangeListener(OnLogChangeListener listener) {
        if (listener == null || mLogListeners == null) return;

        SharedPreferences.OnSharedPreferenceChangeListener _listener = mLogListeners.get(listener.hashCode());
        mPref.unregisterOnSharedPreferenceChangeListener(_listener);
    }

    public void setAppWebRtc(boolean v) {
        setBoolean(ZEGO_APP_WEB_RTC, v);
    }

    public boolean getAppWebRtc() {
        return mPref.getBoolean(ZEGO_APP_WEB_RTC, false);
    }

    public void setTestEncode(boolean testEncode) {

        setBoolean(KEY_TEST_ENCODE, testEncode);
    }

    public boolean getTestEncode() {
        return mPref.getBoolean(KEY_TEST_ENCODE, true);
    }

    static private String VIDEO_CODEC = "VIDEO_CODEC";


    public int getLayeredCoding() {
        return mPref.getInt(VIDEO_CODEC, 0);
    }

    public void setLayeredCoding(int videoCodec) {
        setInt(VIDEO_CODEC, videoCodec);
    }

    public void setBusinessType(int businessType) {
        setInt(ZEGO_SDK_APP_BUSINESS_TYPE, businessType);
    }

    public void setInternational(boolean international) {
        setBoolean(ZEGO_APP_INTERNATIONAL, international);
    }

    public int getBusinessType() {
        return mPref.getInt(ZEGO_SDK_APP_BUSINESS_TYPE, 0);
    }

    public interface OnLogChangeListener {
        void onLogDataChanged();
    }
}
