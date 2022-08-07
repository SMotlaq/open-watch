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

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.view.Gravity;

public class AXGraphCanvas {

    private Canvas canvas;
    private AXGraphView graphView;
    private Paint defaultPaint;
    private boolean radiusFromAxis = false;
    private boolean applyTransform = false;
    private AXGraphFormula formula;

    AXGraphCanvas(AXGraphView graphView){
        this.graphView = graphView;
    }

    void setCanvas (Canvas canvas,AXGraphFormula formula){
        this.canvas = canvas;
        this.defaultPaint = formula.getGraphPaint();
        this.formula = formula;
        this.applyTransform = false;
    }

    public AXGraphView getGraphView() {
        return graphView;
    }

    public Canvas getCanvas(){
        return canvas;
    }

    public AXGraphOptions getGraphOptions() {
        return graphView.getGraphOptions();
    }

    public void setRadiusFromAxis(boolean radiusFromAxis) {
        this.radiusFromAxis = radiusFromAxis;
    }

    public void setApplyFormulaTransform(boolean applyTransform) {
        this.applyTransform = applyTransform;
    }

    public float findFormulaX(float rx) {
        if (applyTransform)
            return formula.getTransformScaleX() * (graphView.findFormulaX(rx) + + formula.getTransformX());
        else
            return graphView.findFormulaX(rx);
    }

    public float findFormulaY(float ry) {
        if (applyTransform)
            return formula.getTransformScaleY() * graphView.findFormulaY(ry) + formula.getTransformY();
        else
            return graphView.findFormulaY(ry);
    }

    public float findGraphX(float fx) {
        if (applyTransform)
            return graphView.findGraphX(formula.getTransformScaleX() * (fx+formula.getTransformX()));
        else
            return graphView.findGraphX(fx);
    }

    public float findGraphY(float fy) {
        if (applyTransform)
            return graphView.findGraphY(formula.getTransformScaleY() * fy +formula.getTransformY());
        else
            return graphView.findGraphY(fy);
    }

    public float findCanvasX(float fx) {
        if (applyTransform)
            return graphView.findCanvasX(formula.getTransformScaleX() * (fx+formula.getTransformX()));
        else
            return graphView.findCanvasX(fx);
    }

    public float findCanvasY(float fy) {
        if (applyTransform)
            return graphView.findCanvasY(formula.getTransformScaleY() * fy + formula.getTransformY());
        else
            return graphView.findCanvasY(fy);
    }

    public float getGraphScale() {
        return graphView.getGraphScale();
    }

    public float getGraphWidth() {
        return graphView.getGraphWidth();
    }

    public float getGraphHeight() {
        return graphView.getGraphHeight();
    }

    public void drawCircle(float cx, float cy, float radius){
        drawCircle(cx,cy,radius,defaultPaint);
    }

    public void drawCircle(float cx, float cy, float radius, Paint paint){
        canvas.drawCircle(findCanvasX(cx),findCanvasY(cy),radiusFromAxis?findGraphX(radius):radius,paint);
    }

    public void drawCircle(float cx, float cy, float radius, int circleColor, float borderWidth, int borderColor) {
        drawCircle(cx,cy,radius,circleColor,borderWidth,borderColor,defaultPaint);
    }

    public void drawCircle(float cx, float cy, float radius, int circleColor, float borderWidth, int borderColor, Paint p) {
        int saveColor = p.getColor();
        p.setColor(circleColor);
        Paint.Style saveStyle = p.getStyle();
        p.setStyle(Paint.Style.FILL);
        canvas.drawCircle(findCanvasX(cx), findCanvasY(cy), radiusFromAxis?findGraphX(radius):radius, p);
        if (borderWidth > 0) {
            p.setColor(borderColor);
            p.setStyle(Paint.Style.STROKE);
            float saveStrokeWidth = p.getStrokeWidth();
            p.setStrokeWidth(borderWidth);
            canvas.drawCircle(findCanvasX(cx), findCanvasY(cy), (radiusFromAxis?findGraphX(radius):radius) - (borderWidth / 2), p);
            p.setStrokeWidth(saveStrokeWidth);
        }
        p.setColor(saveColor);
        p.setStyle(saveStyle);
    }

