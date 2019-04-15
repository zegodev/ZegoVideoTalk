package com.zego.videotalk;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;

import com.zego.zegoliveroom.ZegoLiveRoom;

/**
 * Created by realuei on 2017/6/29.
 */

public class ZegoAppHelper {


    /**
     * 请提前在即构管理控制台获取 appID 与 appSign
     * Demo 默认使用 UDP 模式，请填充该模式下的 AppID 与 appSign,其他模式不需要可不用填
     * AppID 填写样式示例：1234567890l
     * appSign 填写样式示例：{(byte)0x00,(byte)0x01,(byte)0x02,(byte)0x03}
     **/

    static final public long RTMP_APP_ID = ;

    static final public long UDP_APP_ID = ;

    static final public long INTERNATIONAL_APP_ID = ;

    static final private byte[] RTMP_APP_SIGN = ;

    static final private byte[] UDP_APP_SIGN = ;

    static final private byte[] INTERNATIONAL_APP_SIGN = ;


    private static ZegoAppHelper sInstance = new ZegoAppHelper();

    private HandlerThread _backgroundThread = new HandlerThread("zego_background");
    private Handler backgroundHandler;

    private ZegoLiveRoom mZegoLiveRoom;

    private ZegoAppHelper() {
        _backgroundThread.start();
        backgroundHandler = new Handler(_backgroundThread.getLooper());
    }

    /**
     * 在后台执行一个任务，相比普通 Thread，可以对所有待执行的任务排序
     *
     * @param runnable
     */
    static public void postTask(Runnable runnable) {
        sInstance.backgroundHandler.post(runnable);
    }

    /**
     * 移除所有后台任务
     */
    static public void removeTask(Runnable runnable) {
        sInstance.backgroundHandler.removeCallbacks(runnable);
    }

    static public boolean isInternationalProduct(long appId) {
        return appId == INTERNATIONAL_APP_ID;
    }

    static public boolean isUdpProduct(long appId) {
        return appId == UDP_APP_ID;
    }

    static public boolean isRtmpProduct(long appId) {
        return appId == RTMP_APP_ID;
    }

    static public byte[] requestSignKey(long appId) {
        if (isRtmpProduct(appId)) {
            return RTMP_APP_SIGN;
        } else if (isUdpProduct(appId)) {
            return UDP_APP_SIGN;
        } else if (isInternationalProduct(appId)) {
            return INTERNATIONAL_APP_SIGN;
        }
        return null;
    }

    static public String getAppTitle(long appId, Context context) {
        String appTitle;
        if (appId == 1L) {  // RTMP
            appTitle = "RTMP";
        } else if (appId == 1082937486L) {   // UDP
            appTitle = "UDP";
        } else if (appId == 3873208266L) {   // International
            appTitle = "Int'l";
        } else {    // Custom
            appTitle = "Customize";
        }
        return appTitle;
    }

    static public String convertSignKey2String(byte[] appSign) {
        StringBuilder buffer = new StringBuilder();
        for (int b : appSign) {
            buffer.append("0x").append(Integer.toHexString((b & 0x000000FF) | 0xFFFFFF00).substring(6)).append(",");
        }
        buffer.setLength(buffer.length() - 1);
        return buffer.toString();
    }

    static public byte[] parseSignKeyFromString(String strAppSign) throws NumberFormatException {
        String[] keys = strAppSign.split(",");
        if (keys.length != 32) {
            throw new NumberFormatException("App Sign Key Illegal");
        }
        byte[] byteAppSign = new byte[32];
        for (int i = 0; i < 32; i++) {
            int data = Integer.valueOf(keys[i].trim().replace("0x", ""), 16);
            byteAppSign[i] = (byte) data;
        }
        return byteAppSign;
    }

    static public void saveLiveRoom(ZegoLiveRoom instance) {
        sInstance.mZegoLiveRoom = instance;
    }

    static public ZegoLiveRoom getLiveRoom() {
        if (sInstance.mZegoLiveRoom == null) {
            sInstance.mZegoLiveRoom = new ZegoLiveRoom();
        }
        return sInstance.mZegoLiveRoom;

    }
}
