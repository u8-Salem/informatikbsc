package assignment1.a1;

class Encryption {
    public static int nextInt(int a, int b, int m, int x) {
        return (a * x + b) % m;
    }

    public static void printCharArray(char[] letters) {
        for (char c : letters) {
            System.out.print(c);
        }
        System.out.print("\n");
    }

    public static int[] encrypt(char[] letters) {
        int[] a = new int[letters.length];
        int x = nextInt(7, 28, 256, 3);
        a[0] = letters[0] ^ x;
        for (int i = 1; i < letters.length; i++) {
            x = nextInt(7, 28, 256, x);
            a[i] = letters[i] ^ x ^ a[i-1];
        }

        return a;
    }

    public static char[] decrypt(int[] nums) {
        char[] str = new char[nums.length];
        int x = nextInt(7, 28, 256, 3);
        str[0] = (char)(nums[0] ^ x);
        for (int i = 1; i < nums.length; i++) {
            x = nextInt(7, 28, 256, x);
            str[i] = (char)(nums[i] ^ x ^ nums[i-1]);
        }

        return str;
    }
}