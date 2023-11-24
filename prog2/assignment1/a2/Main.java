package assignment1.a2;

import java.util.Arrays;

public class Main {
    public static void main(String[] args) {
        int a = 5;
        int l = IntList.countNumbers(a);
        int[] arr = IntList.intToList(a);
        int b = IntList.listToInt(arr);
        int[] arr2 = IntList.addOne(arr);

        System.out.println(l);
        System.out.println(Arrays.toString(arr));
        System.out.println(b);
        System.out.println(Arrays.toString(arr2));
    }
}
