package com.openwatch.util;

import android.graphics.Color;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class IntAdapter extends RecyclerView.Adapter<IntAdapter.StringVH> {

    private final int[] values;

    public IntAdapter(int[] values) {
        this.values = values;
    }

    @NonNull
    @Override
    public StringVH onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new StringVH(new TextView(parent.getContext()));
    }

    @Override
    public void onBindViewHolder(@NonNull StringVH holder, int position) {
        holder.bind(values[position]);
    }

    @Override
    public int getItemCount() {
        return values.length;
    }

    protected static class StringVH extends RecyclerView.ViewHolder {

        public StringVH(@NonNull TextView itemView) {
            super(itemView);
            itemView.setTextSize(20);
            itemView.setTextColor(Color.WHITE);
            itemView.setGravity(Gravity.CENTER);
            itemView.setLayoutParams(new FrameLayout.LayoutParams(-1,
                    (int) (itemView.getContext().getResources().getDisplayMetrics().density * 40)));
        }

        public void bind(int value) {
            ((TextView) itemView).setText(String.valueOf(value));
        }
    }
}
