package com.zego.videotalk.utils;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.widget.Toast;



import java.io.File;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

/**
 * Copyright © 2016 Zego. All rights reserved.
 * des: 分享工具类.
 */

public class ShareUtils {

    private static ShareUtils sInstance;


    public static ShareUtils getInstance() {
        if (sInstance == null) {
            synchronized (ShareUtils.class) {
                if (sInstance == null) {
                    sInstance = new ShareUtils();
                }
            }
        }

        return sInstance;
    }



    static final public void sendFiles(File[] fileList, Activity activity) {
        File cacheDir = activity.getExternalCacheDir();
        if (cacheDir == null || !cacheDir.canWrite()) {
            cacheDir = activity.getCacheDir();
        }

        File[] oldLogCaches = cacheDir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String name) {
                return name.startsWith("zegoavlog") && name.endsWith(".zip");
            }
        });

        for (File cache : oldLogCaches) {
            cache.delete();
        }

        SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMdd-HHmmss");
        String zipFileName = String.format("zegoavlog_%s.zip", sdf.format(new Date()));
        File zipFile = new File(cacheDir, zipFileName);

        try {
            ZipUtil.zipFiles(fileList, zipFile, "Zego VideoTalk 日志信息");

            Intent shareIntent = new Intent(Intent.ACTION_SEND);
//            shareIntent.setDataAndType(Uri.fromFile(zipFile), "application/zip");//getMimeType(logFile));
            shareIntent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(zipFile));
            shareIntent.setType("application/zip");//getMimeType(logFile));
//            shareIntent.putExtra(Intent.EXTRA_TEXT, "ZegoLiveDemo5 日志信息");
            shareIntent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK
                    | Intent.FLAG_ACTIVITY_NEW_TASK
                    | Intent.FLAG_GRANT_READ_URI_PERMISSION);
            activity.startActivity(shareIntent);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
