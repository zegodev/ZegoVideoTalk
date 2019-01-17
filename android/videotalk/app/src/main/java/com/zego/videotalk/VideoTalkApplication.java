package com.zego.videotalk;

import android.app.Application;
import android.os.Build;
import android.text.TextUtils;
import android.widget.Toast;

import com.tencent.bugly.crashreport.CrashReport;
import com.zego.videotalk.utils.PrefUtil;
import com.zego.videotalk.utils.SystemUtil;
import com.zego.videotalk.utils.TimeUtil;
import com.zego.zegoliveroom.ZegoLiveRoom;
import com.zego.zegoliveroom.constants.ZegoAvConfig;
import com.zego.zegoliveroom.constants.ZegoConstants;

import static com.zego.zegoliveroom.constants.ZegoConstants.Config.VIDEO_ACCELERATOR_WHITELIST;


/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 24/10/2017.
 */

public class VideoTalkApplication extends Application {

    static private VideoTalkApplication sInstance;

    final static private long DEFAULT_ZEGO_APP_ID = ZegoAppHelper.UDP_APP_ID;

    final static private String BUGLY_APP_KEY = "70580e12bb";

    static public VideoTalkApplication getAppContext() {
        return VideoTalkApplication.sInstance;
    }

    /**
     * Called when the application is starting, before any activity, service,
     * or receiver objects (excluding content providers) have been created.
     * Implementations should be as quick as possible (for example using
     * lazy initialization of state) since the time spent in this function
     * directly impacts the performance of starting the first activity,
     * service, or receiver in a process.
     * If you override this method, be sure to call super.onCreate().
     */
    @Override
    public void onCreate() {
        super.onCreate();

        sInstance = this;

        initUserInfo(); // first

        initCrashReport();  // second

        setupZegoSDK();  // last


    }

    private void initUserInfo() {
        String userId = PrefUtil.getInstance().getUserId();
        String userName = PrefUtil.getInstance().getUserName();
        if (TextUtils.isEmpty(userId) || TextUtils.isEmpty(userName)) {
            userId = TimeUtil.getNowTimeStr();
            userName = String.format("VT_%s_%s", Build.MODEL.replaceAll(",", "."), userId);

            PrefUtil.getInstance().setUserId(userId);
            PrefUtil.getInstance().setUserName(userName);
        }
    }

    private void initCrashReport() {
        CrashReport.initCrashReport(this, BUGLY_APP_KEY, false);
        CrashReport.setUserId(PrefUtil.getInstance().getUserId());
    }

    private void setupZegoSDK() {

        ZegoLiveRoom liveRoom = ZegoAppHelper.getLiveRoom();

        ZegoLiveRoom.setSDKContext(new ZegoLiveRoom.SDKContext() {
            @Override
            public String getSoFullPath() {
                return null;
            }

            @Override
            public String getLogPath() {
                return null;
            }

            @Override
            public Application getAppContext() {
                return VideoTalkApplication.this;
            }
        });


        initZegoSDK(liveRoom);


        ZegoAppHelper.saveLiveRoom(liveRoom);
    }

    public ZegoAvConfig config;

    private void initZegoSDK(ZegoLiveRoom liveRoom) {

        ZegoLiveRoom.setUser(PrefUtil.getInstance().getUserId(), PrefUtil.getInstance().getUserName());
        ZegoLiveRoom.requireHardwareEncoder(PrefUtil.getInstance().getHardwareEncode());
        ZegoLiveRoom.requireHardwareDecoder(PrefUtil.getInstance().getHardwareDecode());
        ZegoLiveRoom.setTestEnv(PrefUtil.getInstance().getTestEncode());
        byte[] signKey;
        long appId = 0;
        String strSignKey;
        int currentAppFlavor = PrefUtil.getInstance().getCurrentAppFlavor();
        if (currentAppFlavor == 3) {

            appId = PrefUtil.getInstance().getAppId();
            strSignKey = PrefUtil.getInstance().getAppSignKey();

        } else {
            switch (currentAppFlavor) {
                case 0:
                    appId = ZegoAppHelper.UDP_APP_ID;
                    break;
                case 1:
                    appId = ZegoAppHelper.INTERNATIONAL_APP_ID;
                    break;
                case 2:
                    appId = ZegoAppHelper.UDP_APP_ID;
                    PrefUtil.getInstance().setAppWebRtc(true);
                    break;
            }
            signKey = ZegoAppHelper.requestSignKey(appId);
            strSignKey = ZegoAppHelper.convertSignKey2String(signKey);
        }


        if (appId == 0 || TextUtils.isEmpty(strSignKey)) {
            appId = DEFAULT_ZEGO_APP_ID;
            signKey = ZegoAppHelper.requestSignKey(DEFAULT_ZEGO_APP_ID);
        } else {
            signKey = ZegoAppHelper.parseSignKeyFromString(strSignKey);
        }

        // 设置视频通话类型
        ZegoLiveRoom.setBusinessType(PrefUtil.getInstance().getBusinessType());

        boolean success = liveRoom.initSDK(appId, signKey);

        liveRoom.setLatencyMode(ZegoConstants.LatencyMode.Low3);


        if (!success) {
            Toast.makeText(this, R.string.vt_toast_init_sdk_failed, Toast.LENGTH_LONG).show();
        } else {

            int height = 0;
            int width = 0;

            int level = PrefUtil.getInstance().getLiveQuality();
            if (level < 0 || level > ZegoAvConfig.Level.SuperHigh) {
                config = new ZegoAvConfig(ZegoAvConfig.Level.High);
                config.setVideoBitrate(PrefUtil.getInstance().getLiveQualityBitrate());
                config.setVideoFPS(PrefUtil.getInstance().getLiveQualityFps());
                int resolutionLevel = PrefUtil.getInstance().getLiveQualityResolution();

                String resolutionText = getResources().getStringArray(R.array.zg_resolutions)[resolutionLevel];
                String[] strWidthHeight = resolutionText.split("x");
                height = Integer.parseInt(strWidthHeight[0].trim());
                width = Integer.parseInt(strWidthHeight[1].trim());
                config.setVideoEncodeResolution(width, height);
                config.setVideoCaptureResolution(width, height);

            } else {
                config = new ZegoAvConfig(level);
            }
            liveRoom.setAVConfig(config);
        }


    }

    public void reInitZegoSDK() {
        ZegoLiveRoom liveRoom = ZegoAppHelper.getLiveRoom();
        liveRoom.unInitSDK();
        setupZegoSDK();

        Toast.makeText(this, R.string.zg_toast_reinit_sdk_success, Toast.LENGTH_LONG).show();
    }
}
