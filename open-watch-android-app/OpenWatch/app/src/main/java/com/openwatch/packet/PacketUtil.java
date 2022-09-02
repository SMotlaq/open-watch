package com.openwatch.packet;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;


public class PacketUtil {

    public static final int TX_BUFFER_SIZE = 21;
    public static final byte HF = (byte) 0xA0;

    public static final byte[] CONNECTION_KEY = {
            (byte) 0xa0,
            (byte) 0xb6, (byte) 0x91, (byte) 0xdf, (byte) 0x92,
            (byte) 0x9a, (byte) 0x92, (byte) 0x90, (byte) 0x8d,
            (byte) 0x86, (byte) 0xdf, (byte) 0x90, (byte) 0x99,
            (byte) 0xdf, (byte) 0x92, (byte) 0x86, (byte) 0xdf,
            (byte) 0x9e, (byte) 0x88, (byte) 0x8d,
            (byte) 0xa0
    };

    public static final byte[] HEART_KEY = {
            (byte) 0xf5,
            (byte) 0x96, (byte) 0x87, (byte) 0x9e, (byte) 0xd1,
            (byte) 0xab, (byte) 0x97, (byte) 0x9a, (byte) 0xdf,
            (byte) 0x93, (byte) 0x90, (byte) 0x8c, (byte) 0x8b,
            (byte) 0xdf, (byte) 0x97, (byte) 0x9a, (byte) 0x9e,
            (byte) 0x8d, (byte) 0x8b, (byte) 0xd1,
            (byte) 0xf5
    };

    public static byte[] generateTimePacket(int stepSize) {
        byte[] b = new byte[TX_BUFFER_SIZE];
        b[0] = b[TX_BUFFER_SIZE - 1] = (byte) 0xC2;

        Calendar c = Calendar.getInstance();

        b[1] = (byte) (c.get(Calendar.YEAR) % 100);
        b[2] = (byte) c.get(Calendar.MONTH);
        b[3] = (byte) c.get(Calendar.DAY_OF_MONTH);
        b[4] = (byte) c.get(Calendar.HOUR_OF_DAY);
        b[5] = (byte) c.get(Calendar.MINUTE);
        b[6] = (byte) c.get(Calendar.SECOND);

        b[9] = (byte) stepSize;
        return b;
    }

    public static byte[] generateNamePacket(String name) {
        ArrayList<Byte> ascii = new ArrayList<>(19);

        byte[] acceptedCharacters = {
                (byte) ' ', (byte) '%', (byte) '(', (byte) ')',
                (byte) ',', (byte) '-', (byte) '.', (byte) '/',
                (byte) ':', (byte) '@', (byte) '|'
        };

        int insertedLen = 0;

        for (char c : name.toCharArray()) {
            if ((c >= 'a' && c <= 'z')
                    || (c >= 'A' && c <= 'Z')
                    || (c >= '0' && c <= '9')
                    || (Arrays.binarySearch(acceptedCharacters, (byte) c) >= 0)) {
                insertedLen++;
                if (insertedLen == 19) {
                    ascii.remove(ascii.size() - 1);
                    ascii.remove(ascii.size() - 1);
                    ascii.remove(ascii.size() - 1);
                    ascii.add((byte) '.');
                    ascii.add((byte) '.');
                    ascii.add((byte) '.');
                    break;
                }
                ascii.add((byte) c);
            }
        }

        if (insertedLen == 0) {
            ascii.add((byte) 'E');
            ascii.add((byte) 'v');
            ascii.add((byte) 'e');
            ascii.add((byte) 'r');
            ascii.add((byte) 'y');
            ascii.add((byte) 'o');
            ascii.add((byte) 'n');
            ascii.add((byte) 'e');
        }

        byte[] b = new byte[TX_BUFFER_SIZE];
        b[0] = b[TX_BUFFER_SIZE - 1] = (byte) 0xB1;
        int len;
        for (len = 0; len <= 18; len++) {
            if (ascii.size() <= len)
                break;
            b[len + 1] = ascii.get(len);
        }
        b[TX_BUFFER_SIZE - 2] = (byte) Math.floor((18 - len) * 7f / 2f);
        return b;
    }

    public static byte[] generateHealthPacket(PPGData data) {
        byte[] b = new byte[TX_BUFFER_SIZE];
        b[0] = b[TX_BUFFER_SIZE - 1] = (byte) 0xE4;
        String spO2 = data.getFormattedSpO2();
        int index = spO2.indexOf(".");
        if (index > 0) {
            String a = spO2.substring(0, index);
            b[1] = (byte) Integer.parseInt(a);
            b[2] = (byte) Integer.parseInt(spO2.substring(index + 1, index + 2));
        } else {
            b[1] = 0; // SpO2 integer
            b[2] = 0; // SpO2 fraction 1digit
        }
        b[3] = data.getFormattedHR(); // HR
        return b;
    }

    public static byte[] generateAlarmPacket(int[] res) {
        byte[] b = new byte[TX_BUFFER_SIZE];
        b[0] = b[TX_BUFFER_SIZE - 1] = (byte) 0xD3;

        if (res == null) {
            b[1] = (byte) 250;
            return b;
        }

        b[4] = (byte) res[0];
        b[5] = (byte) res[1];
        b[7] = (byte) res[2];
        b[8] = (byte) (res[2] == 0 ? 1 : 0);
        return b;
    }
}
