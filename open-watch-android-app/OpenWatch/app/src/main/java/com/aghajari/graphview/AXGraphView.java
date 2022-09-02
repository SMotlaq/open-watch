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

import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.os.Build;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.animation.LinearInterpolator;

import androidx.annotation.RequiresApi;
import androidx.core.view.GestureDetectorCompat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.aghajari.graphview.AXGraphOptions.DEFAULT;

/**
 * @author Amir Hossein Aghajari
 * @version 1.1.0
 */
public class AXGraphView extends View {

    public static final float EPSILON = 1f;
    public static final float SENSITIVE = 1f;

    private List<AXGraphFormula> formulas = new ArrayList<>();
    private AXGraphOptions options;
    protected float graphScale = 1;
    protected float[] paintsStrokeWidth = new float[5];
    protected PointF axis;
    protected int mdc = 4;
    private int bg = Color.WHITE;
    private AXGraphCanvas graphCanvas;

    float[] matrixValues = new float[9];
    float dy, dx;
    float xDIPx, xDI, yDIPx, yDI;
    private List<CirclePoints> drawPoints = new ArrayList<>();
    private List<Float> mainPoints = new ArrayList<>();
    float centerY, centerX;
    private RectF graphSize = new RectF();

    private ScaleGestureDetector mScaleGestureDetector;
    private GestureDetectorCompat mGestureDetector;
    private float scale = 1f;
    private float org_scale = 1f;
    private PointF start = new PointF();
    private PointF mid = new PointF();
    private float distanceX = 0f;
    private float distanceY = 0f;

    private RectF contentSize;
    private Matrix matrix = new Matrix();
    private Matrix matrixInverse = new Matrix();
    private Matrix savedMatrix = new Matrix();
    private MatrixAnimation animation;
    private long animationDuration = 250;

    /**
     * The scale listener, used for handling multi-finger scale gestures.
     */
    private final ScaleGestureDetector.OnScaleGestureListener mScaleGestureListener = new ScaleGestureDetector.SimpleOnScaleGestureListener() {

        float fx;

        @Override
        public boolean onScaleBegin(ScaleGestureDetector scaleGestureDetector) {
            xDI = options.xDividerInterval / org_scale;
            matrix.getValues(matrixValues);
            fx = findFormulaX(matrixValues[Matrix.MTRANS_X] + getMeasuredWidth() / 2f);
            xDI = options.xDividerInterval;

            if (scaleGestureDetector.getCurrentSpan() > 10f) {
                savedMatrix.set(matrix);
                mid.set(scaleGestureDetector.getFocusX(), scaleGestureDetector.getFocusY());
            }
            return true;
        }

        @Override
        public boolean onScale(ScaleGestureDetector scaleGestureDetector) {
            scale = scaleGestureDetector.getScaleFactor();
            float oldScale = org_scale;
            org_scale = Math.max(Math.min(6f, org_scale * scale), 1f);
            if (oldScale != org_scale) {
                xDI = options.xDividerInterval / org_scale;

                matrix.getValues(matrixValues);
                float rx = findGraphX(fx);
                float dx = rx - getMeasuredWidth() / 2f;
                matrixValues[Matrix.MTRANS_X] = Math.min(0f, dx);
                matrix.setValues(matrixValues);
                xDI = options.xDividerInterval;
            }
            return true;
        }
    };

    /**
     * The gesture listener, used for handling simple gestures such as double touches, scrolls,
     * and flings.
     */
    private final GestureDetector.SimpleOnGestureListener mGestureListener = new GestureDetector.SimpleOnGestureListener() {
        @Override
        public boolean onDown(MotionEvent event) {
            savedMatrix.set(matrix);
            start.set(event.getX(), event.getY());
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float dX, float dY) {
            setupTranslation(dX, dY);
            matrix.postTranslate(distanceX, distanceY);
            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent e) {
            return AXGraphView.this.onDoubleTap(e) || super.onDoubleTap(e);
        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            return true;
        }
    };

    protected boolean onDoubleTap(MotionEvent e) {
        if (animationDuration < 0)
            return false;
        else if (graphScale != 1)
            return resetOnDoubleTap(e);
        else
            return zoomOnDoubleTap(e);
    }

