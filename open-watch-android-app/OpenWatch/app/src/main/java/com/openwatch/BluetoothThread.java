package com.openwatch;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.openwatch.packet.MessageReceiver;
import com.openwatch.packet.PPGAlgorithms;
import com.openwatch.packet.PPGData;
import com.openwatch.packet.PacketUtil;
import com.openwatch.util.DispatchQueue;
import com.openwatch.util.Utils;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Iterator;
import java.util.UUID;

import static com.openwatch.packet.PacketUtil.HF;

public class BluetoothThread extends Thread {

    private static final int RX_BUFFER_SIZE = 811;
    private static final int RX_BUFFER_CAPACITY = RX_BUFFER_SIZE + 101;
    private static final long HEART_REFRESH_TIME = 800; //ms

    private final static Handler handler = new Handler(Looper.getMainLooper());

    private final Activity activity;
    private final MessageReceiver receiver;

    private DispatchQueue queue = null;
    private BluetoothSocket socket = null;
    private OutputStream out;

    private ArrayList<Byte> rxBuffer;
    private Field elementData;
    private boolean isFirstPacket, justRejected;

    private final Runnable runnable = new Runnable() {
        @Override
        public void run() {
            if (queue != null && out != null) {
                try {
                    out.write(PacketUtil.HEART_KEY);
                    queue.postRunnable(runnable, HEART_REFRESH_TIME);
                } catch (Exception ignore) {
                }
            }
        }
    };

    public BluetoothThread(Activity activity, MessageReceiver receiver) {
        this.activity = activity;
        this.receiver = receiver;
        rxBuffer = new ArrayList<>(RX_BUFFER_CAPACITY);
        try {
            elementData = rxBuffer.getClass().getDeclaredField("elementData");
            elementData.setAccessible(true);
        } catch (Exception ignore) {
            ignore.printStackTrace();
        }
    }

    @Override
    public void run() {
        BluetoothManager bluetoothManager = (BluetoothManager)
                activity.getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();
        isFirstPacket = true;
        justRejected = false;

        /* TODO: Create a list of paired devices */
        BluetoothDevice device = bluetoothAdapter.getRemoteDevice("98:D3:51:FD:B5:E4");
        InputStream in = null;
        out = null;

        int tryCount = 0;
        boolean tryPassed = false;

        while (!isInterrupted() && !tryPassed && tryCount < 5) {
            tryCount++;
            try {
                // https://arduino.stackexchange.com/a/31469
                socket = device.createInsecureRfcommSocketToServiceRecord(
                        UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
                socket.connect();
                in = socket.getInputStream();
                out = socket.getOutputStream();
                out.write(PacketUtil.CONNECTION_KEY);
                tryPassed = true;
            } catch (IOException e) {
                e.printStackTrace();
                System.out.println("NEXT TRY");

                try {
                    Thread.sleep(500);
                } catch (Exception ignore) {
                }
            }
        }

        if (out != null && socket.isConnected()) {
            handler.post(() -> receiver.receive(MessageReceiver.TYPE_WAITING, null));

            rxBuffer.clear();

            byte[] buffer = new byte[100];
            int numBytes;

            while (!isInterrupted() && socket.isConnected()) {
                try {
                    numBytes = in.read(buffer);
                    for (int i = 0; i < numBytes; i++)
                        rxBuffer.add(buffer[i]);

                    if (rxBuffer.size() >= RX_BUFFER_SIZE) {
                        rxParser();
                        rxCreator();
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    break;
                }
            }
        }

        forceClose();
    }

    private void rxParser() throws Exception {
        if (rxBuffer.get(0) != HF || rxBuffer.get(RX_BUFFER_SIZE - 1) != HF) {
            System.out.println("INVALID **************************");
            interrupt();
            return;
        }

        System.out.println(":) VALID **************************");

        if (isFirstPacket) {
            if (rxBuffer.get(4) == 1) {
                handler.post(() -> receiver.receive(MessageReceiver.TYPE_CONNECT, null));
                out.write(PacketUtil.generateNamePacket(Utils.getName(activity)));
                Thread.sleep(100);
                out.write(PacketUtil.generateTimePacket(Utils.getStepSize(activity)));
                Thread.sleep(100);
                out.write(PacketUtil.generateHealthPacket());

                queue = new DispatchQueue(() ->
                        queue.postRunnable(runnable, HEART_REFRESH_TIME));
            } else {
                handler.post(() -> receiver.receive(MessageReceiver.TYPE_REJECT, null));
                justRejected = true;
                interrupt();
            }
            isFirstPacket = false;
        } else {

            Object[] elements = (Object[]) elementData.get(rxBuffer);
            if (elements == null)
                return;

            PPGData out = PPGAlgorithms.test(elements, 800);
            handler.post(() -> receiver.receive(MessageReceiver.TYPE_DATA, out));
        }
    }

    private void rxCreator() {
        if (rxBuffer.size() > RX_BUFFER_SIZE) {
            try {
                Object[] elements = (Object[]) elementData.get(rxBuffer);
                if (elements != null) {
                    Object[] newElements = new Object[rxBuffer.size() - RX_BUFFER_SIZE];
                    System.arraycopy(elements, RX_BUFFER_SIZE, newElements, 0, newElements.length);
                    rxBuffer = new ArrayList<>(new RxCollection(newElements));
                    return;
                }
            } catch (Exception ignore) {
            }
        }
        rxBuffer.clear();
    }

    @Override
    public void interrupt() {
        super.interrupt();
        forceClose();
    }


    private void forceClose() {
        // INTERRUPT MAY CALL SEVERAL TIMES
        if (!justRejected)
            handler.post(() -> receiver.receive(MessageReceiver.TYPE_DISCONNECT, null));

        try {
            if (queue != null)
                queue.recycle();

            out = null;
            socket.close();
        } catch (IOException ignore) {
        }
    }

    private static class RxCollection implements Collection<Byte> {

        private final Object[] elementData;

        private RxCollection(Object[] elementData) {
            this.elementData = elementData;
        }

        @Override
        public int size() {
            return elementData.length;
        }

        @Override
        public boolean isEmpty() {
            return elementData.length == 0;
        }

        @Override
        public boolean contains(@Nullable Object o) {
            return false;
        }

        @NonNull
        @Override
        public Iterator<Byte> iterator() {
            return null;
        }

        @NonNull
        @Override
        public Object[] toArray() {
            return elementData;
        }

        @NonNull
        @Override
        public <T> T[] toArray(@NonNull T[] ts) {
            return null;
        }

        @Override
        public boolean add(Byte aByte) {
            return false;
        }

        @Override
        public boolean remove(@Nullable Object o) {
            return false;
        }

        @Override
        public boolean containsAll(@NonNull Collection<?> collection) {
            return false;
        }

        @Override
        public boolean addAll(@NonNull Collection<? extends Byte> collection) {
            return false;
        }

        @Override
        public boolean removeAll(@NonNull Collection<?> collection) {
            return false;
        }

        @Override
        public boolean retainAll(@NonNull Collection<?> collection) {
            return false;
        }

        @Override
        public void clear() {

        }
    }
}
