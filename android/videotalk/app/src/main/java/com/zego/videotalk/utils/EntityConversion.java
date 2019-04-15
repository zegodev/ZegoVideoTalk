package com.zego.videotalk.utils;

import com.zego.videotalk.adapter.VideoLiveViewAdapter;
import com.zego.zegoliveroom.entity.ZegoPlayStreamQuality;
import com.zego.zegoliveroom.entity.ZegoPublishStreamQuality;

/**
 * Created by zego on 2019/1/14.
 */

public class EntityConversion {


    public static VideoLiveViewAdapter.CommonStreamQuality publishQualityToCommonStreamQuality(ZegoPublishStreamQuality zegoPlayStreamQuality) {
        VideoLiveViewAdapter.CommonStreamQuality commonStreamQuality = new VideoLiveViewAdapter.CommonStreamQuality();
        commonStreamQuality.audioFps = zegoPlayStreamQuality.anetFps;
        commonStreamQuality.videoFps = zegoPlayStreamQuality.vnetFps;
        commonStreamQuality.rtt = zegoPlayStreamQuality.rtt;
        commonStreamQuality.vkbps = zegoPlayStreamQuality.vkbps;
        commonStreamQuality.pktLostRate = zegoPlayStreamQuality.pktLostRate;
        commonStreamQuality.quality = zegoPlayStreamQuality.quality;
        commonStreamQuality.width = zegoPlayStreamQuality.width;
        commonStreamQuality.height = zegoPlayStreamQuality.height;

        return commonStreamQuality;
    }

    public static VideoLiveViewAdapter.CommonStreamQuality playQualityToCommonStreamQuality(ZegoPlayStreamQuality zegoPlayStreamQuality) {
        VideoLiveViewAdapter.CommonStreamQuality commonStreamQuality = new VideoLiveViewAdapter.CommonStreamQuality();
        commonStreamQuality.audioFps = zegoPlayStreamQuality.anetFps;
        commonStreamQuality.videoFps = zegoPlayStreamQuality.vnetFps;
        commonStreamQuality.rtt = zegoPlayStreamQuality.rtt;
        commonStreamQuality.vkbps = zegoPlayStreamQuality.vkbps;
        commonStreamQuality.pktLostRate = zegoPlayStreamQuality.pktLostRate;
        commonStreamQuality.quality = zegoPlayStreamQuality.quality;
        commonStreamQuality.width = zegoPlayStreamQuality.width;
        commonStreamQuality.height = zegoPlayStreamQuality.height;

        return commonStreamQuality;
    }




}
