package com.openwatch;

import android.content.Context;
import android.content.res.ColorStateList;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatEditText;
import androidx.appcompat.widget.AppCompatImageView;
import androidx.appcompat.widget.SwitchCompat;
import androidx.cardview.widget.CardView;
import androidx.recyclerview.widget.RecyclerView;

import com.aghajari.axanimation.layouts.AXAFrameLayout;
import com.aghajari.chart.HRVChart;
import com.aghajari.graphview.AXGraphView;
import com.aghajari.pathloading.AXPathLoadingView;

public class activity_main extends AXAFrameLayout {

	public FrameLayout login_content;
	public FrameLayout circle;
	public FrameLayout bg;
	public FrameLayout header_content;
	public TextView hrText;
	public AppCompatImageView index_img;
	public TextView spo2Text;
	public CardView alarm;
	public TextView alarm_text;
	public LinearLayout statusLayout;
	public TextView stepCountText;
	public TextView batteryText;
	public CardView hrv;
	public TextView title;
	public LinearLayout connected_content;
	public FrameLayout start_btn;
	public FrameLayout hrv_panel;
	public CardView hrv_back;
	public TextView hrv_avg;
	public FrameLayout hrv_pnl;
	public HRVChart hrv_chart;
	public TextView hrv_title;
	public AXGraphView graph_view;
	public LinearLayout legend;
	public View legendHR;
	public TextView legendHRText;
	public View legendSpO2;
	public TextView legendSpO2Text;
	public AXPathLoadingView path_progress;
	public TextView connectionStatus;
	public FrameLayout signInForm;
	public FrameLayout formDone;
	public AppCompatEditText nameEdt;
	public SwitchCompat gender;
	public RecyclerView ageRV;
	public RecyclerView stepSizeRV;

	protected AppCompatImageView view1;
	protected LinearLayout view2;
	protected FrameLayout view3;
	protected AppCompatImageView view4;
	protected FrameLayout view5;
	protected FrameLayout view6;
	protected AppCompatImageView view7;
	protected LinearLayout view8;
	protected AppCompatImageView view9;
	protected AppCompatImageView view10;
	protected AppCompatImageView view11;
	protected AppCompatImageView view12;
	protected TextView view13;
	protected TextView view14;
	protected AppCompatImageView view15;
	protected LinearLayout view16;
	protected LinearLayout view17;
	protected CardView view18;
	protected LinearLayout view19;
	protected FrameLayout view20;
	protected TextView view21;
	protected AppCompatImageView view22;
	protected TextView view23;
	protected LinearLayout view24;
	protected LinearLayout view25;
	protected AppCompatImageView view26;
	protected AppCompatImageView view27;
	protected LinearLayout view28;
	protected FrameLayout view29;
	protected TextView view30;
	protected FrameLayout view31;
	protected TextView view32;

    public activity_main(Context context) {
        this(context, null);
    }

