package com.aghajari.chart;

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PathMeasure;
import android.graphics.Rect;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.view.View;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

// https://github.com/Aghajari/SimpleChart
public class HRVChart extends View {

    public final static int DATA_COUNT = 30;

    final List<ChartData> list = new ArrayList<>(DATA_COUNT);
    final Rect textBounds = new Rect();

    float max = -1;
    int divideItemBy = 2;
    boolean drawHelperLine = true;

    float textLine = 12;
    float pointRadius = 3;
    float firstPointPadding = 16;
    float lastPointPadding = 16;

    final Paint helperPaint = new Paint();
    final Paint helperPaint2 = new Paint();
    final Paint helperPaint3 = new Paint();

    final Paint linePaint = new Paint();
    final Paint pointPaint = new Paint();
    final Paint pointPaint2 = new Paint();
    final TextPaint textPaint = new TextPaint();

    final Path linePath = new Path();
    final Path visiblePath = new Path();
    final float[] pos = new float[2];
    final float[] min_max = new float[2];
    PathMeasure pathMeasure;

    final HashMap<Float, Float> points = new LinkedHashMap<>(DATA_COUNT);

    float visible = 0f;

    public HRVChart(Context context) {
        this(context, null);
    }

    public HRVChart(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public HRVChart(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        helperPaint.setColor(Color.rgb(240, 240, 240));
        helperPaint.setStrokeWidth(3);

        helperPaint2.setColor(Color.argb(150, 240, 240, 240));
        helperPaint2.setStrokeWidth(2);

        helperPaint3.setColor(Color.BLACK);
        helperPaint3.setStrokeWidth(1.2f * context.getResources().getDisplayMetrics().density);

        linePaint.setStyle(Paint.Style.STROKE);
        linePaint.setColor(Color.RED);
        linePaint.setStrokeWidth(2 * context.getResources().getDisplayMetrics().density);

        pointPaint.setColor(Color.BLUE);
        pointPaint.setStyle(Paint.Style.STROKE);
        pointPaint.setStrokeWidth(linePaint.getStrokeWidth());

        pointPaint2.setColor(0xFFFCF8FD);
        pointPaint2.setStyle(Paint.Style.FILL);

        lastPointPadding *= context.getResources().getDisplayMetrics().density;
        firstPointPadding *= context.getResources().getDisplayMetrics().density;
        pointRadius *= context.getResources().getDisplayMetrics().density;
        textLine *= context.getResources().getDisplayMetrics().density;

        textPaint.setColor(Color.DKGRAY);
        textPaint.setTextSize(12 * context.getResources().getDisplayMetrics().scaledDensity);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    public void setFirstPointPadding(float firstPointPadding) {
        this.firstPointPadding = firstPointPadding;
        invalidate();
    }

    public void setLastPointPadding(float lastPointPadding) {
        this.lastPointPadding = lastPointPadding;
        invalidate();
    }

    public void setDivideItemBy(int divideItemBy) {
        this.divideItemBy = divideItemBy;
        invalidate();
    }

    public void setDrawHelperLine(boolean drawHelperLine) {
        this.drawHelperLine = drawHelperLine;
        invalidate();
    }

    public void setMaxValue(float max) {
        this.max = max;
        invalidate();
    }

    public void setPointColor(int color) {
        pointPaint.setColor(color);
        invalidate();
    }

    public void setLineColor(int color) {
        linePaint.setColor(color);
        invalidate();
    }

    public void setHelperLineColor(int color) {
        helperPaint.setColor(color);
        invalidate();
    }

    public void setHelperLineWidth(float width) {
        helperPaint.setStrokeWidth(width);
        invalidate();
    }

    public void setLineWidth(float width) {
        linePaint.setStrokeWidth(width);
        invalidate();
    }

    public void addData(float hr, float spo2) {
        list.add(new ChartData(hr, spo2));
        //invalidate();
    }

    public List<ChartData> getList() {
        return list;
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        fillMinMax();
        float minData = min_max[0], maxData = min_max[1];

        float pr = pointRadius * 2;
        float left = getPaddingLeft() + pr + getInnerPaddingLeft();
        float top = getPaddingTop() + pr;
        float right = getMeasuredWidth() - getPaddingRight() - pr;
        float bottom = getMeasuredHeight() - getPaddingBottom();
        float height = bottom - top;

        if (drawHelperLine) {
            int count = 4;
            int maxValueForText = (int) (minData + maxData);
            int stepValueForText = (int) (maxData / (count - 1));

            float helperLinePadding = height / (count - 1);
            float top2 = getPaddingTop() + pointRadius;

            int i = 0;
            while (true) {
                float l = left + i++ * helperLinePadding;
                canvas.drawLine(l, top2, l, bottom - pointRadius, helperPaint);

                int part = 5;
                float part_padding = helperLinePadding / part;
                for (int p = 1; p < part; p++) {
                    canvas.drawLine(l + p * part_padding, top2,
                            l + p * part_padding, bottom - pointRadius, helperPaint2);
                }

                if (l >= right)
                    break;
            }

            for (i = 0; i < count; i++) {
                final float t = top2 + i * helperLinePadding;
                canvas.drawLine(left, t, right, t, helperPaint);
                canvas.drawLine(left - textLine, t, left, t, helperPaint3);

                String value = i == count - 1 ? String.valueOf((int) minData) :
                        String.valueOf(maxValueForText - i * stepValueForText);

                float topPaddingForText = textBounds.height() / 2f - 4;
                if (i == 0)
                    topPaddingForText = textBounds.height();
                else if (i == count - 1)
                    topPaddingForText = 0;

                textPaint.getTextBounds(value, 0, value.length(), textBounds);
                canvas.drawText(value, left - textLine - textBounds.width() - 20,
                        t + topPaddingForText, textPaint);

                int part = 5;
                float part_padding = helperLinePadding / part;
                for (int p = 1; p < part; p++) {
                    float t2 = t + p * part_padding;
                    canvas.drawLine(left, t2, right, t2, helperPaint2);
                    canvas.drawLine(left - (textLine / 2f), t2, left, t2, helperPaint3);
                }
            }
        }

        if (visible == 0 || list.size() == 0)
            return;

        if (visible < 1f && pathMeasure != null) {
            visiblePath.reset();
            pathMeasure.getSegment(0f, pathMeasure.getLength() * visible, visiblePath, true);
            pathMeasure.getPosTan(pathMeasure.getLength() * visible, pos, null);
            canvas.drawPath(visiblePath, linePaint);

            for (Map.Entry<Float, Float> entry : points.entrySet()) {
                if (entry.getKey() > pos[0])
                    break;

                canvas.drawCircle(entry.getKey(), entry.getValue(), pointRadius, pointPaint2);
                canvas.drawCircle(entry.getKey(), entry.getValue(), pointRadius, pointPaint);
            }
        } else {
            canvas.drawPath(linePath, linePaint);

            for (Map.Entry<Float, Float> entry : points.entrySet()) {
                canvas.drawCircle(entry.getKey(), entry.getValue(), pointRadius, pointPaint2);
                canvas.drawCircle(entry.getKey(), entry.getValue(), pointRadius, pointPaint);
            }
        }
    }

    private void fillMinMax() {
        float maxData = max;
        float minData = Float.MAX_VALUE;
        if (maxData == -1) {
            maxData = 0;
            for (ChartData d : list) {
                maxData = Math.max(maxData, d.hr);
                minData = Math.min(minData, d.hr);
            }
        }

        if (list.size() != DATA_COUNT || maxData == minData) {
            minData = Math.min(minData, 70);
            maxData = Math.max(maxData, 130);
        } else if (maxData - minData < 50)
            maxData += 50;

        maxData = Math.max(1, maxData - minData);
        min_max[0] = minData;
        min_max[1] = maxData;
    }

    private void fillLinePath() {
        fillMinMax();
        float minData = min_max[0], maxData = min_max[1];

        float pr = pointRadius * 2;
        float left = getPaddingLeft() + pr + getInnerPaddingLeft();
        float top = getPaddingTop() + pr;
        float right = getMeasuredWidth() - getPaddingRight() - pr;
        float bottom = getMeasuredHeight() - getPaddingBottom();
        float height = bottom - top;

        float height2 = (int) (height - pr);

        float linePadding = height2 / (maxData + 1);

        left += firstPointPadding;
        right -= lastPointPadding;

        float width = right - left;
        final int count = list.size();
        float xPadding = (width - DATA_COUNT * pr) / (DATA_COUNT - 1);

        linePath.reset();
        points.clear();

        for (int i = 0; i < count; i++) {
            ChartData mData = list.get(i);
            float y = (maxData - mData.hr + minData) * linePadding + top;
            float x = left + (i * xPadding) + (i * pr);

            if (i == 0)
                linePath.moveTo(x, y);

            if (i + 1 < count) {
                ChartData nextData = list.get(i + 1);
                float y2 = (maxData - nextData.hr + minData) * linePadding + top;
                float x2 = left + ((i + 1) * xPadding) + ((i + 1) * pr);
                //canvas.drawLine(x, y, x2, y2, linePaint);
                linePath.lineTo(x2, y2);

            }
            points.put(x, y);
            //canvas.drawCircle(x, y, pointRadius, pointPaint);
        }
    }

    private float getInnerPaddingLeft() {
        textPaint.getTextBounds("9999", 0, 4, textBounds);
        return getContext().getResources().getDisplayMetrics().density * 36f
                + textBounds.width();
    }

    private ValueAnimator animator = null;

    public void startAnimation() {
        if (animator != null)
            animator.cancel();

        fillLinePath();
        visible = 0f;
        pathMeasure = new PathMeasure(linePath, false);

        invalidate();
        animator = ValueAnimator.ofFloat(0f, 1f);
        animator.setDuration((long) (pathMeasure.getLength() / (list.size() >= DATA_COUNT / 2 ? 2f : 1f)));
        animator.setStartDelay(300);
        animator.addUpdateListener(a -> {
            visible = (float) a.getAnimatedValue();
            invalidate();
        });
        animator.start();
    }

    public static class ChartData {
        float hr, spo2;

        public ChartData(float hr, float spo2) {
            this.hr = hr;
            this.spo2 = spo2;
        }

        public float getHr() {
            return hr;
        }

        public float getSpo2() {
            return spo2;
        }
    }
}