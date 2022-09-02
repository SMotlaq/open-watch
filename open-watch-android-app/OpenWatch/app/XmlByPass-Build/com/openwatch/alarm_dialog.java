package com.openwatch;

import android.content.Context;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.appcompat.widget.AppCompatTextView;
import androidx.cardview.widget.CardView;
import androidx.recyclerview.widget.RecyclerView;

public class alarm_dialog extends CardView {

	public AppCompatTextView title;
	public RecyclerView hourRV;
	public RecyclerView minRV;
	public RecyclerView dayRV;
	public FrameLayout cancel_parent;
	public AppCompatTextView cancel;
	public FrameLayout accept_parent;
	public AppCompatTextView accept;

	protected LinearLayout view1;
	protected LinearLayout view2;
	protected TextView view3;
	protected LinearLayout view4;

    public alarm_dialog(Context context) {
        this(context, null);
    }

    public alarm_dialog(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public alarm_dialog(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        init();
    }

	protected void init() {
		initThis();
		initView1();
		initTitle();
		initView2();
		initHourRV();
		initView3();
		initMinRV();
		initDayRV();
		initView4();
		initCancel_parent();
		initCancel();
		initAccept_parent();
		initAccept();
	}

	protected void initThis() {
		this.setCardElevation(dp(0f));
		this.setCardBackgroundColor(getResources().getColor(R.color.white));
		this.setRadius(dp(16f));
		FrameLayout.LayoutParams this_lp = new FrameLayout.LayoutParams(-1, dp(320f));
		this_lp.gravity = Gravity.CENTER;
		this.setLayoutParams(this_lp);
	}

	protected void initView1() {
		view1 = new LinearLayout(getContext());
		view1.setOrientation(1);
		FrameLayout.LayoutParams view1_lp = new FrameLayout.LayoutParams(-1, -2);
		view1_lp.gravity = Gravity.CENTER;
		this.addView(view1, view1_lp);
	}

	protected void initTitle() {
		title = new AppCompatTextView(getContext());
		title.setId(R.id.title);
		title.setText("Alarm");
		title.setTextSize(20);
		LinearLayout.LayoutParams title_lp = new LinearLayout.LayoutParams(-2, -2);
		title_lp.leftMargin = dp(24f);
		title_lp.topMargin = dp(24f);
		title_lp.rightMargin = dp(24f);
		title_lp.gravity = Gravity.CENTER_HORIZONTAL;
		view1.addView(title, title_lp);
	}

	protected void initView2() {
		view2 = new LinearLayout(getContext());
		view2.setGravity(Gravity.CENTER);
		view2.setOrientation(0);
		LinearLayout.LayoutParams view2_lp = new LinearLayout.LayoutParams(-1, dp(144f));
		view2_lp.leftMargin = dp(32f);
		view2_lp.topMargin = dp(12f);
		view2_lp.rightMargin = dp(32f);
		view1.addView(view2, view2_lp);
	}

	protected void initHourRV() {
		hourRV = new RecyclerView(getContext());
		hourRV.setId(R.id.hourRV);
		LinearLayout.LayoutParams hourRV_lp = new LinearLayout.LayoutParams(dp(0f), dp(120f));
		hourRV_lp.weight = (float) 0.4f;
		view2.addView(hourRV, hourRV_lp);
	}

	protected void initView3() {
		view3 = new TextView(getContext());
		view3.setTypeface(Typeface.DEFAULT_BOLD);
		view3.setText(":");
		view3.setTextSize(16);
		view3.setTextColor(getResources().getColor(R.color.black));
		view2.addView(view3, new LinearLayout.LayoutParams(-2, -2));
	}

	protected void initMinRV() {
		minRV = new RecyclerView(getContext());
		minRV.setId(R.id.minRV);
		LinearLayout.LayoutParams minRV_lp = new LinearLayout.LayoutParams(dp(0f), dp(120f));
		minRV_lp.weight = (float) 0.4f;
		view2.addView(minRV, minRV_lp);
	}

	protected void initDayRV() {
		dayRV = new RecyclerView(getContext());
		dayRV.setId(R.id.dayRV);
		LinearLayout.LayoutParams dayRV_lp = new LinearLayout.LayoutParams(dp(0f), dp(120f));
		dayRV_lp.weight = (float) 0.6f;
		view2.addView(dayRV, dayRV_lp);
	}

	protected void initView4() {
		view4 = new LinearLayout(getContext());
		LinearLayout.LayoutParams view4_lp = new LinearLayout.LayoutParams(dp(268f), dp(46f));
		view4_lp.topMargin = dp(24f);
		view4_lp.bottomMargin = dp(30f);
		view4_lp.gravity = Gravity.CENTER;
		view1.addView(view4, view4_lp);
	}

	protected void initCancel_parent() {
		cancel_parent = new FrameLayout(getContext());
		cancel_parent.setBackgroundResource(R.drawable.bg_trans);
		cancel_parent.setId(R.id.cancel_parent);
		LinearLayout.LayoutParams cancel_parent_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		cancel_parent_lp.weight = (float) 0.5f;
		view4.addView(cancel_parent, cancel_parent_lp);
	}

	protected void initCancel() {
		cancel = new AppCompatTextView(getContext());
		cancel.setBackgroundResource(resolveAttribute(R.attr.selectableItemBackground));
		cancel.setId(R.id.cancel);
		cancel.setTextSize(18);
		cancel.setText("Delete");
		cancel.setGravity(Gravity.CENTER);
		cancel.setTextColor(0xFFFF0000);
		FrameLayout.LayoutParams cancel_lp = new FrameLayout.LayoutParams(-1, -1);
		cancel_lp.gravity = Gravity.CENTER;
		cancel_parent.addView(cancel, cancel_lp);
	}

	protected void initAccept_parent() {
		accept_parent = new FrameLayout(getContext());
		accept_parent.setId(R.id.accept_parent);
		LinearLayout.LayoutParams accept_parent_lp = new LinearLayout.LayoutParams(dp(0f), -1);
		accept_parent_lp.weight = (float) 0.5f;
		view4.addView(accept_parent, accept_parent_lp);
	}

	protected void initAccept() {
		accept = new AppCompatTextView(getContext());
		accept.setBackgroundResource(resolveAttribute(R.attr.selectableItemBackground));
		accept.setId(R.id.accept);
		accept.setTextSize(18);
		accept.setText("Save");
		accept.setGravity(Gravity.CENTER);
		accept.setTextColor(getResources().getColor(R.color.white));
		FrameLayout.LayoutParams accept_lp = new FrameLayout.LayoutParams(-1, -1);
		accept_lp.gravity = Gravity.CENTER;
		accept_parent.addView(accept, accept_lp);
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