package com.openwatch.util;

import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.os.Handler;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.PopupWindow;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * The keyboard height provider, this class uses a PopupWindow
 * to calculate the window height when the floating keyboard is opened and closed.
 */
public class KeyboardHeightProvider extends PopupWindow {

    private final EditText edt;
    private final View popupView;
    private final View parentView;
    private final OnKeyboardHeightChanged listener;

    public interface OnKeyboardHeightChanged {
        void onChange(int height);
    }

    /**
     * Construct a new KeyboardHeightProvider
     */
    public KeyboardHeightProvider(EditText edt, Activity activity, OnKeyboardHeightChanged listener) {
        super(activity);
        this.edt = edt;

        this.popupView = new View(activity);
        this.listener = listener;
        setContentView(popupView);

        setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE | WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
        setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);

        parentView = activity.findViewById(android.R.id.content);

        setWidth(0);
        setHeight(WindowManager.LayoutParams.MATCH_PARENT);

        popupView.getViewTreeObserver().addOnGlobalLayoutListener(this::handleOnGlobalLayout);
    }

    /**
     * Start the KeyboardHeightProvider, this must be called after the onResume of the Activity.
     * PopupWindows are not allowed to be registered before the onResume has finished
     * of the Activity.
     */
    public void start() {
        if (!isShowing() && parentView.getWindowToken() != null) {
            setBackgroundDrawable(new ColorDrawable(0));
            showAtLocation(parentView, Gravity.NO_GRAVITY, 0, 0);
        }
    }

    public void stickOnStart() {
        final Handler handler = new Handler();
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (!isShowing() && parentView.getWindowToken() != null) {
                    setBackgroundDrawable(new ColorDrawable(0));
                    showAtLocation(parentView, Gravity.NO_GRAVITY, 0, 0);
                    return;
                }
                if (!isShowing()) {
                    handler.post(this);
                }
            }
        });
    }

    /**
     * Close the keyboard height provider,
     * this provider will not be used anymore.
     */
    public void close() {
        dismiss();
    }

    /**
     * Popup window itself is as big as the window of the Activity.
     * The keyboard can then be calculated by extracting the popup view bottom
     * from the activity window height.
     */
    private void handleOnGlobalLayout() {
        final int keyboardHeight = getInputMethodHeight(popupView.getContext(), edt.getRootView());
        listener.onChange(keyboardHeight);
    }

    public static int getInputMethodHeight(final Context context, final View rootView) {
        try {
            final InputMethodManager imm = (InputMethodManager) context.getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            final Class<?> inputMethodManagerClass = imm.getClass();
            final Method visibleHeightMethod = inputMethodManagerClass.getDeclaredMethod("getInputMethodWindowVisibleHeight");
            visibleHeightMethod.setAccessible(true);
            return (int) visibleHeightMethod.invoke(imm);
        } catch (Exception exception) {
            exception.printStackTrace();
        }

        return alternativeInputMethodHeight(rootView);
    }

    public static int getViewBottomInset(final View rootView) {
        try {
            final Field attachInfoField = View.class.getDeclaredField("mAttachInfo");
            attachInfoField.setAccessible(true);
            final Object attachInfo = attachInfoField.get(rootView);
            if (attachInfo != null) {
                final Field stableInsetsField = attachInfo.getClass().getDeclaredField("mStableInsets");
                stableInsetsField.setAccessible(true);
                return ((Rect) stableInsetsField.get(attachInfo)).bottom;
            }
        } catch (Exception ignore) {
        }
        return 0;
    }

    public static int alternativeInputMethodHeight(final View rootView) {
        int viewInset = getViewBottomInset(rootView);

        final Rect rect = new Rect();
        rootView.getWindowVisibleDisplayFrame(rect);

        final int availableHeight = rootView.getHeight() - viewInset - rect.top;
        return availableHeight - (rect.bottom - rect.top);
    }

}