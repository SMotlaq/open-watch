package com.openwatch.packet;

import java.util.Locale;

public class PPGData {

    private final double[] redData, irData;
    private final double HR, SpO2;
    private final int index;

    private int visible = 1;

    public PPGData(double[] redData, double[] irData, double hr, double spO2, int index) {
        this.redData = redData;
        this.irData = irData;
        this.HR = hr;
        this.SpO2 = spO2;
        this.index = index;
    }

    public int getIndex() {
        return index;
    }

    public double[] getRedData() {
        return redData;
    }

    public double[] getIrData() {
        return irData;
    }

    public int getVisible() {
        return visible;
    }

    public void setVisible(int visible) {
        this.visible = visible;
    }

    public double getHR() {
        return HR;
    }

    public double getSpO2() {
        return SpO2;
    }

    public byte getFormattedHR() {
        return (byte) HR;
    }

    public String getFormattedSpO2() {
        return getFormattedSpO2(getSpO2());
    }

    public String getReadableSpO2() {
        return getReadableSpO2(getSpO2());
    }

    public static String getFormattedSpO2(double SpO2) {
        return String.format(Locale.US, "%.2f", SpO2);
    }

    public static String getReadableSpO2(double SpO2) {
        String str = getFormattedSpO2(SpO2);
        if (str.endsWith(".00"))
            str = str.substring(0, str.length() - 3);
        else if (str.endsWith("0"))
            str = str.substring(0, str.length() - 1);

        return str;
    }

}
