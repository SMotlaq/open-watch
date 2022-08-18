package com.openwatch.packet;

public class WatchData {

    private final int battery;
    private final int stepCount;

    public WatchData(int battery, int stepCount) {
        this.battery = battery;
        this.stepCount = stepCount;
    }

    public int getBattery() {
        return battery;
    }

    public int getStepCount() {
        return stepCount;
    }
}
