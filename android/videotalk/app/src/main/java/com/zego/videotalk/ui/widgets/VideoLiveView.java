package com.zego.videotalk.ui.widgets;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.zego.videotalk.R;
import com.zego.zegoliveroom.ZegoLiveRoom;
import com.zego.zegoliveroom.constants.ZegoVideoViewMode;


/**
 * Copyright © 2017 Zego. All rights reserved.
 * des: 直播view.
 */
public class VideoLiveView extends RelativeLayout {

    /**
     * 推拉流颜色.
     */
    private TextView mTvQualityColor;

    /**
     * 推拉流质量.
     */
    private TextView mTvQuality;

    /**
     * 全屏.
     */
    private TextView mTvSwitchToFullScreen;

    /**
     * 分享.
     */
    private TextView mTvShare;

    /**
     * 编码方式
     */
    private TextView mTvEncoder;

    /**
     * 解码方式
     */
    private TextView mTvDecoder;

    /**
     * 用于渲染视频.
     */
    private TextureView mTextureView;

    private int[] mArrColor;

    private String[] mArrLiveQuality;

    private Resources mResources;

    private View mRootView;

    private ZegoLiveRoom mZegoLiveRoom = null;

    private Activity mActivityHost = null;

    public String streamID;

    /**
     * 推拉流质量.
     */
    private int mLiveQuality = 0;

    /**
     * 视频显示模式.
     */
    private int mVideoViewMode = ZegoVideoViewMode.ScaleAspectFit;

    /**
     * "切换全屏" 标记.
     */
    private boolean mNeedToSwitchFullScreen = false;

    private String mStreamID = null;

    private boolean mIsPublishView = false;

    private boolean mIsPlayView = false;

    private boolean mIsBigView = false;

    public VideoLiveView(Context context, boolean isBigView) {
        super(context, null, 0);

        mIsBigView = isBigView;
        initViews(context);
    }

    public VideoLiveView(Context context) {
        super(context);
    }

