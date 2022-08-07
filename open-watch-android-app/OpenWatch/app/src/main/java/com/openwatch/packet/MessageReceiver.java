package com.openwatch.packet;

public interface MessageReceiver {
    int TYPE_WAITING = 0;
    int TYPE_CONNECT = 1;
    int TYPE_REJECT = 2;
    int TYPE_DISCONNECT = 3;
    int TYPE_DATA = 4;

    void receive(int type, Object data);
}
