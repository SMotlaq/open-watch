package com.openwatch.packet;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;

public class PacketUtil {

    public static final int TX_BUFFER_SIZE = 21;
    public static byte HF = (byte) 0xA0;

    public static byte[] CONNECTION_KEY = {
            (byte) 0xa0,
            (byte) 0xb6, (byte) 0x91, (byte) 0xdf, (byte) 0x92,
            (byte) 0x9a, (byte) 0x92, (byte) 0x90, (byte) 0x8d,
            (byte) 0x86, (byte) 0xdf, (byte) 0x90, (byte) 0x99,
            (byte) 0xdf, (byte) 0x92, (byte) 0x86, (byte) 0xdf,
            (byte) 0x9e, (byte) 0x88, (byte) 0x8d,
            (byte) 0xa0
    };

    public static byte[] HEART_KEY = {
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

    public static byte[] generateHealthPacket() {
        byte[] b = new byte[TX_BUFFER_SIZE];
        b[0] = b[TX_BUFFER_SIZE - 1] = (byte) 0xE4;
        b[1] = 98; // SpO2 integer
        b[2] = 2; // SpO2 fraction 1digit
        b[3] = 72; // HR
        return b;
    }
}
