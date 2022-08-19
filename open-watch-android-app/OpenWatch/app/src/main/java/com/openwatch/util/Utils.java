package com.openwatch.util;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Path;
import android.os.Build;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

import com.openwatch.activity_main;

import java.util.Calendar;


public class Utils {

    public static int getStatusBarHeight(Context context) {
        int result = 0;
        int resourceId = context.getResources().getIdentifier("status_bar_height", "dimen", "android");
        if (resourceId > 0) {
            result = context.getResources().getDimensionPixelSize(resourceId);
        }
        return result;
    }

    public static void setStatusBarLight(Activity activity, boolean light) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            View v = activity.getWindow().getDecorView();
            if (light) {
                v.setSystemUiVisibility(v.getSystemUiVisibility() | View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            } else {
                v.setSystemUiVisibility(v.getSystemUiVisibility() ^ View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            }
        }
    }

    public static Path createPath(Context context) {
        float h = context.getResources().getDisplayMetrics().density * 40;
        float w = context.getResources().getDisplayMetrics().density * 56;

        float part = w / 4;
        float center = h / 2f;

        Path path = new Path();
        path.moveTo(0, center);
        path.lineTo(part, center);
        path.lineTo(part + part / 2f, h);
        path.lineTo(2 * part, center);
        path.lineTo(2 * part + part / 2f, 0);
        path.lineTo(3 * part, center);
        path.lineTo(4 * part, center);
        return path;
    }

    public static boolean needsSignInForm(Context context) {
        return /*!context.getSharedPreferences("OpenWatch", Context.MODE_PRIVATE)
                .contains("name");*/ true;
    }

    public static void save(Context context, String name, int age, int stepSize, boolean isMale) {
        SharedPreferences.Editor editor = context.getSharedPreferences("OpenWatch", Context.MODE_PRIVATE).edit();
        editor.putString("name", name);
        editor.putInt("birthYear", Calendar.getInstance().get(Calendar.YEAR) - age);
        editor.putInt("stepSize", stepSize);
        editor.putBoolean("gender", isMale);
        editor.apply();
    }

    public static void loadUISignInForm(Context context, activity_main view) {
        SharedPreferences pref = context.getSharedPreferences("OpenWatch", Context.MODE_PRIVATE);
        view.nameEdt.setText(pref.getString("name", ""));
        view.gender.setChecked(pref.getBoolean("gender", true));

        int this_year = Calendar.getInstance().get(Calendar.YEAR);
        int age = this_year - pref.getInt("birthYear", this_year - 20);
        int stepSize = pref.getInt("stepSize", 30);

        ((PickerLayoutManager) view.ageRV.getLayoutManager()).setSelectedPosition(age - 2);
        ((PickerLayoutManager) view.stepSizeRV.getLayoutManager()).setSelectedPosition(stepSize / 5 - 5);
    }

    public static int getStepSize(Context context){
        return context.getSharedPreferences("OpenWatch", Context.MODE_PRIVATE)
                .getInt("stepSize", 30);
    }

    public static String getName(Context context){
        return context.getSharedPreferences("OpenWatch", Context.MODE_PRIVATE)
                .getString("name", "");
    }

    public static void hideSoftKeyboard(View edt) {
        edt.clearFocus();
        InputMethodManager imm = (InputMethodManager) edt.getContext().getSystemService(Activity.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(edt.getWindowToken(), 0);
    }

}
