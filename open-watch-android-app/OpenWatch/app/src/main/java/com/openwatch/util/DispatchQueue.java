package com.openwatch.util;

import android.os.Handler;
import android.os.Looper;

public class DispatchQueue extends Thread {

    private volatile Handler handler = null;

    public DispatchQueue(Runnable runnable) {
        super(runnable);
        start();
    }

    public void postRunnable(Runnable runnable, long delay) {
        clear();
        handler.postDelayed(runnable, delay);
    }

    public void clear() {
        try {
            handler.removeCallbacksAndMessages(null);
        } catch (Exception ignore) {
        }
    }

    public void recycle() {
        handler.getLooper().quit();
        interrupt();
    }

    @Override
    public void run() {
        Looper.prepare();
        handler = new Handler();
        super.run();
        Looper.loop();
    }
}