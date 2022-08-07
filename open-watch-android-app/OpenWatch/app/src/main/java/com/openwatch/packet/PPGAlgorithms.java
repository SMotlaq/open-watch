package com.openwatch.packet;

public class PPGAlgorithms {
    static {
        System.loadLibrary("native-lib");
    }

    public static native PPGData test(Object[] bytes, int len);

}
