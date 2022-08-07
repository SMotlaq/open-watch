package com.openwatch.util;

import android.content.Context;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.LinearSnapHelper;
import androidx.recyclerview.widget.RecyclerView;

/**
 * https://github.com/DingMouRen/LayoutManagerGroup/blob/master/LayoutManagerGroup/src/main/java/com/dingmouren/layoutmanagergroup/picker/PickerLayoutManager.java
 */
public class PickerLayoutManager extends LinearLayoutManager {
    private final float mScale = 0.5f;
    private final boolean mIsAlpha = true;
    private final int mItemCount = 3;
    private int selectedPosition = -1;

    private final LinearSnapHelper mLinearSnapHelper;
    private RecyclerView mRecyclerView;
    private OnSelectedViewListener mOnSelectedViewListener;

    public PickerLayoutManager(Context context) {
        super(context, LinearLayoutManager.VERTICAL, false);
        this.mLinearSnapHelper = new LinearSnapHelper();
    }

    @Override
    public void onAttachedToWindow(RecyclerView view) {
        super.onAttachedToWindow(view);
        this.mRecyclerView = view;
        mLinearSnapHelper.attachToRecyclerView(view);
    }

    @Override
    public void onMeasure(@NonNull RecyclerView.Recycler recycler, @NonNull RecyclerView.State state, int widthSpec, int heightSpec) {
        if (mRecyclerView == null)
            return;

        int mItemViewHeight = (int) (mRecyclerView.getContext().getResources().getDisplayMetrics().density * 40);

        int paddingVertical = (mItemCount - 1) / 2 * mItemViewHeight;
        mRecyclerView.setClipToPadding(false);
        mRecyclerView.setPadding(0, paddingVertical, 0, paddingVertical);
        super.onMeasure(recycler, state, widthSpec, heightSpec);
    }

    @Override
    public void onLayoutChildren(RecyclerView.Recycler recycler, RecyclerView.State state) {
        super.onLayoutChildren(recycler, state);
        if (getItemCount() < 0 || state.isPreLayout()) return;

        scaleVerticalChildView();
    }

    @Override
    public int scrollVerticallyBy(int dy, RecyclerView.Recycler recycler, RecyclerView.State state) {
        scaleVerticalChildView();
        return super.scrollVerticallyBy(dy, recycler, state);
    }

    private void scaleVerticalChildView() {
        float mid = getHeight() / 2.0f;
        for (int i = 0; i < getChildCount(); i++) {
            View child = getChildAt(i);
            if (child == null)
                return;
            float childMid = (getDecoratedTop(child) + getDecoratedBottom(child)) / 2.0f;
            float scale = 1.0f + (-1 * (1 - mScale)) * (Math.min(mid, Math.abs(mid - childMid))) / mid;
            child.setScaleX(scale);
            child.setScaleY(scale);
            if (mIsAlpha) {
                child.setAlpha(scale);
            }
        }
    }

    @Override
    public void onScrollStateChanged(int state) {
        super.onScrollStateChanged(state);
        if (state == 0) {
            if (mLinearSnapHelper != null) {
                View view = mLinearSnapHelper.findSnapView(this);
                if (view == null)
                    return;
                selectedPosition = getPosition(view);
                if (mOnSelectedViewListener != null)
                    mOnSelectedViewListener.onSelectedView(view, selectedPosition);
            }
        }
    }

    public void setOnSelectedViewListener(OnSelectedViewListener listener) {
        this.mOnSelectedViewListener = listener;
    }

    public interface OnSelectedViewListener {
        void onSelectedView(View view, int position);
    }

    public int getSelectedPosition() {
        return selectedPosition;
    }

    public void setSelectedPosition(int selectedPosition) {
        this.selectedPosition = selectedPosition;
        mRecyclerView.scrollToPosition(selectedPosition);
    }
}