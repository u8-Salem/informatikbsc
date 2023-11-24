package assignment1.a1;

import java.util.Arrays;

import static assignment1.a1.Encryption.*;

class Main {
    public static void main(String[] args) {
        char[] str = {'H', 'A', 'L', 'L', 'O'};

        printCharArray(str);

        int[] enc = encrypt(str);

        System.out.println(Arrays.toString(enc));

        char[] dec = decrypt(enc);

        printCharArray(dec);
    }
}