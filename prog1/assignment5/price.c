/*
 * Compile:         make X
 * Run:             ./X
 * Compile and Run: make X && ./X
 */
#include "base.h" // prog1lib

// todo: define constants
const int first = 10;
const int second = 50;
const int price_base = 100;
const int price_first = 90;
const int price_second = 75;
const int cap = 2000;
const int fee = 500;

// returns the total price of the items depending on the amount ordered.
int price(int amount)
{
	int p = price_base;
	int cost;

	if (amount <= 0) 		{ return 0; }
	if (amount >= first) 	{ p = price_first; }
	if (amount >= second) 	{ p = price_second; }

	cost = p * amount;
	if (cost < cap) { cost += fee; }

	return cost;
}
// todo: write function price_test
// test returned values of price using test_equal_i
void price_test()
{
	test_equal_i(price(0), 0);
	test_equal_i(price(5), 1000);
	test_equal_i(price(10), 1400);
	test_equal_i(price(22), 2480);
	test_equal_i(price(50), 3750);
	test_equal_i(price(100), 7500);
}

int main(void) {
    // todo: call price_test function

    price_test();
}
