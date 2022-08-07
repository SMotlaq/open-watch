package com.openwatch.packet;

public class PPGData {

    public static final int SAMPLE_SIZE = 200;

    private final int[] redData, irData;
    private int visible = 1;

    public PPGData(int[] redData, int[] irData) {
        this.redData = redData;
        this.irData = irData;
    }

    public int[] getRedData() {
        return redData;
    }

    public int[] getIrData() {
        return irData;
    }

    public int getVisible() {
        return visible;
    }

    public void setVisible(int visible) {
        this.visible = visible;
    }
}
