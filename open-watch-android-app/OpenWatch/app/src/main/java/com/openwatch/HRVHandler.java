package com.openwatch;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.view.View;
import android.view.ViewAnimationUtils;
import android.view.ViewGroup;

import com.aghajari.axanimation.AXAnimation;
import com.aghajari.chart.HRVChart;
import com.openwatch.packet.PPGData;
import com.openwatch.util.HRVMorseCode;
import com.openwatch.util.Utils;

import java.text.MessageFormat;
import java.util.Random;

public class HRVHandler {

    private final activity_main view;
    private boolean isChartFull = false;
    private View morseView;

    public HRVHandler(Activity activity, activity_main view) {
        this.view = view;

        view.hrv_panel.setVisibility(View.INVISIBLE);
        view.hrv_panel.setOnClickListener(v -> {
            // DO NOTHING
        });

        view.hrv.setTag(false);
        View.OnClickListener clickListener = v -> {
            if (view.hrv.getTag() == null)
                return;

            boolean currentStare = (boolean) view.hrv.getTag();

            if (currentStare) {
                if (morseView != null) {
                    view.hrv_panel.removeView(morseView);
                    morseView = null;
                }
            } else {
                morseView = HRVMorseCode.create(view.hrv_panel);
            }

            view.hrv.setTag(null);

            int[] position = new int[2];
            view.hrv.getLocationInWindow(position);
            position[0] += view.hrv.getMeasuredWidth() / 2;
            position[1] += view.hrv.getMeasuredHeight() / 2;

            float r = (float) Math.hypot(view.bg.getMeasuredWidth(), view.bg.getMeasuredHeight());

            Animator anim = ViewAnimationUtils.createCircularReveal(
                    view.hrv_panel,
                    position[0],
                    position[1],
                    currentStare ? r : 0f,
                    currentStare ? 0f : r
            );

            view.hrv_panel.setVisibility(View.VISIBLE);
            if (!currentStare)
                view.hrv_chart.startAnimation();

            anim.setDuration(500);
            anim.addListener(new AnimatorListenerAdapter() {
                @Override
                public void onAnimationEnd(Animator animation) {
                    super.onAnimationEnd(animation);
                    view.hrv.setTag(!currentStare);

                    if (currentStare)
                        view.hrv_panel.setVisibility(View.INVISIBLE);
                }
            });
            anim.start();

            findAvg();
            AXAnimation.create()
                    .nextSectionWithDelay(currentStare ? 20 : 300)
                    .withSectionStartAction(a -> Utils.setStatusBarLight(activity, !currentStare))
                    .duration(currentStare ? 480 : 200)
                    .alpha(1f)
                    .start(view.title);
        };

        view.hrv.setOnClickListener(clickListener);
        view.hrv_back.setOnClickListener(clickListener);

        ((ViewGroup.MarginLayoutParams) view.hrv_pnl.getLayoutParams())
                .topMargin += Utils.getStatusBarHeight(activity);

        if (MainActivity.ONLY_PHONE_DEBUG) {
            Random rnd = new Random();
            for (int i = 0; i < HRVChart.DATA_COUNT; i++) {
                view.hrv_chart.addData(rnd.nextInt(130 - 70) + 70, 98);
            }
        }
    }

    public void updateColor(int[] colors) {
        view.hrv_chart.setLineColor(colors[0]);
        view.hrv_chart.setPointColor(colors[1]);
        view.hrv_avg.setTextColor(colors[0]);
    }

    public void addData(PPGData data) {
        boolean firstFullChart = !isChartFull && (view.hrv_chart.getList().size() == HRVChart.DATA_COUNT - 1);

        while (view.hrv_chart.getList().size() >= HRVChart.DATA_COUNT)
            view.hrv_chart.getList().remove(0);

        view.hrv_chart.addData((float) data.getHR(), (float) data.getSpO2());

        if (firstFullChart) {
            isChartFull = true;

            if (view.hrv.getTag() == null)
                return;

            if (view.hrv.getTag() != null && !((boolean) view.hrv.getTag()))
                view.hrv.performClick();
        }
    }

    private void findAvg() {
        int size = view.hrv_chart.getList().size();
        if (size == 0) {
            view.hrv_avg.setText("");

        } else {
            float sum = 0, sum2 = 0;

            for (HRVChart.ChartData c : view.hrv_chart.getList()) {
                sum += c.getHr();
                sum2 += c.getSpo2();
            }

            view.hrv_avg.setText(MessageFormat.format("{0} bpm  {1}%",
                    (int) (sum / size), PPGData.getReadableSpO2(sum2 / size)));
        }
    }

}
