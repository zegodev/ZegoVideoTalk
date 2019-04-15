package com.zego.videotalk.adapter;

import android.app.Activity;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;

import com.zego.videotalk.R;
import com.zego.videotalk.ZegoAppHelper;
import com.zego.videotalk.ui.widgets.VideoLiveView;
import com.zego.videotalk.utils.PrefUtil;
import com.zego.zegoliveroom.constants.ZegoVideoViewMode;
import com.zego.zegoliveroom.entity.ZegoStreamInfo;

import java.lang.ref.SoftReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 26/10/2017.
 */

public class VideoLiveViewAdapter extends RecyclerView.Adapter {

    private SoftReference<Activity> mParentReference;
    private ArrayList<ZegoStreamInfo> mStreamList;
    private Map<String, CommonStreamQuality> mQualityMap;
    private int mItemWidth = 0;
    private OnItemClickListener onItemClickListener;


    public void setOnItemClickListener(OnItemClickListener onItemClickListener) {
        this.onItemClickListener = onItemClickListener;
    }

    public VideoLiveViewAdapter(Activity activity) {
        mParentReference = new SoftReference<>(activity);
        mStreamList = new ArrayList<>();
        mQualityMap = new HashMap<>();
        mItemWidth = activity.getResources().getDimensionPixelSize(R.dimen.vt_video_live_item_width);
    }

    public synchronized void remove(Object key) {
        if (mQualityMap != null) {
            mQualityMap.remove(key);
        }
    }

    public synchronized void put(String key, CommonStreamQuality value) {
        if (mQualityMap != null) {
            mQualityMap.put(key, value);
        }
    }


    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        VideoLiveView convertView = new VideoLiveView(parent.getContext(), false);
        RecyclerView.LayoutParams params = new RecyclerView.LayoutParams(mItemWidth, mItemWidth * 16 / 9);
        convertView.setLayoutParams(params);
        MyViewHolder viewHolder = new MyViewHolder(convertView);
        return viewHolder;
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, final int position) {

        final VideoLiveView liveView = ((MyViewHolder) holder).itemView;

        final ZegoStreamInfo streamInfo = mStreamList.get(position);
        if (TextUtils.isEmpty(streamInfo.userID) || TextUtils.equals(streamInfo.userID, PrefUtil.getInstance().getUserId())) {
            if (zegoMapView.get(streamInfo.streamID) == null || zegoMapView.get(streamInfo.streamID) != liveView.getTextureView().getId()) {
                // preview
                ZegoAppHelper.getLiveRoom().setPreviewView(liveView.getTextureView());
                ZegoAppHelper.getLiveRoom().setPreviewViewMode(ZegoVideoViewMode.ScaleAspectFit);
            }
        } else {
            if (zegoMapView.get(streamInfo.streamID) == null || zegoMapView.get(streamInfo.streamID) != liveView.getTextureView().getId()) {
                // play
                ZegoAppHelper.getLiveRoom().updatePlayView(streamInfo.streamID, liveView.getTextureView());
                ZegoAppHelper.getLiveRoom().setViewMode(ZegoVideoViewMode.ScaleAspectFit, streamInfo.streamID);

            }
        }

        liveView.setTag(R.id.view_video, position);

        liveView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (onItemClickListener != null) {
                    onItemClickListener.onItemClick(v, (int) v.getTag(R.id.view_video));
                }
            }
        });

        zegoMapView.put(streamInfo.streamID, liveView.getTextureView().getId());
        CommonStreamQuality commonStreamQuality = mQualityMap.get(streamInfo.streamID);
        if (commonStreamQuality != null) {

            liveView.setLiveQuality(commonStreamQuality.quality, commonStreamQuality.videoFps, commonStreamQuality.vkbps, commonStreamQuality.rtt, commonStreamQuality.pktLostRate);

        }

    }

    public interface OnItemClickListener {
        void onItemClick(View v, int position);
    }

    public synchronized Object getItem(int position) {
        return mStreamList.get(position);
    }

    public static class MyViewHolder extends RecyclerView.ViewHolder {
        VideoLiveView itemView;

        public MyViewHolder(VideoLiveView itemView) {
            super(itemView);
            this.itemView = itemView;

        }
    }

    /**
     * Get the row id associated with the specified position in the list.
     *
     * @param position The position of the item within the adapter's data set whose row id we want.
     * @return The id of the item at the specified position.
     */
    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public int getItemCount() {
        return mStreamList.size();
    }

    volatile Map<String, Integer> zegoMapView = new HashMap<>();

    public synchronized void setData(final ZegoStreamInfo[] streamList) {
        mStreamList.clear();
        for (ZegoStreamInfo streamInfo : streamList) {
            mStreamList.add(streamInfo);
        }
        notifyRefreshUI();
    }

    public synchronized ZegoStreamInfo getStream(int position) {

        return mStreamList.get(position);
    }

    public synchronized void addStream(final ZegoStreamInfo streamInfo) {
        mStreamList.add(streamInfo);
        notifyRefreshUI();
    }

    public synchronized void removeStream(String streamId) {
        if (TextUtils.isEmpty(streamId)) return;

        ZegoStreamInfo deleteStream = null;
        for (ZegoStreamInfo streamInfo : mStreamList) {
            if (TextUtils.equals(streamId, streamInfo.streamID)) {
                deleteStream = streamInfo;
                break;
            }
        }
        remove(streamId);
        if (deleteStream != null) {
            mStreamList.remove(deleteStream);
            notifyRefreshUI();
        }

    }

    public synchronized ArrayList<ZegoStreamInfo> getCurrentList() {
        return mStreamList;
    }

    public synchronized ZegoStreamInfo replace(final ZegoStreamInfo newStream, int position) {

        if (position < 0 || position >= mStreamList.size()) return null;

        ZegoStreamInfo oldStream = mStreamList.remove(position);
        remove(oldStream.streamID);
        ZegoStreamInfo _newStream = new ZegoStreamInfo();
        _newStream.streamID = newStream.streamID;
        _newStream.userID = newStream.userID;
        _newStream.userName = newStream.userName;
        _newStream.extraInfo = newStream.extraInfo;
        mStreamList.add(position, _newStream);
        zegoMapView.clear();
        notifyRefreshUI();
        return oldStream;
    }


    private void notifyRefreshUI() {

        if (mParentReference == null && mParentReference.get() != null) return;

        final Activity parent = mParentReference.get();
        parent.findViewById(android.R.id.content).post(new Runnable() {
            @Override
            public void run() {
                if (!parent.isFinishing()) {
                    notifyDataSetChanged();
                }
            }
        });
    }

    public synchronized void onPlayQualityUpdate(String streamId, final CommonStreamQuality commonStreamQuality) {
        put(streamId, commonStreamQuality);
        notifyRefreshUI();
    }

    public static class CommonStreamQuality {
        public double audioFps;
        public double videoFps;
        public double vkbps;
        public int rtt;
        public int pktLostRate;
        public int quality;
        public int width;
        public int height;
    }
}
