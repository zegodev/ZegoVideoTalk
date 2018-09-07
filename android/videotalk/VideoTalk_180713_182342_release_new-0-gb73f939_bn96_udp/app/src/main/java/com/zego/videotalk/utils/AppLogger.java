package com.zego.videotalk.utils;

import java.util.LinkedList;

/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 26/10/2017.
 */

public class AppLogger {

    static private AppLogger sInstance;

    final private LinkedList<String> mLogList = new LinkedList<>();

    private AppLogger() {
    }

    static public AppLogger getInstance() {
        if (sInstance == null) {
            synchronized (AppLogger.class) {
                if (sInstance == null) {
                    sInstance = new AppLogger();
                }
            }
        }
        return sInstance;
    }

    public void writeLog(String format, Object... args) {
        String message;

        if (args.length == 0) {
            message = format;
        } else {
            message = String.format(format, args);
        }

        mLogList.addFirst(String.format("%s %s", TimeUtil.getLogStr(), message));
        PrefUtil.getInstance().setLogData(mLogList);
    }

    public LinkedList<String> getAllLog() {
        return (LinkedList<String>) PrefUtil.getInstance().getLogData();
    }

    public void clearLog() {
        mLogList.clear();
        PrefUtil.getInstance().setLogData(mLogList);
    }
}
