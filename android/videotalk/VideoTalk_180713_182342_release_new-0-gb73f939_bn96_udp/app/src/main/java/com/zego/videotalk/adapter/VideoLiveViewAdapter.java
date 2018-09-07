package com.zego.videotalk.adapter;

import android.app.Activity;
import android.text.TextUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ListView;

import com.zego.videotalk.R;
import com.zego.videotalk.ZegoAppHelper;
import com.zego.videotalk.ui.widgets.VideoLiveView;
import com.zego.videotalk.utils.PrefUtil;
import com.zego.zegoliveroom.ZegoLiveRoom;
import com.zego.zegoliveroom.constants.ZegoVideoViewMode;
import com.zego.zegoliveroom.entity.ZegoStreamInfo;
import com.zego.zegoliveroom.entity.ZegoStreamQuality;

import java.lang.ref.SoftReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 26/10/2017.
 */

public class VideoLiveViewAdapter extends BaseAdapter {

    private SoftReference<Activity> mParentReference;
    private ArrayList<ZegoStreamInfo> mStreamList;
    private Map<String, ZegoStreamQuality> mQualityMap;
    private int mItemWidth = 0;
    volatile private List<View> videoLiveViewList = new ArrayList<>();

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

    public synchronized void put(String key, ZegoStreamQuality value) {
        if (mQualityMap != null) {
            mQualityMap.put(key, value);
        }
    }

    /**
     * How many items are in the data set represented by this Adapter.
     *
     * @return Count of items.
     */
    @Override
    public int getCount() {
        return mStreamList.size();
    }

    /**
     * Get the data item associated with the specified position in the data set.
     *
     * @param position Position of the item whose data we want within the adapter's
     *                 data set.
     * @return The data at the specified position.
     */
    @Override
    public Object getItem(int position) {
        return mStreamList.get(position);
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

    /**
     * Get a View that displays the data at the specified position in the data set. You can either
     * create a View manually or inflate it from an XML layout file. When the View is inflated, the
     * parent View (GridView, ListView...) will apply default layout parameters unless you use
     * {@link LayoutInflater#inflate(int, ViewGroup, boolean)}
     * to specify a root view and to prevent attachment to the root.
     *
     * @param position    The position of the item within the adapter's data set of the item whose view
     *                    we want.
     * @param convertView The old view to reuse, if possible. Note: You should check that this view
     *                    is non-null and of an appropriate type before using. If it is not possible to convert
     *                    this view to display the correct data, this method can create a new view.
     *                    Heterogeneous lists can specify their number of view types, so that this View is
     *                    always of the right type (see {@link #getViewTypeCount()} and
     *                    {@link #getItemViewType(int)}).
     * @param parent      The parent that this view will eventually be attached to
     * @return A View corresponding to the data at the specified position.
     */
    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = new VideoLiveView(parent.getContext(), false);
            GridView.LayoutParams params = new GridView.LayoutParams(mItemWidth, mItemWidth * 16 / 9);
            convertView.setLayoutParams(params);
        }

        VideoLiveView liveView = (VideoLiveView) convertView;

        ZegoLiveRoom liveRoom = ZegoAppHelper.getLiveRoom();
        ZegoStreamInfo streamInfo = mStreamList.get(position);
        if (TextUtils.isEmpty(streamInfo.userID) || TextUtils.equals(streamInfo.userID, PrefUtil.getInstance().getUserId())) {
            // preview
            liveRoom.setPreviewView(liveView.getTextureView());
            liveRoom.setPreviewViewMode(ZegoVideoViewMode.ScaleAspectFit);
        } else {
            // play
            liveRoom.updatePlayView(streamInfo.streamID, liveView.getTextureView());
            liveRoom.setViewMode(ZegoVideoViewMode.ScaleAspectFit, streamInfo.streamID);
        }

        ZegoStreamQuality zegoStreamQuality = mQualityMap.get(streamInfo.streamID);
        if (zegoStreamQuality != null) {

            liveView.setLiveQuality(zegoStreamQuality.quality, zegoStreamQuality.videoFPS, zegoStreamQuality.videoBitrate, zegoStreamQuality.rtt, zegoStreamQuality.pktLostRate);

        }

        return convertView;
    }

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
        Log.e("====", "pro"+position);
        if (position < 0 || position >= mStreamList.size()) return null;

        ZegoStreamInfo oldStream = mStreamList.remove(position);
        remove(oldStream.streamID);
        ZegoStreamInfo _newStream = new ZegoStreamInfo();
        _newStream.streamID = newStream.streamID;
        _newStream.userID = newStream.userID;
        _newStream.userName = newStream.userName;
        _newStream.extraInfo = newStream.extraInfo;
        mStreamList.add(position, _newStream);

        notifyRefreshUI();
        return oldStream;
    }

    public void notifyDataSetChanged() {
        videoLiveViewList.clear();
        super.notifyDataSetChanged();
    }

    private void notifyRefreshUI() {
        if (mParentReference == null && mParentReference.get() != null) return;

        final Activity parent = mParentReference.get();
        parent.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!parent.isFinishing()) {
                    notifyDataSetChanged();
                }
            }
        });
    }

    public synchronized void onPlayQualityUpdate(String streamId, final ZegoStreamQuality zegoStreamQuality) {
        put(streamId, zegoStreamQuality);
        notifyRefreshUI();

    }
}