    public void drawBorderCircle(float cx, float cy, float radius, float borderWidth, Paint p) {
        Paint.Style saveStyle = p.getStyle();
        if (borderWidth > 0) {
            p.setStyle(Paint.Style.STROKE);
            float saveStrokeWidth = p.getStrokeWidth();
            p.setStrokeWidth(borderWidth);
            canvas.drawCircle(findCanvasX(cx), findCanvasY(cy), (radiusFromAxis?findGraphX(radius):radius) - (borderWidth / 2), p);
            p.setStrokeWidth(saveStrokeWidth);
        }
        p.setStyle(saveStyle);
    }

    public void drawLine(float startX, float startY, float stopX, float stopY) {
        drawLine(startX,startY,stopX,stopY,defaultPaint);
    }

    public void drawLine(float startX, float startY, float stopX, float stopY,Paint paint) {
        canvas.drawLine(findCanvasX(startX),findCanvasY(startY),findCanvasX(stopX),findCanvasY(stopY),paint);
    }

    public void drawArc(float left, float top, float right, float bottom, float startAngle, float sweepAngle, boolean useCenter) {
        drawArc(new RectF(left,top,right,bottom),startAngle,sweepAngle,useCenter);
    }

    public void drawArc(float left, float top, float right, float bottom, float startAngle, float sweepAngle, boolean useCenter, Paint paint) {
        drawArc(new RectF(left,top,right,bottom),startAngle,sweepAngle,useCenter,paint);
    }

    public void drawArc(RectF oval, float startAngle, float sweepAngle, boolean useCenter) {
        drawArc(oval,startAngle,sweepAngle,useCenter,defaultPaint);
    }

    public void drawArc(RectF oval, float startAngle, float sweepAngle, boolean useCenter,Paint paint) {
        fixOval(oval);
        canvas.drawArc(oval,startAngle,sweepAngle,useCenter,paint);
    }

    public void drawPoint(float x, float y) {
        drawPoint(x,y,defaultPaint);
    }

    public void drawPoint(float x, float y,Paint paint) {
        canvas.drawPoint(findCanvasX(x),findCanvasY(y),paint);
    }

    public void drawText(String text, float x, float y) {
        drawText(text,x,y,getGraphOptions().textPaint);
    }

    public void drawText(String text, float x, float y, Paint paint) {
        canvas.drawText(text,findCanvasX(x),findCanvasY(y),paint);
    }

    public void drawText(String text, float x, float y,int gravity, Paint paint) {
        Rect rect = new Rect();
        paint.getTextBounds(text,0,text.length(),rect);

        float left;
        float top;

        switch (gravity & Gravity.HORIZONTAL_GRAVITY_MASK) {
            case Gravity.CENTER_HORIZONTAL:
                left = -rect.width()/2f;
                break;
            case Gravity.RIGHT:
                left = -rect.width();
                break;
            case Gravity.LEFT:
            default:
                left = 0;
        }
        switch (gravity & Gravity.VERTICAL_GRAVITY_MASK) {
            case Gravity.CENTER_VERTICAL:
                top = rect.height()/2f;
                break;
            case Gravity.TOP:
                top = rect.height();
                break;
            case Gravity.BOTTOM:
            default:
                top = 0;
        }

        canvas.drawText(text,findCanvasX(x) + left,findCanvasY(y) + top,paint);
    }

    void fixOval(RectF oval){
        oval.set(findCanvasX(0) + findGraphX(oval.left),
                findCanvasY(0) + findGraphY(oval.top),
                findCanvasX(0) + findGraphX(oval.right),
                findCanvasY(0) + findGraphY(oval.bottom));
    }

}
