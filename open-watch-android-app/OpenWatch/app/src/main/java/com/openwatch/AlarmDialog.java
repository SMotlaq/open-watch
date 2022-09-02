package com.openwatch;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.view.ViewGroup;
import android.view.Window;

import androidx.annotation.NonNull;

import com.openwatch.util.ArrayAdapter;
import com.openwatch.util.PickerLayoutManager;
import com.openwatch.util.Utils;

public class AlarmDialog extends Dialog {

    private final Runnable saver, updater;
    private final alarm_dialog view;
    private final PickerLayoutManager h, m, d;
    private final ArrayAdapter<String> ha, ma, da;

    public AlarmDialog(@NonNull Context context, int[] colors, Runnable updater) {
        super(context, R.style.AghajariDialog);
        this.updater = updater;

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(view = new alarm_dialog(context));

        Window window = getWindow();
        window.setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
        window.setLayout((int) (context.getResources().getDisplayMetrics().widthPixels * 0.86)
                , ViewGroup.LayoutParams.WRAP_CONTENT);

        view.accept_parent.setClipToOutline(true);
        view.cancel_parent.setClipToOutline(true);

        String[] hour = new String[24];
        String[] min = new String[60];

        for (int i = 0; i < 24; i++) {
            hour[i] = i >= 10 ? String.valueOf(i) : "0" + i;
        }
        for (int i = 0; i < 60; i++) {
            min[i] = i >= 10 ? String.valueOf(i) : "0" + i;
        }

        view.hourRV.setAdapter(ha = new ArrayAdapter<>(hour, true));
        view.minRV.setAdapter(ma = new ArrayAdapter<>(min, true));
        view.dayRV.setAdapter(da = new ArrayAdapter<>(Utils.getDays(), false));

        view.hourRV.setLayoutManager(h = new PickerLayoutManager(context));
        view.minRV.setLayoutManager(m = new PickerLayoutManager(context));
        view.dayRV.setLayoutManager(d = new PickerLayoutManager(context));

        h.setRecyclerView(view.hourRV);
        m.setRecyclerView(view.minRV);
        d.setRecyclerView(view.dayRV);

        int[] res = Utils.getAlarmPositions(context);

        h.setSelectedPosition((Integer.MAX_VALUE >> 1) - 15 + res[0]);
        m.setSelectedPosition((Integer.MAX_VALUE >> 1) - 3 + res[1]);
        d.setSelectedPosition(res[2]);

        saver = () -> {
            Utils.saveAlarm(context, h.getSelectedPosition() % 24,
                    m.getSelectedPosition() % 60,
                    d.getSelectedPosition());
            updater.run();
        };

        updateColor(colors);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        view.cancel.setOnClickListener(v -> {
            Utils.deleteAlarm(getContext());
            updater.run();
            cancel();
        });

        view.accept.setOnClickListener(v -> {
            saver.run();
            dismiss();
        });
    }

    public void updateColor(int[] colors) {
        GradientDrawable gd = new GradientDrawable();
        gd.setCornerRadius(1000);
        gd.setColors(colors);
        gd.setOrientation(GradientDrawable.Orientation.TL_BR);
        view.accept_parent.setBackground(gd);

        view.title.setTextColor(colors[0]);
        view.cancel.setTextColor(colors[0]);

        ha.setTextColor(colors[0]);
        ma.setTextColor(Utils.getColorByIndex(colors[0], colors[1], 0.5f));
        da.setTextColor(colors[1]);
    }

}
