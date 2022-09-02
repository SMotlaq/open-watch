package com.openwatch;

import android.animation.ValueAnimator;

import com.aghajari.graphview.AXGraphFormula;
import com.aghajari.graphview.AXGraphOptions;
import com.aghajari.graphview.AXGraphView;
import com.openwatch.packet.PPGAlgorithms;
import com.openwatch.packet.PPGData;

import java.util.ArrayList;
import java.util.Arrays;

public class GraphFormula {

    private final ArrayList<PPGData> buffer = new ArrayList<>();
    private final AXGraphFormula f1, f2;

    GraphFormula(AXGraphView view) {
        AXGraphOptions options = new AXGraphOptions(view.getContext());
        options.scrollEnabled = true;
        options.minGraphX = 0;
        options.maxGraphX = 1000 * 200f;
        options.minZoom = 1f;
        options.maxZoom = 1f;
        options.minGraphY = 0;
        options.drawYText = false;
        options.drawXText = false;

        options.xDividerInterval = 200f;
        options.yDividerInterval = 0.8f;
        options.axisPaint.setColor(options.gridLinePaint.getColor());
        options.xDividerIntervalInPx = 150;
        options.yDividerIntervalInPx = 150;

        view.setGraphOptions(options);
        double count = view.getMeasuredHeight() / options.yDividerIntervalInPx;
        double each = 1.0 / options.yDividerInterval;
        count -= each * 4;
        count /= 3;

        f1 = create(true, options.yDividerInterval * (Math.max(1, count) + count + 3 * each));
        f2 = create(false, options.yDividerInterval * (Math.max(1, count) + each));
        view.addFormulas(f1, f2);
    }

    private AXGraphFormula create(boolean redData, double top) {
        return new AXGraphFormula() {

            public int getMs(float x) {
                return (int) Math.floor(x);
            }

            @Override
            public float function(float x) {
                if (buffer.size() == 0)
                    return Integer.MIN_VALUE;


                double data;
                if (!isInDomain(x)) {
                    double[] array = redData ? buffer.get(buffer.size() - 1).getRedData()
                            : buffer.get(buffer.size() - 1).getIrData();
                    data = array[buffer.get(buffer.size() - 1).getVisible()];
                } else {
                    int ms = getMs(x);
                    try {
                        int index = (int) Math.floor((ms * 1.0) / PPGAlgorithms.SAMPLE_SIZE);
                        ms -= index * PPGAlgorithms.SAMPLE_SIZE;
                        data = redData ? buffer.get(index).getRedData()[ms]
                                : buffer.get(index).getIrData()[ms];
                    } catch (Exception ignore) {
                        data = 0;
                    }
                }

                return (float) (data + top);
            }

            @Override
            public boolean isInDomain(float x) {
                int ms = getMs(x);
                if ((PPGAlgorithms.SAMPLE_SIZE * buffer.size()) > ms) {
                    int index = (int) Math.floor((ms * 1.0) / PPGAlgorithms.SAMPLE_SIZE);
                    if (index == buffer.size() - 1) {
                        ms -= index * PPGAlgorithms.SAMPLE_SIZE;
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
        if (MainActivity.ONLY_PHONE_DEBUG) {
            System.out.println(Arrays.toString(data.getRedData()));
            System.out.println(data.getHR());
            System.out.println(data.getSpO2());
        }

        long duration = 1500 * PPGAlgorithms.SUM_PACKET_COUNT;

        if (buffer.size() >= Math.max(0, 1 / view.getOriginalScale())
                && view.isPointVisible(PPGAlgorithms.SAMPLE_SIZE * buffer.size()))
            view.animateX(-150 * PPGAlgorithms.SUM_PACKET_COUNT, duration);

        ValueAnimator animator = ValueAnimator.ofInt(1, PPGAlgorithms.SAMPLE_SIZE - 1);
        animator.addUpdateListener(a -> {
            data.setVisible((int) animator.getAnimatedValue());
            view.invalidate();
        });
        animator.setDuration(duration);

        buffer.add(data);
        animator.start();
    }

    public int size() {
        return buffer.size();
    }
}
