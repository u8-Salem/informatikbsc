package assignment1.a2;

import java.lang.Math;
class IntList {
    public static int countNumbers(int a) {
        return String.valueOf(a).length();
    }

    public static int[] intToList(int value) {
        int l = countNumbers(value);
        int[] arr = new int[l];

        if (value == 0) { return new int[]{0}; }

        while (value != 0) {
            arr[--l] = value % 10;
            value /= 10;
        }

        return arr;
    }

    public static int listToInt(int[] numbers) {
        int ret = 0;
        int power_of_ten = (int)Math.pow(10, numbers.length - 1);
        for (int a : numbers) {
            ret += a * power_of_ten;
            power_of_ten /= 10;
        }

        return ret;
    }

    public static int[] addOne(final int[] numbers) {
        int[] n = numbers.clone();
        int l = n.length;

        if (n[l-1] < 9 ) {
            n[l-1] += 1;
            return n;
        }

        boolean expand = true;
        for (int a : n) {
            if (a != 9) {
                expand = false;
                break;
            }
        }

        if (expand) {
            int[] arr = new int[l+1];
            arr[0] = 1;
            return arr;
        }

        for (int i = l-1; i >= 0; i--) {
            if (n[i] == 9) {
                n[i] = 0;
                continue;
            }
            n[i] += 1;
            break;
        }

        return n;
    }
}