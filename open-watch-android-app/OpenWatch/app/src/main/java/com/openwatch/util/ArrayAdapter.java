package com.openwatch.util;

import android.graphics.Color;
import android.view.Gravity;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

public class ArrayAdapter<T> extends RecyclerView.Adapter<ArrayAdapter.StringVH> {

    private final T[] values;
    private int textColor = Color.WHITE;
    private final boolean loop;

    public ArrayAdapter(T[] values) {
        this(values, false);
    }

    public ArrayAdapter(T[] values, boolean loop) {
        this.values = values;
        this.loop = loop;
    }

    public void setTextColor(int textColor) {
        this.textColor = textColor;
        notifyDataSetChanged();
    }

    @NonNull
    @Override
    public StringVH onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return new StringVH(new TextView(parent.getContext()));
    }

    @Override
    public void onBindViewHolder(@NonNull StringVH holder, int position) {
        holder.bind(values[loop ? position % values.length : position], textColor);
    }

    @Override
    public int getItemCount() {
        return loop ? Integer.MAX_VALUE : values.length;
    }

    protected static class StringVH extends RecyclerView.ViewHolder {

        public StringVH(@NonNull TextView itemView) {
            super(itemView);
            itemView.setTextSize(20);
            itemView.setGravity(Gravity.CENTER);
            itemView.setLayoutParams(new FrameLayout.LayoutParams(-1,
                    (int) (itemView.getContext().getResources().getDisplayMetrics().density * 40)));
        }

        public void bind(Object value, int textColor) {
            if (value == null)
                return;

            TextView tv = (TextView) itemView;
            tv.setTextColor(textColor);
            tv.setText(value.toString());
        }
    }
}
