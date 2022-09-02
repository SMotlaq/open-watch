package com.openwatch.util;

import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.view.View;
import android.widget.FrameLayout;

public class HRVMorseCode {

    private static final long DELAY = 100;
    private static final long DOT_DELAY = DELAY;
    private static final long UNDERLINE_DELAY = DOT_DELAY * 2;
    private static final long CHARACTER_DELAY = UNDERLINE_DELAY;

    private static final String HRV_MORSE_CODE = ".... .-. ...-";

    public static View create(FrameLayout parent) {
        View view = new View(parent.getContext());
        int size = (int) (parent.getResources().getDisplayMetrics().density * 6);
        FrameLayout.LayoutParams lp = new FrameLayout.LayoutParams(size / 2, size / 2);
        lp.topMargin = Utils.getStatusBarHeight(parent.getContext()) + size;
        lp.leftMargin = size * 6;
        view.setVisibility(View.GONE);

        GradientDrawable gd = new GradientDrawable();
        gd.setColor(Color.BLACK);
        gd.setCornerRadius(1000);
        view.setBackground(gd);
        parent.addView(view, lp);

        view.postDelayed(() -> tick(view, 0), 300);
        return view;
    }

    private static void tick(final View view, final int index) {
        view.setVisibility(View.GONE);

        if (view.getParent() == null || index >= HRV_MORSE_CODE.length())
            return;

        char c = HRV_MORSE_CODE.charAt(index);

        int v = c == ' ' ? View.GONE : View.VISIBLE;

        final long delay;
        switch (c) {
            case '.':
                delay = DOT_DELAY;
                break;
            case '_':
                delay = UNDERLINE_DELAY;
                break;
            case ' ':
                delay = CHARACTER_DELAY;
                break;
            default:
                delay = 0;
        }

        view.postDelayed(() -> {
            view.setVisibility(v);
            view.postDelayed(() -> tick(view, index + 1), delay);
        }, DELAY);
    }
}
