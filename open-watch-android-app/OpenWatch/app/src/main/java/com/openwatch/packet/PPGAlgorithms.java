package com.openwatch.packet;

import android.content.Context;

import com.openwatch.util.Utils;

public class PPGAlgorithms {
    static {
        System.loadLibrary("native-lib");
    }

    public static final int RX_BUFFER_SIZE = 811;
    public static final int RX_BUFFER_CAPACITY = RX_BUFFER_SIZE + 101;
    public static final long HEART_REFRESH_TIME = 800; //ms

    public static final int SUM_PACKET_COUNT = 5;
    public static final int EACH_PACKET_SIZE = 800;

    public static final int SAMPLE_SIZE = 1000;

    public static native PPGData algorithm(Object[] bytes, int len, boolean gender, int age, double prevSpO2);

    public static native void terminate();

    public static native PPGData test(boolean gender, int age);

    public static PPGData algorithm(Context context, Object[] bytes, int len, double prevSpO2) {
        return algorithm(bytes, len,
                Utils.getGender(context),
                Utils.getAge(context),
                prevSpO2
        );
    }

    public static PPGData test(Context context) {
        return test(
                Utils.getGender(context),
                Utils.getAge(context)
        );
    }
}
