package com.aghajari.pathloading;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ValueAnimator;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.CornerPathEffect;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PathMeasure;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.View;

import androidx.annotation.Nullable;

/**
 * @author Amir Hossein Aghajari
 * @version 1.0.0
 */
public class AXPathLoadingView extends View {

    private final Paint paint = new Paint();
    private PathMeasure pathMeasure;
    private Path orgPath, path;
    private ValueAnimator animator;

    private float progressSize, currentPosition;
    private long duration = 1200, delay = 100;
    private int trackColor = Color.WHITE, progressColor = Color.LTGRAY;

    private boolean alphaAnimationEnabled = true;
    private float startAlphaAnimationFromFraction = 0.65f;

    public AXPathLoadingView(Context context) {
        this(context, null);
    }

    public AXPathLoadingView(Context context, @Nullable AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public AXPathLoadingView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        paint.setStrokeWidth(context.getResources().getDisplayMetrics().density * 6);
        paint.setStyle(Paint.Style.STROKE);
        paint.setStrokeJoin(Paint.Join.ROUND);
        paint.setStrokeCap(Paint.Cap.ROUND);
        paint.setPathEffect(new CornerPathEffect(paint.getStrokeWidth() / 2f));
        setProgressSize(0.28f);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        if (path != null) {
            paint.setColor(trackColor);
            canvas.drawPath(path, paint);

            if (isAlphaAnimationEnabled() && animator != null) {
                float fraction = animator.getAnimatedFraction();
                if (fraction >= startAlphaAnimationFromFraction) {
                    float max = calculateMaxAlphaFactor();
                    fraction -= startAlphaAnimationFromFraction;
                    int alpha = (int) ((max - fraction) * 255 / max);
                    paint.setColor((progressColor & 0x00ffffff) | (alpha << 24));
                } else {
                    paint.setColor(progressColor);
                }
            } else {
                paint.setColor(progressColor);
            }

            canvas.drawPath(getSubPath(
                    Math.max(0f, currentPosition),
                    Math.min(1f, currentPosition + progressSize)
            ), paint);
        }
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
        super.onLayout(changed, left, top, right, bottom);
        createPathToCenter();
    }

    public Path getPath() {
        return orgPath;
    }

    private Path getSubPath(float start, float end) {
        Path sub = new Path();
        pathMeasure.getSegment(start * pathMeasure.getLength(), end * pathMeasure.getLength(), sub, true);
        return sub;
    }

    public void setPath(Path path) {
        this.orgPath = path;
        createPathToCenter();
        animator.start();
    }

    private void createPathToCenter() {
        if (orgPath == null)
            return;

        path = new Path(orgPath);
        RectF bound = new RectF();
        path.computeBounds(bound, true);
        path.offset((getMeasuredWidth() - bound.right) / 2,
                (getMeasuredHeight() - bound.bottom) / 2f);
        pathMeasure = new PathMeasure(this.path, false);
    }

    public float getProgressSize() {
        return progressSize;
    }

    public void setProgressSize(float progressSize) {
        this.progressSize = progressSize;
        if (animator != null) {
            cancelAnimation();
        } else {
            currentPosition = -progressSize;
        }

        animator = ValueAnimator.ofFloat(-progressSize, 1f);
        animator.setDuration(duration);
        animator.setStartDelay(delay);
        animator.addUpdateListener(a -> {
            currentPosition = (float) a.getAnimatedValue();
            invalidate();
        });
        animator.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                super.onAnimationEnd(animation);
                // We need a delay before next start
                // So, We can't use inf repeat count!
                if (animator != null)
                    animator.start();
            }
        });
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();
        if (path != null && animator != null)
            animator.start();
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (animator != null) {
            cancelAnimation();
        }
    }

    public void cancelAnimation() {
        currentPosition = -progressSize;
        if (animator != null)
            animator.cancel();
        animator = null;
        invalidate();
    }

    public void startAnimation() {
        animator.start();
    }

    public long getDuration() {
        return duration;
    }

    public void setDuration(long duration) {
        this.duration = duration;
        animator.setDuration(duration);
    }

    public long getDelay() {
        return delay;
    }

    public void setDelay(long delay) {
        this.delay = delay;
        animator.setStartDelay(delay);
    }

    public int getTrackColor() {
        return trackColor;
    }

    public void setTrackColor(int trackColor) {
        this.trackColor = trackColor;
        invalidate();
    }

    public int getProgressColor() {
        return progressColor;
    }

    public void setProgressColor(int progressColor) {
        this.progressColor = progressColor;
        invalidate();
    }

    public Paint getPaint() {
        return paint;
    }

    public boolean isAlphaAnimationEnabled() {
        return alphaAnimationEnabled;
    }

    public void setAlphaAnimationEnabled(boolean alphaAnimationEnabled) {
        this.alphaAnimationEnabled = alphaAnimationEnabled;
    }

    public float getStartAlphaAnimationFromFraction() {
        return startAlphaAnimationFromFraction;
    }

    public void setStartAlphaAnimationFromFraction(float startAlphaAnimationFromFraction) {
        this.startAlphaAnimationFromFraction = startAlphaAnimationFromFraction;
    }

    protected float calculateMaxAlphaFactor() {
        return (1f - startAlphaAnimationFromFraction) * (1f + progressSize * 3);
    }
}