    public activity_main(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public activity_main(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        init();
    }

	protected void init() {
		initThis();
		initLogin_content();
		initView1();
		initCircle();
		initBg();
		initHeader_content();
		initView2();
		initView3();
		initView4();
		initHrText();
		initView5();
		initIndex_img();
		initView6();
		initView7();
		initSpo2Text();
		initAlarm();
		initView8();
		initView9();
		initAlarm_text();
		initStatusLayout();
		initView10();
		initStepCountText();
		initView11();
		initBatteryText();
		initHrv();
		initView12();
		initTitle();
		initConnected_content();
		initView13();
		initStart_btn();
		initView14();
		initHrv_panel();
		initHrv_back();
		initView15();
		initHrv_avg();
		initHrv_pnl();
		initHrv_chart();
		initHrv_title();
		initGraph_view();
		initLegend();
		initView16();
		initLegendHR();
		initLegendHRText();
		initView17();
		initLegendSpO2();
		initLegendSpO2Text();
		initPath_progress();
		initConnectionStatus();
		initSignInForm();
		initView18();
		initView19();
		initView20();
		initView21();
		initFormDone();
		initView22();
		initView23();
		initView24();
		initNameEdt();
		initView25();
		initView26();
		initGender();
		initView27();
		initView28();
		initView29();
		initView30();
		initAgeRV();
		initView31();
		initView32();
		initStepSizeRV();
	}

	protected void initThis() {
		this.setBackgroundColor(getResources().getColor(R.color.white));
		this.setLayoutParams(new AXAFrameLayout.LayoutParams(-1, -1));
	}

	protected void initLogin_content() {
		login_content = new FrameLayout(getContext());
		login_content.setId(R.id.login_content);
		this.addView(login_content, new AXAFrameLayout.LayoutParams(-1, -1));
	}

	protected void initView1() {
		view1 = new AppCompatImageView(getContext());
		view1.setImageResource(R.drawable.watch);
		view1.setTranslationX(dp(3f));
		FrameLayout.LayoutParams view1_lp = new FrameLayout.LayoutParams(dp(380f), dp(380f));
		view1_lp.topMargin = dp(56f);
		view1_lp.gravity = Gravity.CENTER_HORIZONTAL;
		login_content.addView(view1, view1_lp);
	}

	protected void initCircle() {
		circle = new FrameLayout(getContext());
		circle.setId(R.id.circle);
		FrameLayout.LayoutParams circle_lp = new FrameLayout.LayoutParams(dp(162f), dp(162f));
		circle_lp.topMargin = dp(165f);
		circle_lp.gravity = Gravity.CENTER_HORIZONTAL;
		this.addView(circle, circle_lp);
	}

	protected void initBg() {
		bg = new FrameLayout(getContext());
		bg.setId(R.id.bg);
		bg.setElevation(dp(8f));
		circle.addView(bg, new FrameLayout.LayoutParams(-1, -1));
	}

	protected void initHeader_content() {
		header_content = new FrameLayout(getContext());
		header_content.setId(R.id.header_content);
		header_content.setVisibility(View.GONE);
		bg.addView(header_content, new FrameLayout.LayoutParams(-1, dp(300f)));
	}

	protected void initView2() {
		view2 = new LinearLayout(getContext());
		view2.setOrientation(0);
		header_content.addView(view2, new FrameLayout.LayoutParams(-1, -1));
	}

	protected void initView3() {
		view3 = new FrameLayout(getContext());
		LinearLayout.LayoutParams view3_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		view3_lp.weight = (float) 0.5f;
		view2.addView(view3, view3_lp);
	}

	protected void initView4() {
		view4 = new AppCompatImageView(getContext());
		view4.setImageResource(R.drawable.hearbeat);
		view4.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		FrameLayout.LayoutParams view4_lp = new FrameLayout.LayoutParams(dp(50f), dp(50f));
		view4_lp.topMargin = dp(10f);
		view4_lp.gravity = Gravity.CENTER;
		view3.addView(view4, view4_lp);
	}

	protected void initHrText() {
		hrText = new TextView(getContext());
		hrText.setId(R.id.hrText);
		hrText.setTypeface(Typeface.DEFAULT_BOLD);
		hrText.setTextSize(20);
		hrText.setGravity(Gravity.CENTER);
		hrText.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams hrText_lp = new FrameLayout.LayoutParams(-2, -2);
		hrText_lp.topMargin = dp(56f);
		hrText_lp.gravity = Gravity.CENTER;
		view3.addView(hrText, hrText_lp);
	}

	protected void initView5() {
		view5 = new FrameLayout(getContext());
		LinearLayout.LayoutParams view5_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		view5_lp.weight = (float) 0.5f;
		view2.addView(view5, view5_lp);
	}

	protected void initIndex_img() {
		index_img = new AppCompatImageView(getContext());
		index_img.setId(R.id.index_img);
		index_img.setImageResource(R.drawable.good);
		index_img.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		FrameLayout.LayoutParams index_img_lp = new FrameLayout.LayoutParams(dp(150f), dp(150f));
		index_img_lp.topMargin = dp(10f);
		index_img_lp.gravity = Gravity.CENTER;
		view5.addView(index_img, index_img_lp);
	}

	protected void initView6() {
		view6 = new FrameLayout(getContext());
		LinearLayout.LayoutParams view6_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		view6_lp.weight = (float) 0.5f;
		view2.addView(view6, view6_lp);
	}

	protected void initView7() {
		view7 = new AppCompatImageView(getContext());
		view7.setImageResource(R.drawable.spo2);
		view7.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		FrameLayout.LayoutParams view7_lp = new FrameLayout.LayoutParams(dp(56f), dp(56f));
		view7_lp.topMargin = dp(10f);
		view7_lp.gravity = Gravity.CENTER;
		view6.addView(view7, view7_lp);
	}

	protected void initSpo2Text() {
		spo2Text = new TextView(getContext());
		spo2Text.setId(R.id.spo2Text);
		spo2Text.setTypeface(Typeface.DEFAULT_BOLD);
		spo2Text.setTextSize(20);
		spo2Text.setTextColor(getResources().getColor(R.color.white));
		spo2Text.setTranslationX(dp(4f));
		FrameLayout.LayoutParams spo2Text_lp = new FrameLayout.LayoutParams(-2, -2);
		spo2Text_lp.topMargin = dp(56f);
		spo2Text_lp.gravity = Gravity.CENTER;
		view6.addView(spo2Text, spo2Text_lp);
	}

	protected void initAlarm() {
		alarm = new CardView(getContext());
		alarm.setId(R.id.alarm);
		alarm.setContentPadding(dp(12f), 0, dp(12f), 0);
		alarm.setCardElevation(dp(0f));
		alarm.setCardBackgroundColor(0x20000000);
		alarm.setRadius(dp(16f));
		FrameLayout.LayoutParams alarm_lp = new FrameLayout.LayoutParams(-2, dp(32f));
		alarm_lp.bottomMargin = dp(24f);
		alarm_lp.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		header_content.addView(alarm, alarm_lp);
	}

	protected void initView8() {
		view8 = new LinearLayout(getContext());
		view8.setGravity(Gravity.CENTER);
		view8.setOrientation(0);
		alarm.addView(view8, new CardView.LayoutParams(-2, -1));
	}

	protected void initView9() {
		view9 = new AppCompatImageView(getContext());
		view9.setImageResource(R.drawable.alarm);
		LinearLayout.LayoutParams view9_lp = new LinearLayout.LayoutParams(dp(20f), dp(20f));
		view9_lp.rightMargin = dp(3f);
		view9_lp.gravity = Gravity.CENTER_VERTICAL;
		view8.addView(view9, view9_lp);
	}

	protected void initAlarm_text() {
		alarm_text = new TextView(getContext());
		alarm_text.setId(R.id.alarm_text);
		alarm_text.setText("Set alarm");
		alarm_text.setTextSize(13);
		alarm_text.setTextColor(getResources().getColor(R.color.white));
		LinearLayout.LayoutParams alarm_text_lp = new LinearLayout.LayoutParams(-2, -2);
		alarm_text_lp.leftMargin = dp(3f);
		view8.addView(alarm_text, alarm_text_lp);
	}

	protected void initStatusLayout() {
		statusLayout = new LinearLayout(getContext());
		statusLayout.setId(R.id.statusLayout);
		statusLayout.setGravity(Gravity.CENTER_VERTICAL);
		statusLayout.setOrientation(0);
		FrameLayout.LayoutParams statusLayout_lp = new FrameLayout.LayoutParams(-2, -2);
		statusLayout_lp.topMargin = dp(24f);
		statusLayout_lp.rightMargin = dp(24f);
		statusLayout_lp.gravity = Gravity.RIGHT;
		header_content.addView(statusLayout, statusLayout_lp);
	}

	protected void initView10() {
		view10 = new AppCompatImageView(getContext());
		view10.setImageResource(R.drawable.distance);
		statusLayout.addView(view10, new LinearLayout.LayoutParams(dp(18f), dp(18f)));
	}

	protected void initStepCountText() {
		stepCountText = new TextView(getContext());
		stepCountText.setId(R.id.stepCountText);
		stepCountText.setText("0.0 km");
		stepCountText.setTextSize(12);
		stepCountText.setTextColor(getResources().getColor(R.color.white));
		LinearLayout.LayoutParams stepCountText_lp = new LinearLayout.LayoutParams(-2, -2);
		stepCountText_lp.leftMargin = dp(2f);
		stepCountText_lp.rightMargin = dp(8f);
		statusLayout.addView(stepCountText, stepCountText_lp);
	}

	protected void initView11() {
		view11 = new AppCompatImageView(getContext());
		view11.setImageResource(R.drawable.battery);
		statusLayout.addView(view11, new LinearLayout.LayoutParams(dp(18f), dp(18f)));
	}

	protected void initBatteryText() {
		batteryText = new TextView(getContext());
		batteryText.setId(R.id.batteryText);
		batteryText.setText("100%");
		batteryText.setTextSize(12);
		batteryText.setTextColor(getResources().getColor(R.color.white));
		statusLayout.addView(batteryText, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initHrv() {
		hrv = new CardView(getContext());
		hrv.setId(R.id.hrv);
		hrv.setCardElevation(dp(0f));
		hrv.setCardBackgroundColor(0x20000000);
		hrv.setRadius(dp(16f));
		FrameLayout.LayoutParams hrv_lp = new FrameLayout.LayoutParams(dp(32f), dp(32f));
		hrv_lp.rightMargin = dp(24f);
		hrv_lp.bottomMargin = dp(24f);
		hrv_lp.gravity = Gravity.BOTTOM|Gravity.RIGHT;
		header_content.addView(hrv, hrv_lp);
	}

	protected void initView12() {
		view12 = new AppCompatImageView(getContext());
		view12.setPadding(dp(8f), dp(8f), dp(8f), dp(8f));
		view12.setImageResource(R.drawable.hrv);
		hrv.addView(view12, new CardView.LayoutParams(-1, -1));
	}

	protected void initTitle() {
		title = new TextView(getContext());
		title.setId(R.id.title);
		title.setTypeface(Typeface.DEFAULT_BOLD);
		title.setText("OpenWatch");
		title.setTextSize(20);
		title.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams title_lp = new FrameLayout.LayoutParams(-2, -2);
		title_lp.gravity = Gravity.CENTER;
		bg.addView(title, title_lp);
	}

	protected void initConnected_content() {
		connected_content = new LinearLayout(getContext());
		connected_content.setId(R.id.connected_content);
		connected_content.setGravity(Gravity.CENTER);
		connected_content.setVisibility(View.GONE);
		connected_content.setOrientation(1);
		bg.addView(connected_content, new FrameLayout.LayoutParams(-2, -2));
	}

	protected void initView13() {
		view13 = new TextView(getContext());
		view13.setText("Connected");
		view13.setTextSize(14);
		view13.setTextColor(getResources().getColor(R.color.white));
		connected_content.addView(view13, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initStart_btn() {
		start_btn = new FrameLayout(getContext());
		start_btn.setId(R.id.start_btn);
		start_btn.setBackgroundResource(R.drawable.bg_connect);
		start_btn.setElevation(dp(2f));
		LinearLayout.LayoutParams start_btn_lp = new LinearLayout.LayoutParams(dp(150f), dp(44f));
		start_btn_lp.topMargin = dp(16f);
		connected_content.addView(start_btn, start_btn_lp);
	}

	protected void initView14() {
		view14 = new TextView(getContext());
		view14.setText("Let's Go");
		view14.setTextSize(15);
		view14.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams view14_lp = new FrameLayout.LayoutParams(-2, -2);
		view14_lp.gravity = Gravity.CENTER;
		start_btn.addView(view14, view14_lp);
	}

	protected void initHrv_panel() {
		hrv_panel = new FrameLayout(getContext());
		hrv_panel.setId(R.id.hrv_panel);
		hrv_panel.setBackgroundResource(R.drawable.bg_hrv);
		hrv_panel.setVisibility(View.GONE);
		bg.addView(hrv_panel, new FrameLayout.LayoutParams(-1, -1));
	}

	protected void initHrv_back() {
		hrv_back = new CardView(getContext());
		hrv_back.setId(R.id.hrv_back);
		hrv_back.setCardElevation(dp(0f));
		hrv_back.setCardBackgroundColor(0x10000000);
		hrv_back.setRadius(dp(16f));
		FrameLayout.LayoutParams hrv_back_lp = new FrameLayout.LayoutParams(dp(32f), dp(32f));
		hrv_back_lp.rightMargin = dp(24f);
		hrv_back_lp.bottomMargin = dp(24f);
		hrv_back_lp.gravity = Gravity.BOTTOM|Gravity.RIGHT;
		hrv_panel.addView(hrv_back, hrv_back_lp);
	}

	protected void initView15() {
		view15 = new AppCompatImageView(getContext());
		view15.setPadding(dp(8f), dp(8f), dp(8f), dp(8f));
		view15.setImageResource(R.drawable.hrv);
		view15.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.black)));
		hrv_back.addView(view15, new CardView.LayoutParams(-1, -1));
	}

	protected void initHrv_avg() {
		hrv_avg = new TextView(getContext());
		hrv_avg.setId(R.id.hrv_avg);
		hrv_avg.setTextSize(18);
		hrv_avg.setGravity(Gravity.CENTER);
		FrameLayout.LayoutParams hrv_avg_lp = new FrameLayout.LayoutParams(-2, dp(32f));
		hrv_avg_lp.leftMargin = dp(24f);
		hrv_avg_lp.bottomMargin = dp(24f);
		hrv_avg_lp.gravity = Gravity.BOTTOM;
		hrv_panel.addView(hrv_avg, hrv_avg_lp);
	}

	protected void initHrv_pnl() {
		hrv_pnl = new FrameLayout(getContext());
		hrv_pnl.setId(R.id.hrv_pnl);
		FrameLayout.LayoutParams hrv_pnl_lp = new FrameLayout.LayoutParams(-2, -2);
		hrv_pnl_lp.topMargin = dp(24f);
		hrv_pnl_lp.bottomMargin = dp(70f);
		hrv_panel.addView(hrv_pnl, hrv_pnl_lp);
	}

	protected void initHrv_chart() {
		hrv_chart = new HRVChart(getContext());
		hrv_chart.setId(R.id.hrv_chart);
		hrv_pnl.addView(hrv_chart, new FrameLayout.LayoutParams(-1, -1));
	}

	protected void initHrv_title() {
		hrv_title = new TextView(getContext());
		hrv_title.setId(R.id.hrv_title);
		hrv_title.setTextSize(18);
		hrv_title.setText("HR");
		hrv_title.setTextColor(getResources().getColor(R.color.black));
		hrv_title.setRotation(90);
		FrameLayout.LayoutParams hrv_title_lp = new FrameLayout.LayoutParams(-2, -2);
		hrv_title_lp.gravity = Gravity.CENTER_VERTICAL;
		hrv_pnl.addView(hrv_title, hrv_title_lp);
	}

	protected void initGraph_view() {
		graph_view = new AXGraphView(getContext());
		graph_view.setId(R.id.graph_view);
		graph_view.setVisibility(View.INVISIBLE);
		AXAFrameLayout.LayoutParams graph_view_lp = new AXAFrameLayout.LayoutParams(-1, -1);
		graph_view_lp.topMargin = dp(276f);
		this.addView(graph_view, graph_view_lp);
	}

	protected void initLegend() {
		legend = new LinearLayout(getContext());
		legend.setId(R.id.legend);
		legend.setVisibility(View.INVISIBLE);
		legend.setOrientation(1);
		FrameLayout.LayoutParams legend_lp = new FrameLayout.LayoutParams(-2, -2);
		legend_lp.rightMargin = dp(24f);
		legend_lp.bottomMargin = dp(56f);
		legend_lp.gravity = Gravity.BOTTOM|Gravity.RIGHT;
		this.addView(legend, legend_lp);
	}

	protected void initView16() {
		view16 = new LinearLayout(getContext());
		view16.setGravity(Gravity.CENTER);
		view16.setOrientation(0);
		legend.addView(view16, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initLegendHR() {
		legendHR = new View(getContext());
		legendHR.setBackgroundColor(getResources().getColor(R.color.black));
		legendHR.setId(R.id.legendHR);
		LinearLayout.LayoutParams legendHR_lp = new LinearLayout.LayoutParams(dp(20f), dp(2f));
		legendHR_lp.rightMargin = dp(12f);
		view16.addView(legendHR, legendHR_lp);
	}

	protected void initLegendHRText() {
		legendHRText = new TextView(getContext());
		legendHRText.setId(R.id.legendHRText);
		legendHRText.setText("Red");
		legendHRText.setTextSize(13);
		legendHRText.setTextColor(getResources().getColor(R.color.black));
		view16.addView(legendHRText, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initView17() {
		view17 = new LinearLayout(getContext());
		view17.setGravity(Gravity.CENTER);
		view17.setOrientation(0);
		legend.addView(view17, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initLegendSpO2() {
		legendSpO2 = new View(getContext());
		legendSpO2.setBackgroundColor(getResources().getColor(R.color.black));
		legendSpO2.setId(R.id.legendSpO2);
		LinearLayout.LayoutParams legendSpO2_lp = new LinearLayout.LayoutParams(dp(20f), dp(2f));
		legendSpO2_lp.rightMargin = dp(12f);
		view17.addView(legendSpO2, legendSpO2_lp);
	}

	protected void initLegendSpO2Text() {
		legendSpO2Text = new TextView(getContext());
		legendSpO2Text.setId(R.id.legendSpO2Text);
		legendSpO2Text.setText("iRed");
		legendSpO2Text.setTextSize(13);
		legendSpO2Text.setTextColor(getResources().getColor(R.color.black));
		view17.addView(legendSpO2Text, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initPath_progress() {
		path_progress = new AXPathLoadingView(getContext());
		path_progress.setId(R.id.path_progress);
		FrameLayout.LayoutParams path_progress_lp = new FrameLayout.LayoutParams(dp(100f), dp(44f));
		path_progress_lp.bottomMargin = dp(150f);
		path_progress_lp.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		this.addView(path_progress, path_progress_lp);
	}

	protected void initConnectionStatus() {
		connectionStatus = new TextView(getContext());
		connectionStatus.setId(R.id.connectionStatus);
		connectionStatus.setText("Connecting...");
		connectionStatus.setTextSize(12);
		connectionStatus.setAlpha(0);
		connectionStatus.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams connectionStatus_lp = new FrameLayout.LayoutParams(-2, -2);
		connectionStatus_lp.bottomMargin = dp(120f);
		connectionStatus_lp.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		this.addView(connectionStatus, connectionStatus_lp);
	}

	protected void initSignInForm() {
		signInForm = new FrameLayout(getContext());
		signInForm.setId(R.id.signInForm);
		signInForm.setVisibility(View.INVISIBLE);
		signInForm.setAlpha(0);
		signInForm.setTranslationY(dp(424f));
		FrameLayout.LayoutParams signInForm_lp = new FrameLayout.LayoutParams(-1, dp(424f));
		signInForm_lp.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		this.addView(signInForm, signInForm_lp);
	}

	protected void initView18() {
		view18 = new CardView(getContext());
		view18.setCardElevation(dp(0f));
		view18.setCardBackgroundColor(0x20000000);
		view18.setContentPadding(dp(24f), dp(24f), dp(24f), dp(24f));
		view18.setRadius(dp(12f));
		FrameLayout.LayoutParams view18_lp = new FrameLayout.LayoutParams(-1, dp(332f));
		view18_lp.leftMargin = dp(24f);
		view18_lp.rightMargin = dp(24f);
		view18_lp.bottomMargin = dp(60f);
		view18_lp.gravity = Gravity.BOTTOM|Gravity.CENTER_HORIZONTAL;
		signInForm.addView(view18, view18_lp);
	}

	protected void initView19() {
		view19 = new LinearLayout(getContext());
		view19.setOrientation(1);
		view18.addView(view19, new CardView.LayoutParams(-1, -1));
	}

	protected void initView20() {
		view20 = new FrameLayout(getContext());
		view19.addView(view20, new LinearLayout.LayoutParams(-1, -2));
	}

	protected void initView21() {
		view21 = new TextView(getContext());
		view21.setTypeface(Typeface.DEFAULT_BOLD);
		view21.setText("Hello!");
		view21.setTextSize(26);
		view21.setTextColor(getResources().getColor(R.color.white));
		view20.addView(view21, new FrameLayout.LayoutParams(-2, -2));
	}

	protected void initFormDone() {
		formDone = new FrameLayout(getContext());
		formDone.setBackgroundResource(resolveAttribute(R.attr.selectableItemBackgroundBorderless));
		formDone.setId(R.id.formDone);
		FrameLayout.LayoutParams formDone_lp = new FrameLayout.LayoutParams(dp(35f), dp(35f));
		formDone_lp.gravity = Gravity.RIGHT;
		view20.addView(formDone, formDone_lp);
	}

	protected void initView22() {
		view22 = new AppCompatImageView(getContext());
		view22.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		view22.setImageResource(R.drawable.check);
		FrameLayout.LayoutParams view22_lp = new FrameLayout.LayoutParams(dp(18f), dp(18f));
		view22_lp.gravity = Gravity.CENTER;
		formDone.addView(view22, view22_lp);
	}

	protected void initView23() {
		view23 = new TextView(getContext());
		view23.setText("Please fill the form for a better analysis.");
		view23.setTextSize(14);
		view23.setTextColor(getResources().getColor(R.color.white));
		view19.addView(view23, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initView24() {
		view24 = new LinearLayout(getContext());
		view24.setOrientation(0);
		LinearLayout.LayoutParams view24_lp = new LinearLayout.LayoutParams(-1, -2);
		view24_lp.topMargin = dp(12f);
		view24_lp.gravity = Gravity.CENTER;
		view19.addView(view24, view24_lp);
	}

	protected void initNameEdt() {
		nameEdt = new AppCompatEditText(getContext());
		nameEdt.setId(R.id.nameEdt);
		nameEdt.setHintTextColor(0xFFBBBBBB);
		nameEdt.setImeOptions(0x00000006);
		nameEdt.setTextSize(16);
		nameEdt.setSingleLine(true);
		nameEdt.setHint("Your Name");
		nameEdt.setTextColor(getResources().getColor(R.color.white));
		LinearLayout.LayoutParams nameEdt_lp = new LinearLayout.LayoutParams(dp(0f), -2);
		nameEdt_lp.rightMargin = dp(12f);
		nameEdt_lp.weight = (float) 0.8f;
		view24.addView(nameEdt, nameEdt_lp);
	}

	protected void initView25() {
		view25 = new LinearLayout(getContext());
		view25.setGravity(Gravity.CENTER);
		view25.setOrientation(0);
		LinearLayout.LayoutParams view25_lp = new LinearLayout.LayoutParams(-2, -1);
		view25_lp.leftMargin = dp(12f);
		view24.addView(view25, view25_lp);
	}

	protected void initView26() {
		view26 = new AppCompatImageView(getContext());
		view26.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		view26.setImageResource(R.drawable.female);
		LinearLayout.LayoutParams view26_lp = new LinearLayout.LayoutParams(dp(18f), dp(18f));
		view26_lp.rightMargin = dp(8f);
		view25.addView(view26, view26_lp);
	}

	protected void initGender() {
		gender = new SwitchCompat(getContext());
		gender.setId(R.id.gender);
		gender.setChecked(true);
		view25.addView(gender, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initView27() {
		view27 = new AppCompatImageView(getContext());
		view27.setImageTintList(ColorStateList.valueOf(getResources().getColor(R.color.white)));
		view27.setImageResource(R.drawable.male);
		LinearLayout.LayoutParams view27_lp = new LinearLayout.LayoutParams(dp(18f), dp(18f));
		view27_lp.leftMargin = dp(8f);
		view25.addView(view27, view27_lp);
	}

	protected void initView28() {
		view28 = new LinearLayout(getContext());
		view28.setGravity(Gravity.CENTER);
		view28.setOrientation(0);
		LinearLayout.LayoutParams view28_lp = new LinearLayout.LayoutParams(-1, dp(144f));
		view28_lp.topMargin = dp(24f);
		view19.addView(view28, view28_lp);
	}

	protected void initView29() {
		view29 = new FrameLayout(getContext());
		LinearLayout.LayoutParams view29_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		view29_lp.weight = (float) 0.5f;
		view28.addView(view29, view29_lp);
	}

	protected void initView30() {
		view30 = new TextView(getContext());
		view30.setText("Age");
		view30.setTextSize(16);
		view30.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams view30_lp = new FrameLayout.LayoutParams(-2, -2);
		view30_lp.gravity = Gravity.CENTER_HORIZONTAL;
		view29.addView(view30, view30_lp);
	}

	protected void initAgeRV() {
		ageRV = new RecyclerView(getContext());
		ageRV.setId(R.id.ageRV);
		FrameLayout.LayoutParams ageRV_lp = new FrameLayout.LayoutParams(-1, dp(120f));
		ageRV_lp.topMargin = dp(28f);
		view29.addView(ageRV, ageRV_lp);
	}

	protected void initView31() {
		view31 = new FrameLayout(getContext());
		LinearLayout.LayoutParams view31_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		view31_lp.weight = (float) 0.5f;
		view28.addView(view31, view31_lp);
	}

	protected void initView32() {
		view32 = new TextView(getContext());
		view32.setText("StepSize (CM)");
		view32.setTextSize(16);
		view32.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams view32_lp = new FrameLayout.LayoutParams(-2, -2);
		view32_lp.gravity = Gravity.CENTER_HORIZONTAL;
		view31.addView(view32, view32_lp);
	}

	protected void initStepSizeRV() {
		stepSizeRV = new RecyclerView(getContext());
		stepSizeRV.setId(R.id.stepSizeRV);
		FrameLayout.LayoutParams stepSizeRV_lp = new FrameLayout.LayoutParams(-1, dp(120f));
		stepSizeRV_lp.topMargin = dp(28f);
		view31.addView(stepSizeRV, stepSizeRV_lp);
	}

    private int dp(float value) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,
                value, getResources().getDisplayMetrics());
    }

    private int resolveAttribute(int attrId){
        TypedValue outValue = new TypedValue();
        getContext().getTheme().resolveAttribute(attrId, outValue, true);
        return outValue.resourceId;
    }

}