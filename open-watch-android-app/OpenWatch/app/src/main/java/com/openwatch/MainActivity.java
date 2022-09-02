package com.openwatch;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.animation.ArgbEvaluator;
import android.animation.FloatEvaluator;
import android.animation.IntEvaluator;
import android.animation.ValueAnimator;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.RelativeSizeSpan;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.Toast;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.ColorUtils;

import com.aghajari.axanimation.AXAnimation;
import com.aghajari.xmlbypass.XmlByPass;
import com.aghajari.xmlbypass.XmlByPassAttr;
import com.aghajari.xmlbypass.XmlLayout;
import com.openwatch.packet.MessageReceiver;
import com.openwatch.packet.PPGAlgorithms;
import com.openwatch.packet.PPGData;
import com.openwatch.packet.WatchData;
import com.openwatch.util.ArrayAdapter;
import com.openwatch.util.KeyboardHeightProvider;
import com.openwatch.util.PickerLayoutManager;
import com.openwatch.util.Utils;

import java.text.MessageFormat;
import java.util.Locale;

@XmlByPass(layouts = {@XmlLayout(layout = "*")})
@XmlByPassAttr(name = "textColorHint", format = "color", codeName = "hintTextColor")
public class MainActivity extends AppCompatActivity implements MessageReceiver {

    public static final boolean ONLY_PHONE_DEBUG = false;

    private static final int[][] COLORS = {
            new int[]{0xFFFF9190, 0xFF5E72EB},
            new int[]{0xFF0AB28C, 0xFFAB13A6},
            new int[]{0xFFC5227A, 0xFF7F19CD},
    };

    private int colorIndex = 0, currentIndex = 3;
    private GraphFormula graphFormula;
    private HRVHandler hrvHandler;
    private AlarmDialog alarmDialog;

    private activity_main view;
    private float r = 0;
    private boolean isWhiteLoading = true;
    private boolean isOnSplash = true;

    private final int[] homePagePositions = new int[2];
    private BluetoothThread bluetoothThread = null;
    private KeyboardHeightProvider heightProvider;

