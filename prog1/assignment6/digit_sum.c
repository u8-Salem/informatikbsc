#include "base.h"

int digit_sum(unsigned number, bool alternating) {
    int ret = 0;
    bool negative = false; 
    
    while (number != 0) {
        ret += number % 10 * (negative ? -1 : 1); 
        number /= 10;
        if (alternating) { negative ^= 1; }
    }

    return ret;
}

bool divisible_by_eleven(unsigned number) {
    if (number < 11) { return false; }
    
    int _digit_sum = digit_sum(number, true);
    if (_digit_sum == 0 || divisible_by_eleven(_digit_sum)) {
        return true;
    } 
        
    return false;
}

void digit_sum_test() {
    
    test_equal_i(digit_sum(123, false), 6);
    test_equal_i(digit_sum(123, true), 2);
    test_equal_i(digit_sum(192, true), -6);
    test_equal_i(digit_sum(45678, false), 30); 
    test_equal_i(digit_sum(10, false), 1);
    test_equal_i(digit_sum(333, false), 9);

    // OPTIONAL TODO
    test_equal_i(divisible_by_eleven(12), false);
    test_equal_i(divisible_by_eleven(11), true);
}

int main(void) {
    digit_sum_test(); 
    printf("%d", digit_sum(123, true));
    return 0;
}
