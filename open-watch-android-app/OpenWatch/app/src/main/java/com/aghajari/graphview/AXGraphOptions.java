/*
 * Copyright (C) 2021 - Amir Hossein Aghajari
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


package com.aghajari.graphview;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;

public class AXGraphOptions {

    public static final float DEFAULT = Float.NEGATIVE_INFINITY;

    public AXGraphOptions(Context context){
        init(context);
    }

    protected void init(Context context){
        axisPaint = new Paint();
        axisPaint.setColor(Color.BLACK);
        axisPaint.setStrokeWidth(4);

        dividerPaint = new Paint();
        dividerPaint.setColor(Color.GRAY);
        dividerPaint.setStrokeWidth(2);

        gridLinePaint = new Paint();
        gridLinePaint.setColor(Color.rgb(240,240,240));
        gridLinePaint.setStrokeWidth(3);

        gridLine2Paint = new Paint();
        gridLine2Paint.setColor(Color.argb(150, 240,240,240));
        gridLine2Paint.setStrokeWidth(2);

        textPaint = new Paint();
        textPaint.setColor(Color.DKGRAY);
        textPaint.setTextSize(12 * context.getResources().getDisplayMetrics().density);
        textPaint.setAntiAlias(true);

        textBackgroundPaint = new Paint();
        textBackgroundPaint.setColor(Color.WHITE);
    }

    public PointF axis = null; // new PointF(DEFAULT,DEFAULT);
    public float xDividerIntervalInPx = 200;
    public float xDividerInterval = 1;
    public float yDividerIntervalInPx = 200;
    public float yDividerInterval = 1;

    // Ui
    public float dividerSize = 20;
    public boolean scrollEnabled = true;
    public float minZoom = 0.5f;
    public float maxZoom = 6f;

    public float minGraphX = DEFAULT;
    public float maxGraphX = DEFAULT;
    public float minGraphY = DEFAULT;
    public float maxGraphY = DEFAULT;

    public boolean drawAxisX = true;
    public boolean drawAxisY = true;
    public boolean drawAxisYDivider = false;
    public boolean drawAxisXDivider = false;
    public boolean drawGridXLines = true;
    public boolean drawGridYLines = true;
    public boolean drawXText = true;
    public boolean drawYText = true;

    public Paint axisPaint;
    public Paint dividerPaint;
    public Paint gridLinePaint;
    public Paint gridLine2Paint;
    public Paint textPaint;
    public Paint textBackgroundPaint;

    protected float lastGraphScale = 1;

    protected String getTextForXAxis(float x,int index){
        if (Math.round(x) == x){
            return String.valueOf(Math.round(x));
        }
        return String.valueOf(x);
    }

    protected String getTextForYAxis(float y,int index){
        if (Math.round(y) == y){
            return String.valueOf(Math.round(y));
        }
        return String.valueOf(y);
    }

    protected float getScaledDividerIntervalInPx(float dividerIntervalInPX, float scale){
        return dividerIntervalInPX * calculateScaleEffect(scale);
    }

    protected float getScaledDividerInterval(float dividerInterval, float scale){
        return dividerInterval * calculateScaleEffect(scale);
    }

    protected float calculateLastScaleEffect(){
        return calculateScaleEffect(lastGraphScale);
    }

    protected float calculateScaleEffect(float scale){
        this.lastGraphScale = scale;
        float s = 1;

        if (scale>1) {
            float scaledDividerInterval = xDividerIntervalInPx * scale;
            float c = 2;
            while (scaledDividerInterval / xDividerIntervalInPx >= c) {
                scaledDividerInterval -= xDividerIntervalInPx;
                s *= 2;
                c += 0.5f;
            }
        }else if (scale<1){
            float scaledDividerInterval = xDividerIntervalInPx / scale;
            float c = 2;
            while (scaledDividerInterval / xDividerIntervalInPx >= c) {
                scaledDividerInterval -= xDividerIntervalInPx;
                s /= 2;
                c += 0.5f;
            }
        }
        s = Math.max(s,0);
        return 1f/s;
    }
}
