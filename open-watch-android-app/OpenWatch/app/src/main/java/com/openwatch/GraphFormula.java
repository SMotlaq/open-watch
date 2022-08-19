package com.openwatch;

import android.animation.ValueAnimator;

import com.aghajari.graphview.AXGraphFormula;
import com.aghajari.graphview.AXGraphView;
import com.openwatch.packet.PPGData;

import java.util.ArrayList;
import java.util.Arrays;

public class GraphFormula {

    private final ArrayList<PPGData> buffer = new ArrayList<>();
    private final AXGraphFormula f1, f2;

    GraphFormula(AXGraphView view) {
        f1 = create(true, 30000);
        f2 = create(false, 30000);
        view.addFormulas(f1, f2);
    }

    private AXGraphFormula create(boolean redData, int top) {
        return new AXGraphFormula() {

            public int getMs(float x) {
                return (int) Math.floor(x);
            }

            @Override
            public float function(float x) {
                if (buffer.size() == 0)
                    return Integer.MIN_VALUE;


                int data;
                if (!isInDomain(x)) {
                    int[] array = redData ? buffer.get(buffer.size() - 1).getRedData()
                            : buffer.get(buffer.size() - 1).getIrData();
                    data = array[buffer.get(buffer.size() - 1).getVisible()];
                } else {
                    int ms = getMs(x);
                    try {
                        int index = (int) Math.floor((ms * 1.0) / PPGData.SAMPLE_SIZE);
                        ms -= index * PPGData.SAMPLE_SIZE;
                        data = redData ? buffer.get(index).getRedData()[ms]
                                : buffer.get(index).getIrData()[ms];
                    } catch (Exception ignore) {
                        data = 0;
                    }
                }

                return data + top;
            }

            @Override
            public boolean isInDomain(float x) {
                int ms = getMs(x);
                if ((PPGData.SAMPLE_SIZE * buffer.size()) > ms) {
                    int index = (int) Math.floor((ms * 1.0) / PPGData.SAMPLE_SIZE);
                    if (index == buffer.size() - 1) {
                        ms -= index * PPGData.SAMPLE_SIZE;
                        return buffer.get(index).getVisible() >= ms;
                    }
                    return true;
                }
                return false;
            }
        };
    }

    public void updateColor(AXGraphView view, int[] colors) {
        f1.getGraphPaint().setColor(colors[1]);
        f2.getGraphPaint().setColor(colors[0]);
        view.invalidate();
    }

    public void addData(AXGraphView view, PPGData data) {
        if (buffer.size() >= Math.max(0, 4 / view.getOriginalScale()) && view.isPointVisible(PPGData.SAMPLE_SIZE * buffer.size()))
            view.animateX(-150);

        ValueAnimator animator = ValueAnimator.ofInt(1, PPGData.SAMPLE_SIZE - 1);
        animator.addUpdateListener(a -> {
            data.setVisible((int) animator.getAnimatedValue());
            view.invalidate();
        });
        animator.setDuration(1500);

        buffer.add(data);
        animator.start();
    }
}
