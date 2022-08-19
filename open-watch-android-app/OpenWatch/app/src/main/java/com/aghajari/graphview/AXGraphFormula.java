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

import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;

import java.util.ArrayList;
import java.util.List;

public abstract class AXGraphFormula {

    public AXGraphFormula(){
        init();
    }

    public abstract float function(float x);

    public boolean isInDomain(float x){
        return true;
    }

    public AXGraphPointType getPointType(float x, float y){
        return AXGraphPointType.CONTINUOUS;
    }

    protected void init(){
        graphPaint = new Paint();
        graphPaint.setColor(Color.BLUE);
        graphPaint.setStrokeWidth(strokeWidth);

        pointPaint = new Paint();
        pointPaint.setColor(Color.BLUE);
        pointPaint.setStyle(Paint.Style.FILL);
    }

    private boolean enabled = true;
    private float strokeWidth = 6;
    private Paint graphPaint;
    private Paint pointPaint;
    private float pointCircleRadius = 10;
    protected int graphBackgroundColor = Color.WHITE;
    private List<PointF> customPoints = null;
    protected AXGraphView graphView;

    protected void onAttachedToView(AXGraphView view){
        this.graphView = view;
    }

    protected void onDetachedFromView(AXGraphView view){
        this.graphView = null;
    }

    public void setCustomPoints(List<PointF> customPoints) {
        this.customPoints = customPoints;
    }

    public void setCustomFunctionPoints(List<Float> customPoints) {
        this.customPoints = new ArrayList<>();
        for (float x : customPoints){
            addCustomFunctionPoint(x);
        }
    }

    public List<PointF> getCustomPoints() {
        return customPoints;
    }

    public void addCustomPoint(float x,float y) {
        if (customPoints == null) customPoints = new ArrayList<>();
        customPoints.add(new PointF(x,y));
    }

    public void addCustomFunctionPoint(float x) {
        if (customPoints == null) customPoints = new ArrayList<>();
        customPoints.add(new PointF(x,function(x)));
    }

    public boolean isEnabled() {
        return enabled;
    }

    public float getStrokeWidth() {
        return strokeWidth;
    }

    public Paint getGraphPaint() {
        return graphPaint;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public void setGraphPaint(Paint graphPaint) {
        this.graphPaint = graphPaint;
    }

    public void setStrokeWidth(float strokeWidth) {
        this.strokeWidth = strokeWidth;
    }

    public float getPointCircleRadius() {
        return pointCircleRadius;
    }

    public void setPointCircleRadius(float pointCircleRadius) {
        this.pointCircleRadius = pointCircleRadius;
    }

    public Paint getPointPaint() {
        return pointPaint;
    }

    public void setPointPaint(Paint pointPaint) {
        this.pointPaint = pointPaint;
    }

    protected float sensitive(){
        return AXGraphView.SENSITIVE;
    }

    protected void drawPoint(AXGraphCanvas canvas, float x, float y, AXGraphPointType type){
        canvas.setRadiusFromAxis(false);

        switch (type){
            case CONTINUOUS:
                break;
            case SPECIAL:
            case CUSTOM:
            case FILL:
                getPointPaint().setStyle(Paint.Style.FILL);
                canvas.drawCircle(x,y, getPointCircleRadius()/canvas.getGraphScale(), getPointPaint());
                break;
            case EMPTY:
                canvas.drawCircle(x,y, getPointCircleRadius()/canvas.getGraphScale(),graphBackgroundColor,
                        getPointCircleRadius()/2/canvas.getGraphScale(), getPointPaint().getColor(), getPointPaint());
        }
    }

    protected boolean onDraw (AXGraphCanvas canvas){
        return false; // continue drawing function by AXGraphView
    }

    float transformX = 0 ,transformY = 0;
    float transformScaleX = 1f,transformScaleY = 1f;

    public float getTransformScaleX() {
        return 1/transformScaleX;
    }

    public float getTransformScaleY() {
        return transformScaleY;
    }

    public float getTransformX() {
        return -transformX;
    }

    public float getTransformY() {
        return transformY;
    }

    public void applyTransformMove(float x,float y){
        this.transformY = y;
        this.transformX = -x;
    }

    public void applyTransformScale (float x,float y){
        this.transformScaleY = y;
        this.transformScaleX = 1/x;
    }

    protected float applyFunction (float x){
        return transformScaleY * (function(transformScaleX * (x + transformX))) + transformY;
    }
}