    public VideoLiveView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public VideoLiveView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.VideoLiveView, defStyleAttr, 0);
        mIsBigView = a.getBoolean(R.styleable.VideoLiveView_isBigView, false);
        a.recycle();

        initViews(context);
    }

    public void setZegoLiveRoom(ZegoLiveRoom zegoLiveRoom) {
        mZegoLiveRoom = zegoLiveRoom;
    }

    private void initViews(Context context) {
        if (context instanceof Activity) {
            mActivityHost = (Activity) context;
        }

        if (isInEditMode()) {
            mRootView = LayoutInflater.from(context).inflate(R.layout.vt_widget_live_view, this);
        } else {
            mResources = context.getResources();

            mArrColor = new int[4];
            mArrColor[0] = R.drawable.vt_shape_green_circle;
            mArrColor[1] = R.drawable.vt_shape_yellow_circle;
            mArrColor[2] = R.drawable.vt_shape_red_circle;
            mArrColor[3] = R.drawable.vt_shape_gray_circle;

            mArrLiveQuality = mResources.getStringArray(R.array.vt_live_quality);

            if (mIsBigView) {
                mRootView = LayoutInflater.from(context).inflate(R.layout.vt_widget_live_view_big, this);

                // 初始化编码解码TextView
                mTvEncoder = mRootView.findViewById(R.id.tv_encoder);
                mTvDecoder = mRootView.findViewById(R.id.tv_decoder);

                mTvSwitchToFullScreen = (TextView) mRootView.findViewById(R.id.tv_switch_full_screen);
                mTvSwitchToFullScreen.setOnClickListener(new OnClickListener() {
                    @Override
                    public void onClick(View v) {


                        if (mIsPlayView && mZegoLiveRoom != null && mActivityHost != null) {

                            mZegoLiveRoom.setViewMode(mVideoViewMode, mStreamID);

                            int currentOrientation = mActivityHost.getWindowManager().getDefaultDisplay().getRotation();
                            if (mVideoViewMode == ZegoVideoViewMode.ScaleAspectFit) {
                                if (currentOrientation == Surface.ROTATION_90 || currentOrientation == Surface.ROTATION_270) {
                                    mZegoLiveRoom.setViewRotation(Surface.ROTATION_90, mStreamID);
                                } else {
                                    mZegoLiveRoom.setViewRotation(Surface.ROTATION_0, mStreamID);
                                }
                            } else if (mVideoViewMode == ZegoVideoViewMode.ScaleAspectFill) {
                                if (currentOrientation == Surface.ROTATION_90 || currentOrientation == Surface.ROTATION_270) {
                                    mZegoLiveRoom.setViewRotation(Surface.ROTATION_0, mStreamID);
                                } else {
                                    mZegoLiveRoom.setViewRotation(Surface.ROTATION_90, mStreamID);
                                }

                            }
                        }
                        if (mVideoViewMode == ZegoVideoViewMode.ScaleAspectFill) {
                            setVideoViewMode(mNeedToSwitchFullScreen, ZegoVideoViewMode.ScaleAspectFit);
                        } else if (mVideoViewMode == ZegoVideoViewMode.ScaleAspectFit) {
                            setVideoViewMode(mNeedToSwitchFullScreen, ZegoVideoViewMode.ScaleAspectFill);
                        }
                    }
                });
            } else {
                mRootView = LayoutInflater.from(context).inflate(R.layout.vt_widget_live_view, this);
            }
        }
        mTextureView = (TextureView) mRootView.findViewById(R.id.textureView);
        mTvQualityColor = (TextView) mRootView.findViewById(R.id.tv_quality_color);
        mTvQuality = (TextView) mRootView.findViewById(R.id.tv_live_quality);
    }

    /**
     * 返回view是否为"空闲"状态.
     */
    public boolean isFree() {
        return TextUtils.isEmpty(mStreamID);
    }

    /**
     * 设置播放质量.
     */
    public void setLiveQuality(int quality) {
        if (quality >= 0 && quality <= 3) {
            mLiveQuality = quality;
            mTvQualityColor.setBackgroundResource(mArrColor[quality]);
        }
    }

    @SuppressLint("StringFormatMatches")
    public void setLiveQuality(int quality, double videoFPS, double videoBitrate, int videoRtt, int videoPktLostRate) {
        setLiveQuality(quality);
        mTvQuality.setText(mResources.getString(R.string.vt_live_quality_fps_and_bitrate, videoFPS, videoBitrate, videoRtt, videoPktLostRate));
    }

    /**
     * 设置当前的编码形式
     * @param isHardware 是否硬件编码
     */
    public void setEncoderFormat(boolean isHardware) {
        if (mIsBigView) {
            mTvEncoder.setText(isHardware ? R.string.hardware_encode : R.string.software_encode);
        }
    }

    /**
     * 设置当前的解码形式
     * @param isHardware 是否硬件解码
     */
    public void setDecoderFormat(boolean isHardware) {
        if (mIsBigView) {
            mTvDecoder.setText(isHardware ? R.string.hardware_decode : R.string.software_decode);
        }
    }


    /**
     * 设置mode.
     */
    public void setVideoViewMode(boolean needToSwitchFullScreen, int mode) {
        mNeedToSwitchFullScreen = needToSwitchFullScreen;
        mVideoViewMode = mode;

        if (mTvSwitchToFullScreen != null) {
            if (mNeedToSwitchFullScreen) {
                mTvSwitchToFullScreen.setVisibility(View.VISIBLE);

                if (mode == ZegoVideoViewMode.ScaleAspectFill) {
                    // 退出全屏
                    mTvSwitchToFullScreen.setText(R.string.vt_btn_exit_fullscreen);
                } else if (mode == ZegoVideoViewMode.ScaleAspectFit) {
                    // 全屏显示
                    mTvSwitchToFullScreen.setText(R.string.vt_btn_fullscreen);
                }
            } else {
                mTvSwitchToFullScreen.setVisibility(View.INVISIBLE);
            }
        }
    }

    public int getLiveQuality() {
        return mLiveQuality;
    }

    public TextureView getTextureView() {
        return mTextureView;
    }

    public boolean isNeedToSwitchFullScreen() {
        return mNeedToSwitchFullScreen;
    }

    public int getVideoViewMode() {
        return mVideoViewMode;
    }

    public void setStreamID(String streamID) {
        mStreamID = streamID;
    }

    public String getStreamID() {
        return mStreamID;
    }

    public boolean isPublishView() {
        return mIsPublishView;
    }

    public boolean isPlayView() {
        return mIsPlayView;
    }

    public void setPublishView(boolean publishView) {
        mIsPublishView = publishView;
    }

    public void setPlayView(boolean playView) {
        mIsPlayView = playView;
    }
}