    ActivityResultLauncher<Intent> launcher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == RESULT_OK) {
                    startScan();
                } else {
                    view.connectionStatus.setText("Bluetooth Disconnected!");
                    view.path_progress.cancelAnimation();
                }
            }
    );

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS,
                WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS);

        setContentView(view = new activity_main(this));
        view.setVisibility(View.INVISIBLE);
        setBackground(COLORS[colorIndex]);
        view.circle.setClipToOutline(true);
        view.path_progress.setPath(Utils.createPath(this));
        /*view.circle.setOnClickListener(v -> {
            view.circle.setOnClickListener(null);
            connectAnimation();
        });*/

        initSplashMode();
        bindAlarm();
        hrvHandler = new HRVHandler(this, view);
    }

    // BLUETOOTH

    public void checkForBluetooth() {
        BluetoothManager bluetoothManager = (BluetoothManager)
                getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
            return;
        }

        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            launcher.launch(enableBtIntent);
        } else {
            startScan();
        }
    }

    public void startScan() {
        if (bluetoothThread != null)
            bluetoothThread.interrupt();

        bluetoothThread = new BluetoothThread(this, this);
        bluetoothThread.start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (bluetoothThread != null) {
            bluetoothThread.interrupt();
            //PPGAlgorithms.terminate();
        }
    }

    // END OF BLUETOOTH

    // INITIAL STATE

    private void initSplashMode() {
        if (view.title.getMeasuredHeight() <= 10) {
            view.title.post(this::initSplashMode);
            return;
        }
        isOnSplash = true;

        int screenWidth = view.getMeasuredWidth();
        int screenHeight = view.getMeasuredHeight();

        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)
                view.bg.getLayoutParams();

        homePagePositions[0] = view.circle.getMeasuredHeight();
        lp.width = screenWidth;
        lp.height = screenHeight;
        lp.gravity = Gravity.TOP;

        FrameLayout.LayoutParams lp2 = (FrameLayout.LayoutParams) view.circle.getLayoutParams();
        homePagePositions[1] = lp2.topMargin;
        lp2.width = screenWidth;
        lp2.height = screenHeight;
        lp2.topMargin = 0;
        lp2.gravity = Gravity.TOP;

        FrameLayout.LayoutParams lpTitle = (FrameLayout.LayoutParams) view.title.getLayoutParams();
        lpTitle.gravity = Gravity.CENTER_HORIZONTAL;
        lpTitle.topMargin = homePagePositions[1]
                + homePagePositions[0] / 2 - view.title.getMeasuredHeight() / 2;

        view.title.setScaleX(2f);
        view.title.setScaleY(2f);

        view.requestLayout();
        view.setVisibility(View.VISIBLE);
    }

    private void initGraph() {
        graphFormula = new GraphFormula(view.graph_view);
        setBackground(lastColors);

        //*/
        if (ONLY_PHONE_DEBUG)
            view.postDelayed(() -> receive(MessageReceiver.TYPE_DATA, PPGAlgorithms.test(this)),
                    1400);
        //*/
    }

    // END OF INITIAL STATE

    // BACKGROUND ANIMATION

    private int[] lastColors;

    private void setBackground(int[] colors) {
        lastColors = colors;

        GradientDrawable gd = new GradientDrawable();
        gd.setColors(colors);
        gd.setOrientation(GradientDrawable.Orientation.TL_BR);

        //float r = getResources().getDisplayMetrics().density * 24;
        gd.setCornerRadii(new float[]{0, 0, 0, 0, r, r, r, r});
        view.bg.setBackground(gd);

        if (graphFormula != null) {
            graphFormula.updateColor(view.graph_view, colors);
            view.legendHR.setBackgroundColor(colors[1]);
            view.legendHRText.setTextColor(colors[1]);
            view.legendSpO2.setBackgroundColor(colors[0]);
            view.legendSpO2Text.setTextColor(colors[0]);
        }

        if (view.path_progress != null)
            updateLoadingColor();

        if (hrvHandler != null)
            hrvHandler.updateColor(colors);

        if (alarmDialog != null)
            alarmDialog.updateColor(colors);
    }

    private void updateLoadingColor() {
        if (isWhiteLoading) {
            view.path_progress.setTrackColor(Color.argb(30, 255, 255, 255));
            view.path_progress.setProgressColor(Color.WHITE);
            view.connectionStatus.setTextColor(Color.WHITE);
        } else {
            view.path_progress.setTrackColor(ColorUtils.setAlphaComponent(lastColors[0], 30));
            view.path_progress.setProgressColor(lastColors[0]);
            view.connectionStatus.setTextColor(lastColors[0]);
        }
    }

    private boolean isAnimating = false;

    private final Runnable bgAnimation = () -> {
        int next = (colorIndex + 1 >= COLORS.length) ? 0 : colorIndex + 1;

        ValueAnimator animator = ValueAnimator.ofFloat(0f, 1f);
        animator.setDuration(4000);
        ArgbEvaluator evaluator = new ArgbEvaluator();
        int[] c = new int[2];

        isAnimating = true;

        animator.addUpdateListener(valueAnimator -> {
            for (int i = 0; i < 2; i++)
                c[i] = (int) evaluator.evaluate(
                        valueAnimator.getAnimatedFraction(),
                        COLORS[colorIndex][i], COLORS[next][i]
                );

            setBackground(c);
        });
        animator.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                super.onAnimationEnd(animation);
                colorIndex = next;
                isAnimating = false;
                animate();
            }
        });
        animator.start();
    };

    private final Runnable splashTimer = this::splashDone;

    @Override
    protected void onResume() {
        super.onResume();
        animate();

        if (isOnSplash)
            view.postDelayed(splashTimer, 3000);
    }

    @Override
    protected void onPause() {
        super.onPause();
        view.removeCallbacks(bgAnimation);
        view.removeCallbacks(splashTimer);
    }

    private void animate() {
        view.removeCallbacks(bgAnimation);
        if (isAnimating)
            return;
        view.postDelayed(bgAnimation, 1000);
    }

    // END OF BACKGROUND ANIMATION

    // PAGE ANIMATION

    private void splashDone() {
        isOnSplash = false;
        if (isDestroyed())
            return;

        if (Utils.needsSignInForm(this)) {
            AXAnimation.create()
                    .duration(500)
                    .alpha(0f)
                    .start(view.path_progress);

            view.signInForm.setVisibility(View.VISIBLE);
            heightProvider = new KeyboardHeightProvider(view.nameEdt, this, height -> {
                if (height > 10) {
                    AXAnimation.create()
                            .duration(200)
                            .alpha(0f)
                            .translationY(-200f)
                            .start(view.title);

                    AXAnimation.create()
                            .translationY(-height - 100f)
                            .duration(200)
                            .start(view.signInForm);
                } else {
                    AXAnimation.create()
                            .duration(200)
                            .alpha(1f)
                            .translationY(0f)
                            .start(view.title);

                    AXAnimation.create()
                            .translationY(0f)
                            .duration(200)
                            .start(view.signInForm);
                }
            });
            heightProvider.start();

            Integer[] age = new Integer[99];
            for (int i = 0; i < 99; i++) {
                age[i] = i + 2;
            }
            Integer[] stepSize = new Integer[20];
            for (int i = 0, j = 25; i < 20; i++, j += 5) {
                stepSize[i] = j;
            }

            view.ageRV.setAdapter(new ArrayAdapter<>(age));
            view.stepSizeRV.setAdapter(new ArrayAdapter<>(stepSize));

            PickerLayoutManager ageLM, stepSizeLM;
            view.ageRV.setLayoutManager(ageLM = new PickerLayoutManager(this));
            view.stepSizeRV.setLayoutManager(stepSizeLM = new PickerLayoutManager(this));

            Utils.loadUISignInForm(this, view);

            float initialTranslationY = view.signInForm.getTranslationY();

            AXAnimation.create()
                    .nextSectionWithDelay(100)
                    .duration(800)
                    .alpha(1f)
                    .translationY(0f)
                    .start(view.signInForm);

            view.formDone.setOnClickListener(v -> {
                if (view.nameEdt.getText() == null || view.nameEdt.getText().toString().trim().length() == 0) {
                    Toast.makeText(v.getContext(), "Enter your name!", Toast.LENGTH_SHORT).show();
                    return;
                }
                Utils.hideSoftKeyboard(view.nameEdt);
                view.nameEdt.postDelayed(() -> {
                    int ageValue = age[ageLM.getSelectedPosition()];
                    int stepSizeValue = stepSize[stepSizeLM.getSelectedPosition()];
                    System.out.println(stepSizeValue);
                    boolean isMale = view.gender.isChecked();
                    Utils.save(v.getContext(), view.nameEdt.getText().toString().trim(), ageValue, stepSizeValue, isMale);

                    heightProvider.close();
                    heightProvider = null;

                    AXAnimation.create()
                            .duration(400)
                            .alpha(1f)
                            .withEndAction(a -> startHomePage())
                            .start(view.path_progress);

                    AXAnimation.create()
                            .duration(500)
                            .alpha(0f)
                            .translationY(initialTranslationY)
                            .withEndAction(a -> view.removeView(view.signInForm))
                            .start(view.signInForm);
                }, 500);
            });
        } else {
            view.removeView(view.signInForm);
            startHomePage();
        }
    }

    private void startHomePage() {
        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)
                view.bg.getLayoutParams();
        FrameLayout.LayoutParams lpCircle = (FrameLayout.LayoutParams) view.circle.getLayoutParams();
        FrameLayout.LayoutParams lpTitle = (FrameLayout.LayoutParams) view.title.getLayoutParams();

        int firstT = lpTitle.topMargin;
        int firstW = lpCircle.width;
        int firstH = lpCircle.height;
        GradientDrawable gd = new GradientDrawable();
        gd.setColor(Color.TRANSPARENT);

        IntEvaluator evaluator = new IntEvaluator();
        FloatEvaluator evaluator2 = new FloatEvaluator();
        ValueAnimator animator = ValueAnimator.ofFloat(0, 1);
        animator.setStartDelay(100);
        animator.setDuration(500);
        animator.addUpdateListener(a -> {
            lp.width = lpCircle.width = evaluator.evaluate(a.getAnimatedFraction(), firstW, homePagePositions[0]);
            lp.height = lpCircle.height = evaluator.evaluate(a.getAnimatedFraction(), firstH, homePagePositions[0]);
            lpCircle.leftMargin = evaluator.evaluate(a.getAnimatedFraction(), 0, view.getMeasuredWidth() / 2 - homePagePositions[0] / 2);
            lpCircle.topMargin = evaluator.evaluate(a.getAnimatedFraction(), 0, homePagePositions[1]);
            lpTitle.topMargin = firstT - lpCircle.topMargin;
            gd.setCornerRadius(evaluator2.evaluate(a.getAnimatedFraction(), 0, homePagePositions[0]));
            view.circle.setBackground(gd);

            view.circle.requestLayout();
        });
        animator.addListener(new AnimatorListenerAdapter() {
            @Override
            public void onAnimationEnd(Animator animation) {
                super.onAnimationEnd(animation);
                view.circle.setBackgroundResource(R.drawable.bg);
            }
        });
        animator.start();

        AXAnimation.create()
                .withSectionStartAction(a -> {
                    Utils.setStatusBarLight(MainActivity.this, true);
                    Utils.setNavigationBarLight(MainActivity.this, true);
                })
                .duration(400)
                .scale(1f)
                .start(view.title);

        view.path_progress.postDelayed(() -> {
            isWhiteLoading = false;
            updateLoadingColor();

            if (!ONLY_PHONE_DEBUG)
                checkForBluetooth();
        }, 280);

        //*/
        if (ONLY_PHONE_DEBUG)
            view.bg.setOnClickListener(v -> {
                connectAnimation();
                view.bg.setOnClickListener(null);
            });
        //*/


        AXAnimation.create()
                .delay(200)
                .duration(300)
                .alpha(1f)
                .start(view.connectionStatus);
    }

    private void connectAnimation() {
        initGraph();

        int screenWidth = view.getMeasuredWidth();
        int screenHeight = view.getMeasuredHeight();

        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)
                view.bg.getLayoutParams();
        lp.width = view.bg.getMeasuredWidth();
        lp.height = view.bg.getMeasuredHeight();

        FrameLayout.LayoutParams lpCircle = (FrameLayout.LayoutParams)
                view.circle.getLayoutParams();
        lpCircle.leftMargin = (int) (screenWidth / 2 - view.circle.getMeasuredWidth() / 2f);
        lpCircle.gravity = Gravity.TOP | Gravity.LEFT;
        view.circle.invalidate();

        int firstTop = lpCircle.topMargin;
        int firstSize = lp.width;

        FrameLayout.LayoutParams lpTitle = (FrameLayout.LayoutParams) view.title.getLayoutParams();
        lpTitle.gravity = Gravity.CENTER_HORIZONTAL;
        lpTitle.topMargin = lp.height / 2 - view.title.getMeasuredHeight() / 2;

        int titleTop = lpTitle.topMargin;

        ValueAnimator animator = ValueAnimator.ofInt(lp.width, screenHeight * 2);
        animator.setDuration(800);
        animator.addUpdateListener(a -> {
            lpCircle.width = lpCircle.height = (int) a.getAnimatedValue();
            lpCircle.topMargin = firstTop - (lpCircle.width - firstSize) / 2;

            lpCircle.leftMargin = (screenWidth / 2 - lpCircle.width / 2);

            if (lpCircle.leftMargin < 0)
                lp.leftMargin = -lpCircle.leftMargin;
            if (lpCircle.topMargin < 0) {
                lp.topMargin = -lpCircle.topMargin;
                lpTitle.topMargin = titleTop + firstTop;
            } else
                lpTitle.topMargin = titleTop + (firstTop - lpCircle.topMargin);

            lp.width = Math.min(lpCircle.width, screenWidth);
            lp.height = Math.min(lpCircle.height, screenHeight);
            view.circle.requestLayout();
        });
        animator.start();

        AXAnimation.create()
                .nextSectionWithDelay(200)
                .duration(400)
                .withSectionStartAction(a ->
                        Utils.setStatusBarLight(MainActivity.this, false))
                .withEndAction(a ->
                        Utils.setNavigationBarLight(MainActivity.this, false))
                .scale(2f)
                .start(view.title);

        FrameLayout.LayoutParams lp2 = (FrameLayout.LayoutParams)
                view.connected_content.getLayoutParams();
        lp2.leftMargin = (int) (screenWidth / 2f - getResources().getDisplayMetrics().density * 75);
        lp2.topMargin = (int) (screenHeight * 2f / 3f);
        AXAnimation.create()
                .nextSectionWithDelay(300)
                .withSectionStartAction(a -> view.connected_content.setVisibility(View.VISIBLE))
                .duration(350)
                .firstValueFromView(false)
                .alpha(0f, 1f)
                .scale(0.4f, 1f)
                .start(view.connected_content);

        AXAnimation.create()
                .duration(300)
                .alpha(0f)
                .withEndAction(a -> {
                    view.removeView(view.path_progress);
                    view.path_progress = null;
                })
                .start(view.path_progress);

        AXAnimation.create()
                .duration(300)
                .alpha(0f)
                .withEndAction(a -> {
                    view.removeView(view.connectionStatus);
                    view.connectionStatus = null;
                })
                .start(view.connectionStatus);

        view.start_btn.setOnClickListener(v -> {
            view.start_btn.setOnClickListener(null);
            letsGoAnimation();
        });
    }

    private void letsGoAnimation() {
        FrameLayout.LayoutParams lp = (FrameLayout.LayoutParams)
                view.bg.getLayoutParams();

        ((ViewGroup.MarginLayoutParams) view.statusLayout.getLayoutParams())
                .topMargin += Utils.getStatusBarHeight(this) + 20;

        view.removeView(view.login_content);
        lp.leftMargin = 0;
        lp.topMargin = 0;
        view.circle.removeView(view.bg);
        view.removeView(view.circle);
        view.addView(view.bg);

        AXAnimation.create()
                .withEndAction(a -> view.connected_content.setVisibility(View.VISIBLE))
                .duration(350)
                .alpha(1f)
                .scale(0.4f)
                .start(view.connected_content);

        AXAnimation.create()
                .duration(400)
                .scale(1f)
                .start(view.title);

        FrameLayout.LayoutParams lpTitle = (FrameLayout.LayoutParams) view.title.getLayoutParams();
        lpTitle.gravity = Gravity.TOP;

        int firstLeft = view.getMeasuredWidth() / 2 - view.title.getMeasuredWidth() / 2;

        float endRadius = getResources().getDisplayMetrics().density * 24;
        int endLeft = (int) (endRadius);

        int firstTop = lpTitle.topMargin;
        int endTop = endLeft + Utils.getStatusBarHeight(this);
        lpTitle.leftMargin = firstLeft;

        int firstHeight = lp.height;
        int endHeight = (int) (getResources().getDisplayMetrics().density * 300);

        ValueAnimator animator = ValueAnimator.ofFloat(0f, 1f);
        animator.setDuration(500);
        IntEvaluator evaluator = new IntEvaluator();
        FloatEvaluator floatEvaluator = new FloatEvaluator();

        animator.addUpdateListener(a -> {
            lpTitle.leftMargin = evaluator.evaluate(a.getAnimatedFraction(), firstLeft, endLeft);
            lpTitle.topMargin = evaluator.evaluate(a.getAnimatedFraction(), firstTop, endTop);
            lp.height = evaluator.evaluate(a.getAnimatedFraction(), firstHeight, endHeight);
            r = floatEvaluator.evaluate(a.getAnimatedFraction(), 0, endRadius);
            ((GradientDrawable) view.bg.getBackground()).setCornerRadii(
                    new float[]{0, 0, 0, 0, r, r, r, r}
            );
            view.bg.getBackground().invalidateSelf();
            view.bg.requestLayout();
        });
        animator.start();

        AXAnimation.create()
                .nextSectionWithDelay(250)
                .withSectionStartAction(a -> view.header_content.setVisibility(View.VISIBLE))
                .duration(350)
                .firstValueFromView(false)
                .alpha(0f, 1f)
                .scale(0.2f, 1f)
                .start(view.header_content);

        AXAnimation.create()
                .withSectionStartAction(a -> view.graph_view.setVisibility(View.VISIBLE))
                .duration(500)
                .firstValueFromView(false)
                .alpha(0f, 1f)
                .translationY(view.graph_view.getMeasuredHeight() / 2f, 0f)
                .start(view.graph_view);

        AXAnimation.create()
                .withSectionStartAction(a -> view.legend.setVisibility(View.VISIBLE))
                .duration(500)
                .firstValueFromView(false)
                .alpha(0f, 1f)
                .translationY(view.legend.getMeasuredHeight() / 2f, 0f)
                .start(view.legend);

        Utils.setNavigationBarLight(MainActivity.this, false);

        view.alarm.setOnClickListener(v -> {
            if (ONLY_PHONE_DEBUG || (bluetoothThread != null && bluetoothThread.isConnected())) {
                alarmDialog = new AlarmDialog(this, lastColors, this::bindAlarm);
                alarmDialog.setOnCancelListener(d -> alarmDialog = null);
                alarmDialog.setOnDismissListener(d -> alarmDialog = null);
                alarmDialog.show();
            }
        });
    }

    public void bindAlarm() {
        if (Utils.hasAlarm(this)) {
            int[] res = Utils.getAlarmPositions(this);
            String h = res[0] >= 10 ? String.valueOf(res[0]) : "0" + res[0];
            String m = res[1] >= 10 ? String.valueOf(res[1]) : "0" + res[1];
            String d = Utils.getDays()[res[2]];
            view.alarm_text.setText(MessageFormat.format("{0}:{1} {2}", h, m, d));

            if (bluetoothThread != null)
                bluetoothThread.updateAlarm(res);

        } else {
            view.alarm_text.setText("Set alarm");

            if (bluetoothThread != null)
                bluetoothThread.updateAlarm(null);
        }
    }

    // END OF PAGE ANIMATION

    // MESSAGE RECEIVER

    @Override
    public void receive(int type, Object data) {
        switch (type) {
            case MessageReceiver.TYPE_WAITING:
                view.connectionStatus.setText("Waiting for acknowledge...");
                break;
            case MessageReceiver.TYPE_REJECT:
                view.connectionStatus.setText("Rejected!");
                view.path_progress.cancelAnimation();
                break;
            case MessageReceiver.TYPE_DISCONNECT:
                if (view.connectionStatus == null) {
                    Toast.makeText(this, "Disconnected!", Toast.LENGTH_SHORT).show();
                } else {
                    view.connectionStatus.setText("Disconnected!");
                    view.path_progress.cancelAnimation();
                }
                break;
            case MessageReceiver.TYPE_CONNECT:
                connectAnimation();
                break;
            case MessageReceiver.TYPE_DATA:
                PPGData ppgData = (PPGData) data;

                if (graphFormula != null)
                    graphFormula.addData(view.graph_view, ppgData);

                if (hrvHandler != null && (graphFormula != null && graphFormula.size() > 1))
                    hrvHandler.addData(ppgData);

                SpannableStringBuilder st = new SpannableStringBuilder(
                        ppgData.getFormattedHR() + "\nbpm");
                st.setSpan(new RelativeSizeSpan(0.7f), st.length() - 3, st.length(),
                        Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                view.hrText.setText(st);

                st = new SpannableStringBuilder(
                        ppgData.getReadableSpO2() + "%");
                st.setSpan(new RelativeSizeSpan(0.7f), st.length() - 1, st.length(),
                        Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                view.spo2Text.setText(st);

                if (currentIndex != ppgData.getIndex())
                    switch (currentIndex = ppgData.getIndex()) {
                        case 1:
                            view.index_img.setImageResource(R.drawable.bad);
                            break;
                        case 2:
                            view.index_img.setImageResource(R.drawable.neut);
                            break;
                        default:
                            view.index_img.setImageResource(R.drawable.good);
                    }

                break;
            case MessageReceiver.TYPE_WATCH_DATA:
                WatchData watchData = (WatchData) data;
                view.stepCountText.setText(String.format(Locale.US, "%.2f km",
                        (watchData.getStepCount() * Utils.getStepSize(this) / 100000.0)));
                view.batteryText.setText(String.format(Locale.US, "%d%%", ((WatchData) data).getBattery()));
                break;
        }
    }

    // END OF MESSAGE RECEIVER
}