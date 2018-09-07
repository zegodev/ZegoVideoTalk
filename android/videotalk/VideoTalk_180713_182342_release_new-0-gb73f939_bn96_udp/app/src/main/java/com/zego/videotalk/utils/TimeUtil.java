package com.zego.videotalk.utils;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * <p>Copyright © 2017 Zego. All rights reserved.</p>
 *
 * @author realuei on 24/10/2017.
 */

public class TimeUtil {
    static final private SimpleDateFormat sFormat = new SimpleDateFormat();

    static public String getNowTimeStr() {
        sFormat.applyPattern("yyMMddHHmmssSSS");
        return sFormat.format(new Date());
    }

    static public String getLogStr() {
        sFormat.applyPattern("HH:mm:ss.SSS");
        return sFormat.format(new Date());
    }
}
