package assignment1.a3;

/*
 * Debug.java:1: error: class, interface, enum, or record expected
 * ckass Debug {
 * ^
 *
 * misspelled class keyword
 */
class Debug {
    // Methode that checks whether a given number is prime
    public static boolean isPrime(int n) {
        // Return false if number is one, zero or negative
        /*
         * to satisfy condition use n < 2 to not exclude 2 from primes
         */
        if (n < 2) {
            return false;
        }
        // Check for all factors smaller and equal to n/2 whether it is a prime factor.
        // If it is a prime factor, return false.
        /*
         * to count upwards in a loop use i++ instead of i--
         * also return true and false statements are switched
         */
        for (int i = 2; i <= n / 2; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        // If no prime factor was found, return true.
        return true;
    }



    // Method that prints all numbers in an array that are prime
    /*
     * Exception in thread "main" java.lang.ArrayIndexOutOfBoundsException: Index 10 out of bounds for length 10
     *         at Debug.printPrime(Debug.java:30)
     *         at Debug.main(Debug.java:47)
     *
     *         since arrays are 0-indexed, accessing the element at n = arr.length results in out of bounds.
     *         make for loop break condition when i < arr.length
     */
    public static void printPrime(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            if (isPrime(arr[i])) {
                System.out.println(arr[i]);
            }
        }
    }

    public static void main(String[] args) {
        int[] arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        /*
         * Debug.java:37: error: invalid method declaration; return type required
         *     printPrime(arr);
         *     ^
         *
         *     function called outside
         *     another functions scope.
         *     need at least main function for function to execute.
         */
        printPrime(arr);

        // output should be as follows:
        // 2
        // 3
        // 5
        // 7
    }

}