    protected boolean resetOnDoubleTap(MotionEvent e) {
        postMatrixAnimation(createDefaultMatrix(), animationDuration);
        return true;
    }

    protected boolean zoomOnDoubleTap(MotionEvent e) {
        float scale = Math.min(2f, options != null ? options.maxZoom : 2f);
        setGraphScale(scale, e.getX(), e.getY());

        return true;
    }

    public AXGraphView(Context context) {
        super(context);
        init();
    }

    public AXGraphView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public AXGraphView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public AXGraphView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        init();
    }

    private void init() {
        graphCanvas = new AXGraphCanvas(this);
        mScaleGestureDetector = new ScaleGestureDetector(getContext(), mScaleGestureListener);
        mGestureDetector = new GestureDetectorCompat(getContext(), mGestureListener);
    }

    public void setGraphOptions(AXGraphOptions options) {
        this.options = options;
        xDIPx = options.xDividerIntervalInPx;
        xDI = options.xDividerInterval;
        yDIPx = options.yDividerIntervalInPx;
        yDI = options.yDividerInterval;
        paintsStrokeWidth[0] = options.axisPaint.getStrokeWidth();
        paintsStrokeWidth[1] = options.dividerPaint.getStrokeWidth();
        paintsStrokeWidth[2] = options.gridLinePaint.getStrokeWidth();
        paintsStrokeWidth[4] = options.gridLine2Paint.getStrokeWidth();
        paintsStrokeWidth[3] = options.textPaint.getTextSize() / getContext().getResources().getDisplayMetrics().density;
        onSizeChanged(getWidth(), getHeight(), 0, 0);
    }

    public AXGraphOptions getGraphOptions() {
        return options;
    }

    public float findFormulaX(float rx) {
        return rx * xDI / xDIPx;
    }

    public float findFormulaY(float ry) {
        return ry * yDI / yDIPx;
    }

    public float findGraphX(float fx) {
        return fx / xDI * xDIPx;
    }

    public float findGraphY(float fy) {
        return fy * yDIPx / yDI;
    }

    public float findCanvasX(float fx) {
        return findGraphX(fx) + centerX;
    }

    public float findCanvasY(float fy) {
        return centerY - findGraphY(fy);
    }

    private void findAxisPosition() {
        //float x = options.axis == null || options.axis.x == DEFAULT ? getGraphWidth() / 2f : (getGraphWidth() / 2f) + options.axis.x;
        float x = options.axis == null || options.axis.x == DEFAULT ? (-graphSize.left) : (-graphSize.left) + options.axis.x;
        //float y = options.axis == null || options.axis.y == DEFAULT ? getGraphHeight() / 2f : (getGraphHeight() / 2f) + options.axis.y;
        float y = options.axis == null || options.axis.y == DEFAULT ? (-graphSize.top) : (-graphSize.top) + options.axis.y;
        axis = new PointF(x, y);

        centerY = axis.y;
        centerX = axis.x;
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        if (options == null) return;

        // CUSTOMIZED -> WON'T SCROLL ON Y axis
        options.axis = new PointF(-w / 4f, h / 4f);
        options.maxGraphY = h / options.yDividerIntervalInPx;
        options.minGraphY = 0;

        //if (contentSize == null) {
        PointF pointF = findContentSize(w, h);
        setMaxScrollSize(w, h, pointF.x, pointF.y);
        //}
        findAxisPosition();
        invalidate();
    }

    private PointF findContentSize(int w, int h) {
        if (options == null) {
            graphSize.set(-w, -h, w, h);
            findAxisDiff();
            return new PointF(w * 2, h * 2);
        } else if (!options.scrollEnabled) {
            graphSize.set(-w / 2f, -h / 2f, w / 2f, h / 2f);
            findAxisDiff();
            return new PointF(w, h);
        }

        if (options.maxGraphX != DEFAULT && options.minGraphX != DEFAULT) {
            graphSize.left = findGraphX(options.minGraphX);
            graphSize.right = findGraphX(options.maxGraphX);
        } else {
            graphSize.left = -w;
            graphSize.right = w;
        }

        if (options.maxGraphY != DEFAULT && options.minGraphY != DEFAULT) {
            graphSize.top = findGraphY(-options.maxGraphY);
            graphSize.bottom = findGraphY(-options.minGraphY);
        } else {
            graphSize.top = -h;
            graphSize.bottom = h;
        }
        findAxisDiff();

        return new PointF(graphSize.width(), graphSize.height());
    }

    private void findAxisDiff() {
        if (options.axis != null && options.axis.x != DEFAULT) {
            float x_dif = findGraphX(options.axis.x * options.xDividerInterval / options.xDividerIntervalInPx);
            graphSize.left += x_dif;
            graphSize.right += x_dif;
        }

        if (options.axis != null && options.axis.y != DEFAULT) {
            float y_dif = findGraphY(options.axis.y * options.yDividerInterval / options.yDividerIntervalInPx);
            graphSize.top += y_dif;
            graphSize.bottom += y_dif;
        }
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (options == null) return;
        canvas.drawColor(0xFFFCF8FD);
        matrix.getValues(matrixValues);

        if (options.scrollEnabled) {
            graphScale = matrixValues[Matrix.MSCALE_X];
            dy = matrixValues[Matrix.MTRANS_Y];
            dx = matrixValues[Matrix.MTRANS_X];
        } else {
            dx = 0;
            dy = 0;
            graphScale = 1;
        }
        canvas.translate(dx, dy);

        canvas.save();
        float org_s = graphScale;
        graphScale = org_scale;
        canvas.scale(graphScale, graphScale);


        final float dx = -this.dx;

        if (axis == null) findAxisPosition();
        xDIPx = options.getScaledDividerIntervalInPx(options.xDividerIntervalInPx, graphScale);
        yDIPx = options.getScaledDividerIntervalInPx(options.yDividerIntervalInPx, graphScale);
        xDI = options.getScaledDividerInterval(options.xDividerInterval, graphScale);
        yDI = options.getScaledDividerInterval(options.yDividerInterval, graphScale);

        float ls = options.dividerSize / 2;
        float text_ls = options.dividerSize / graphScale;
        updatePaints();


        float min_display_x = findFormulaX((dx / graphScale) - centerX) - 0.5f;
        float max_display_x = findFormulaX(((dx + getWidth()) / graphScale) - centerX) + 0.5f;
        float min_display_y = findFormulaY(((-dy) / graphScale) - centerY) - 0.5f;
        float max_display_y = findFormulaY((((-dy) + getHeight()) / graphScale) - centerY) + 0.5f;

        mainPoints.clear();
        if (options.drawAxisXDivider || options.drawGridXLines) {
            int xCount = (int) (centerX / xDIPx) * mdc;
            for (int i = 1; i <= xCount; i++) {
                if (min_display_x > -i * xDI || -i * xDI > max_display_x) continue;

                float xSelection = i * xDIPx;
                mainPoints.add(-i * xDI);

                if (options.drawGridXLines)
                    canvas.drawLine(centerX - xSelection, Integer.MIN_VALUE,
                            centerX - xSelection, Integer.MAX_VALUE, options.gridLinePaint);

                if (options.drawAxisXDivider)
                    canvas.drawLine(centerX - xSelection, centerY - ls,
                            centerX - xSelection, centerY + ls, options.dividerPaint);

                //if (options.drawXText)
                //    drawTextX(canvas,centerX,centerY,text_ls,-xSelection,-i);
            }

            xCount = (int) ((getGraphWidth() - centerX) / xDIPx) * mdc;
            for (int i = 1; i <= xCount; i++) {
                if (min_display_x > i * xDI || i * xDI > max_display_x) continue;

                float xSelection = i * xDIPx;
                mainPoints.add(i * xDI);

                if (options.drawGridXLines) {
                    canvas.drawLine(centerX + xSelection, Integer.MIN_VALUE,
                            centerX + xSelection, Integer.MAX_VALUE, options.gridLinePaint);

                    int part = 5;
                    float part_padding = (xSelection - (i + 1) * xDIPx) / part;
                    for (int p = 1; p < part; p++) {
                        canvas.drawLine(centerX + xSelection + p * part_padding, Integer.MIN_VALUE,
                                centerX + xSelection + p * part_padding, Integer.MAX_VALUE, options.gridLine2Paint);
                    }
                }
                if (options.drawAxisXDivider)
                    canvas.drawLine(centerX + xSelection, centerY - ls,
                            centerX + xSelection, centerY + ls, options.dividerPaint);

                //if (options.drawXText)
                //   drawTextX(canvas,centerX,centerY,text_ls,xSelection,i);
            }
        }

        if (options.drawAxisYDivider || options.drawGridYLines) {
            int yCount = (int) (centerY / yDIPx) * mdc;
            for (int i = 1; i <= yCount; i++) {
                if (min_display_y > -i * yDI || -i * yDI > max_display_y) continue;

                float ySelection = i * yDIPx;
                if (options.drawGridYLines) {
                    canvas.drawLine(Integer.MIN_VALUE, centerY - ySelection,
                            Integer.MAX_VALUE, centerY - ySelection, options.gridLinePaint);

                    int part = 5;
                    float part_padding = (ySelection - (i + 1) * yDIPx) / part;
                    for (int p = 1; p < part; p++) {
                        canvas.drawLine(Integer.MIN_VALUE, centerY - ySelection - p * part_padding,
                                Integer.MAX_VALUE, centerY - ySelection - p * part_padding, options.gridLine2Paint);
                    }
                }

                if (options.drawAxisYDivider)
                    canvas.drawLine(centerX - ls, centerY - ySelection,
                            centerX + ls, centerY - ySelection, options.dividerPaint);

                //if (options.drawYText)
                //   drawTextY(canvas,centerX,centerY,text_ls,ySelection,i);
            }
            yCount = (int) ((getGraphHeight() - centerY) / yDIPx) * mdc;
            for (int i = 1; i <= yCount; i++) {
                if (min_display_y > i * yDI || i * yDI > max_display_y) continue;

                float ySelection = i * yDIPx;
                if (options.drawGridYLines)
                    canvas.drawLine(Integer.MIN_VALUE, centerY + ySelection,
                            Integer.MAX_VALUE, centerY + ySelection, options.gridLinePaint);

                if (options.drawAxisYDivider)
                    canvas.drawLine(centerX - ls, centerY + ySelection,
                            centerX + ls, centerY + ySelection, options.dividerPaint);

                //if (options.drawYText)
                //    drawTextY(canvas,centerX,centerY,text_ls,-ySelection,-i);
            }
        }

        if (options.drawXText) {
            int xCount = (int) (centerX / xDIPx) * mdc;
            for (int i = 1; i <= xCount; i++) {
                if (min_display_x > -i * xDI || -i * xDI > max_display_x) continue;
                drawTextX(canvas, centerX, centerY, text_ls, -(i * xDIPx), -i);
            }
            xCount = (int) ((getGraphWidth() - centerX) / xDIPx) * mdc;
            for (int i = 1; i <= xCount; i++) {
                if (min_display_x > i * xDI || i * xDI > max_display_x) continue;
                drawTextX(canvas, centerX, centerY, text_ls, (i * xDIPx), i);
            }
        }
        if (options.drawYText) {
            int yCount = (int) (centerY / yDIPx) * mdc;
            for (int i = 1; i <= yCount; i++) {
                if (min_display_y > -i * yDI || -i * yDI > max_display_y) continue;
                drawTextY(canvas, centerX, centerY, text_ls, (i * yDIPx), i);
            }
            yCount = (int) ((getGraphHeight() - centerY) / yDIPx) * mdc;
            for (int i = 1; i <= yCount; i++) {
                if (min_display_y > i * yDI || i * yDI > max_display_y) continue;
                drawTextY(canvas, centerX, centerY, text_ls, -(i * yDIPx), -i);
            }
        }

        if (options.drawAxisY && min_display_x < 0)
            canvas.drawLine(centerX, Integer.MIN_VALUE, centerX, Integer.MAX_VALUE, options.axisPaint);
        if (options.drawAxisX && min_display_y < 0)
            canvas.drawLine(Integer.MIN_VALUE, centerY, Integer.MAX_VALUE, centerY, options.axisPaint);


        canvas.restore();
        graphScale = org_s;
        canvas.scale(graphScale, graphScale);
        if (axis == null) findAxisPosition();
        xDIPx = options.getScaledDividerIntervalInPx(options.xDividerIntervalInPx, graphScale);
        yDIPx = options.getScaledDividerIntervalInPx(options.yDividerIntervalInPx, graphScale);
        xDI = options.getScaledDividerInterval(options.xDividerInterval, graphScale) / org_scale;
        yDI = options.getScaledDividerInterval(options.yDividerInterval, graphScale);

        final float X_EPSILON = findFormulaX(EPSILON);
        for (AXGraphFormula formula : getFormulas()) {
            if (!formula.isEnabled()) continue;
            formula.getGraphPaint().setStrokeWidth(formula.getStrokeWidth() / graphScale);
            graphCanvas.setCanvas(canvas, formula);
            formula.onAttachedToView(this);

            if (!formula.onDraw(graphCanvas)) {
                graphCanvas.setApplyFormulaTransform(false);

                for (float rx = 0; rx <= getWidth() / graphScale; rx += formula.sensitive()) {
                    final float x = (dx + (rx * graphScale)) / graphScale;
                    final float xInAxis = x - centerX;
                    final float fx = AXGraphUtils.round(findFormulaX(xInAxis), 3);

                    if (fx >= 0 && !formula.isInDomain(fx)) {
                        /*/
                        final float x1 = fx + X_EPSILON;
                        final float x2 = fx - X_EPSILON;
                        final float fy = formula.function(fx);

                        if (fy == Float.POSITIVE_INFINITY || fy == Float.NEGATIVE_INFINITY)
                            continue;
                        if (formula.isInDomain(x1) && Math.abs(fy - formula.function(x1)) <= X_EPSILON) {
                            drawPoints.add(new CirclePoints(fy, fx, AXGraphPointType.EMPTY));
                        }
                        if (formula.isInDomain(x2) && Math.abs(fy - formula.function(x2)) <= X_EPSILON) {
                            drawPoints.add(new CirclePoints(fy, fx, AXGraphPointType.EMPTY));
                        }
                        continue;
                        //*/
                        break;
                    }

                    drawFormula(formula, fx, X_EPSILON);
                }
                for (float mainFX : mainPoints) {
                    if (formula.isInDomain(mainFX)) {
                        drawFormula(formula, mainFX, X_EPSILON);
                    }
                }
            }

            for (CirclePoints point : drawPoints) {
                formula.drawPoint(graphCanvas, point.fx, point.fy, point.type);
            }
            if (formula.getCustomPoints() != null) {
                for (PointF pointF : formula.getCustomPoints()) {
                    formula.drawPoint(graphCanvas, pointF.x, pointF.y, AXGraphPointType.CUSTOM);
                }
            }
            drawPoints.clear();
        }
    }

    protected void drawFormula(AXGraphFormula formula, float fx, float X_EPSILON) {
        //if (min_display_y > -fy || -fy > max_display_y) continue;
        if (formula instanceof AXGraphMultiFormula) {
            float[] fy1 = ((AXGraphMultiFormula) formula).applyMultiFunction(fx);
            float[] fy2 = ((AXGraphMultiFormula) formula).applyMultiFunction(fx + X_EPSILON);
            for (int fIndex = 0; fIndex < fy1.length; fIndex++) {
                if (fy2.length <= fIndex) break;

                if (Math.abs(findCanvasY(fy1[fIndex]) - findCanvasY(fy2[fIndex])) > getHeight())
                    continue;
                if (fy1[fIndex] == Float.POSITIVE_INFINITY || fy1[fIndex] == Float.NEGATIVE_INFINITY)
                    continue;
                if (fy2[fIndex] == Float.POSITIVE_INFINITY || fy2[fIndex] == Float.NEGATIVE_INFINITY)
                    continue;
                drawFunction(formula, fx, fy1[fIndex], fy2[fIndex], X_EPSILON);
            }
        } else {
            final float fy = formula.applyFunction(fx);

            if (fy == Float.POSITIVE_INFINITY || fy == Float.NEGATIVE_INFINITY) return;
            final float fy2 = formula.applyFunction(fx + X_EPSILON);
            if (fy2 == Float.POSITIVE_INFINITY || fy2 == Float.NEGATIVE_INFINITY) return;
            if (Math.abs(findCanvasY(fy) - findCanvasY(fy2)) > getHeight()) return;

            drawFunction(formula, fx, fy, fy2, X_EPSILON);
        }
    }

    protected void drawFunction(AXGraphFormula formula, float fx, float fy, float fy2, float X_EPSILON) {
        AXGraphPointType pointType = formula.getPointType(fx, fy);
        if (pointType == AXGraphPointType.CONTINUOUS) {
            graphCanvas.drawLine(fx, fy, fx + X_EPSILON, fy2);
        } else {
            drawPoints.add(new CirclePoints(fy, fx, pointType));
        }
    }

    protected void drawTextX(Canvas canvas, float centerX, float centerY, float text_ls, float xSelection, int i) {
        String text = options.getTextForXAxis(findFormulaX(xSelection), i);
        Rect textBounds = new Rect();
        options.textPaint.getTextBounds(text, 0, text.length(), textBounds);

        float left = centerX + xSelection - text_ls / 2 - textBounds.width() / 2f;
        float top = centerY + text_ls - textBounds.top - textBounds.height();
        top = Math.max(top, -dy / graphScale + text_ls);
        top = Math.min(top, ((getHeight() - dy) / graphScale) - textBounds.height() - text_ls);
        float right = left + textBounds.width() + text_ls;
        float bottom = top + textBounds.height() + text_ls;
        if (options.textBackgroundPaint != null) {
            canvas.drawRect(left, top, right, bottom, options.textBackgroundPaint);
        }
        canvas.drawText(text, left + text_ls / 2, bottom - text_ls / 2, options.textPaint);
    }


    protected void drawTextY(Canvas canvas, float centerX, float centerY, float text_ls, float ySelection, int i) {
        String text = options.getTextForYAxis(findFormulaY(ySelection), i);
        Rect textBounds = new Rect();
        options.textPaint.getTextBounds(text, 0, text.length(), textBounds);

        float left = centerX - textBounds.width() - text_ls;
        left = Math.max(left, -dx / graphScale);
        left = Math.min(left, ((getWidth() - dx) / graphScale) - textBounds.width() - text_ls);
        float top = centerY - ySelection - textBounds.height() / 2f - text_ls / 2;
        float right = left + textBounds.width() + text_ls - EPSILON;
        float bottom = top + textBounds.height() + text_ls;
        if (options.textBackgroundPaint != null) {
            canvas.drawRect(left, top, right, bottom, options.textBackgroundPaint);
        }
        canvas.drawText(text, left + text_ls / 4, bottom - text_ls / 2, options.textPaint);
    }

    protected void updatePaints() {
        options.axisPaint.setStrokeWidth(paintsStrokeWidth[0] / graphScale);
        options.dividerPaint.setStrokeWidth(paintsStrokeWidth[1] / graphScale);
        options.gridLinePaint.setStrokeWidth(paintsStrokeWidth[2] / graphScale);
        options.gridLine2Paint.setStrokeWidth(paintsStrokeWidth[4] / graphScale);
        options.textPaint.setTextSize(paintsStrokeWidth[3] / graphScale * getContext().getResources().getDisplayMetrics().density);
    }

    public float getCenterX() {
        return centerX;
    }

    public float getCenterY() {
        return centerY;
    }

    public float getGraphWidth() {
        return options.scrollEnabled && contentSize != null ? contentSize.width() : getWidth();
    }

    public float getGraphHeight() {
        return options.scrollEnabled && contentSize != null ? contentSize.height() : getHeight();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (options == null || !options.scrollEnabled) return super.onTouchEvent(event);

        matrix.set(savedMatrix);
        boolean gestureDetected = mGestureDetector.onTouchEvent(event);
        if (event.getPointerCount() > 1) {
            gestureDetected = mScaleGestureDetector.onTouchEvent(event) | gestureDetected;
            if (checkScaleBounds())
                matrix.postScale(scale, scale, mid.x, mid.y);
        }
        matrix.invert(matrixInverse);
        savedMatrix.set(matrix);
        invalidate();
        return gestureDetected;
    }

    private boolean checkScaleBounds() {
        float[] values = new float[9];
        matrix.getValues(values);
        float sx = values[Matrix.MSCALE_X] * scale;
        float sy = values[Matrix.MSCALE_Y] * scale;
        return sx >= options.minZoom && sx <= options.maxZoom && sy >= options.minZoom && sy <= options.maxZoom;
    }

    private void setupTranslation(float dX, float dY) {
        distanceX = -1 * dX;
        // CUSTOMIZED: DISABLE Y SCROLL
        distanceY = 0/*-1 * dY*/;

        if (contentSize != null) {
            float[] values = new float[9];
            matrix.getValues(values);
            float totX = values[Matrix.MTRANS_X] + distanceX;
            //float totY = values[Matrix.MTRANS_Y] + distanceY;
            float sx = values[Matrix.MSCALE_X];

            Rect viewableRect = new Rect();
            this.getDrawingRect(viewableRect);
            float offscreenWidth = contentSize.width() - (viewableRect.right - viewableRect.left);
            float offscreenHeight = contentSize.height() - (viewableRect.bottom - viewableRect.top);
            float maxDx = (contentSize.width() - (contentSize.width() / sx)) * sx;
            //float maxDy = (contentSize.height() - (contentSize.height() / sx)) * sx;
            if (totX > 0 && distanceX > 0) distanceX = 0;
            //if (totY > 0 && distanceY > 0) distanceY = 0;
            if (totX * -1 > offscreenWidth + maxDx && distanceX < 0) distanceX = 0;
            //if (totY * -1 > offscreenHeight + maxDy && distanceY < 0) distanceY = 0;
        }

        if (distanceX != 0 && animation != null)
            animation.cancel();
    }

    float graphWidth, graphHeight;

    protected void setMaxScrollSize(float graphWidth, float graphHeight, float maxWidth, float maxHeight) {
        this.contentSize = new RectF(0, 0, maxWidth, maxHeight);
        this.graphWidth = graphWidth;
        this.graphHeight = graphHeight;
        resetScroll(false);
    }

    @Override
    public void setBackgroundColor(int color) {
        super.setBackgroundColor(color);
        setGraphInnerPointColor(color);
    }

    public void setGraphInnerPointColor(int color) {
        bg = color;
        if (options != null)
            options.textBackgroundPaint.setColor(color);
        for (AXGraphFormula formula : formulas)
            formula.graphBackgroundColor = color;
    }

    public float getGraphScale() {
        return graphScale;
    }

    public Matrix getGraphMatrix() {
        return matrix;
    }

    public void setAnimationDuration(long animationDuration) {
        this.animationDuration = animationDuration;
    }

    public long getAnimationDuration() {
        return animationDuration;
    }

    public void animateX(float dx, long duration) {
        Matrix m = new Matrix(matrix);
        m.postTranslate(dx * org_scale, 0);
        postMatrixAnimation(m, duration);
    }

    public boolean isPointVisible(float fx) {
        xDI = options.xDividerInterval / org_scale;
        matrix.getValues(matrixValues);
        float x = findGraphX(fx) + matrixValues[Matrix.MTRANS_X];
        xDI = options.xDividerInterval;

        return x >= 0 && x < getMeasuredWidth();
    }

    public void setGraphScale(float scale, float x, float y) {
        matrix.getValues(matrixValues);

        Matrix matrix = new Matrix();
        matrix.setTranslate(matrixValues[Matrix.MTRANS_X], matrixValues[Matrix.MTRANS_Y]);
        matrix.postScale(scale, scale, x, y);
        postMatrixAnimation(matrix, animationDuration);
    }

    public void postMatrixAnimation(Matrix matrix, long duration) {
        if (animation != null) animation.cancel();

        if (duration <= 0) {
            this.matrix.set(matrix);
            this.savedMatrix.set(matrix);
            invalidate();
        } else {
            animation = new MatrixAnimation(this.matrix, matrix, duration);
        }
    }

    public void addFormula(AXGraphFormula formula) {
        formulas.add(formula);
        formula.graphBackgroundColor = bg;
        formula.onAttachedToView(this);


    }

    public void addFormulas(AXGraphFormula... graphFormulas) {
        formulas.addAll(Arrays.asList(graphFormulas));
        for (AXGraphFormula formula : graphFormulas) {
            formula.graphBackgroundColor = bg;
            formula.onAttachedToView(this);
        }
    }

    public void addFormulas(List<AXGraphFormula> graphFormulas) {
        formulas.addAll(graphFormulas);
        for (AXGraphFormula formula : graphFormulas) {
            formula.graphBackgroundColor = bg;
            formula.onAttachedToView(this);
        }
    }

    public void removeFormula(AXGraphFormula formula) {
        formulas.remove(formula);
        formula.onDetachedFromView(this);
    }

    public void resetScroll(boolean animation) {
        if (contentSize == null)
            return;

        if (animation) {
            postMatrixAnimation(createDefaultMatrix(), animationDuration);
        } else {
            matrix.reset();
            savedMatrix.reset();
            matrix.set(createDefaultMatrix());
            savedMatrix.set(matrix);
            mdc = (int) ((contentSize.width() / graphWidth) * 2);
            invalidate();
        }
    }

    public Matrix createDefaultMatrix() {
        //float axisX = (contentSize.width()-graphWidth)/2 ,axisY = (contentSize.height()-graphHeight) /2;
        float axisX = graphSize.left + graphWidth / 2f, axisY = graphSize.top + graphHeight / 2;
        if (options != null && options.axis != null && options.axis.x != DEFAULT)
            axisX += options.axis.x;
        if (options != null && options.axis != null && options.axis.y != DEFAULT)
            axisY += options.axis.y;

        Matrix matrix = new Matrix();
        //matrix.postTranslate(-axisX, -axisY);
        matrix.postTranslate(axisX, axisY);
        matrix.postScale(1f, 1f);
        return matrix;
    }

    public void clearFormulas() {
        for (AXGraphFormula formula : formulas)
            formula.onDetachedFromView(this);
        formulas.clear();
    }

    public List<AXGraphFormula> getFormulas() {
        return formulas;
    }

    public float getOriginalScale() {
        return org_scale;
    }

    @Override
    protected void onDetachedFromWindow() {
        resetScroll(false);
        super.onDetachedFromWindow();
    }

    private static class CirclePoints {
        AXGraphPointType type;
        float fy, fx;

        public CirclePoints(float fy, float fx, AXGraphPointType type) {
            this.type = type;
            this.fy = fy;
            this.fx = fx;
        }
    }

    private class MatrixAnimation {
        private final PointF scaleStart;
        private final PointF scaleEnd;
        private final PointF translateStart;
        private final PointF translateEnd;
        private final ValueAnimator animator;

        public MatrixAnimation(Matrix startMatrix, Matrix endMatrix, long duration) {
            float[] a = new float[9];
            float[] b = new float[9];

            startMatrix.getValues(a);
            endMatrix.getValues(b);

            scaleStart = new PointF(a[Matrix.MSCALE_X], a[Matrix.MSCALE_Y]);
            scaleEnd = new PointF(b[Matrix.MSCALE_X], b[Matrix.MSCALE_Y]);
            translateStart = new PointF(a[Matrix.MTRANS_X], a[Matrix.MTRANS_Y]);
            translateEnd = new PointF(b[Matrix.MTRANS_X], b[Matrix.MTRANS_Y]);

            animator = ValueAnimator.ofFloat(0, 1f);
            animator.setInterpolator(new LinearInterpolator());
            animator.setDuration(duration);
            animator.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator valueAnimator) {
                    float interpolatedTime = valueAnimator.getAnimatedFraction();

                    PointF sFactor = new PointF(
                            scaleEnd.x * interpolatedTime / scaleStart.x + 1 - interpolatedTime,
                            scaleEnd.y * interpolatedTime / scaleStart.y + 1 - interpolatedTime);
                    PointF tFactor = new PointF(
                            (translateEnd.x - translateStart.x) * interpolatedTime,
                            (translateEnd.y - translateStart.y) * interpolatedTime);

                    Matrix matrix = new Matrix();
                    matrix.postScale(scaleStart.x, scaleStart.y, 0, 0);
                    matrix.postScale(sFactor.x, sFactor.y, 0, 0);
                    matrix.postTranslate(translateStart.x, translateStart.y);
                    matrix.postTranslate(tFactor.x, tFactor.y);
                    AXGraphView.this.matrix.set(matrix);
                    AXGraphView.this.savedMatrix.set(matrix);
                    AXGraphView.this.invalidate();
                }
            });
            animator.start();
        }

        public void cancel() {
            if (animator != null && animator.isRunning()) animator.end();
        }

    }
}