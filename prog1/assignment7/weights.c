/*
Compile: make weights
Run: ./weights
*/

#include "base.h"

// 1 lb = 0.45359237 kg

typedef enum unit_e {
    G,
    KG,
    T,
    LB
} Unit;

typedef struct weight_s {
    double amount;
    Unit unit;
} Weight;

Weight make_weight(double amount, Unit unit) {
    // todo
	Weight w;
	w.amount = amount;
	w.unit = unit;
	return w;
}

bool test_within_weight(int line, Weight a, Weight e, double tolerance) {
    bool x = base_test_within_d(__FILE__, line, a.amount, e.amount, tolerance);
    bool u = base_test_equal_i(__FILE__, line, a.unit, e.unit);
    return x && u;
}

void print_weight(Weight w) {
    // todo
	switch (w.unit){
		case G:
			printf("%.2f g\n", w.amount);
			break;
		case KG:
			printf("%.2f kg\n", w.amount);
			break;
		case T:
			printf("%.2f t\n", w.amount);
			break;
		case LB:
			printf("%.2f lb\n", w.amount);
			break;
	}
}

void print_weight_test() {
    print_weight(make_weight(1234, G));
    print_weight(make_weight(4.749, KG));
    print_weight(make_weight(3.1001, T));
    print_weight(make_weight(5.40006, LB));
}

// Weight, Unit -> Weight
// Convert weight to the given target unit.
Weight to_unit(Weight w, Unit target_unit) {
    // todo
	switch(w.unit){
		case G:
			w.amount /= 1000.00;
			break;
		case KG:
			break;
		case T:
			w.amount *= 1000.00;
			break;
		case LB:
			w.amount *= 0.453592;
			break;
	}
	switch(target_unit){
		case G:
			w.amount *= 1000.00;
			break;
		case KG:
			break;
		case T:
			w.amount *= 1000;
			break;
		case LB:
			w.amount *= 2.20462;
			break;
			}
	Weight result;
	result.amount = w.amount;
	result.unit = target_unit;
	return result;
}

void to_unit_test(void) {
    test_within_weight(__LINE__, to_unit(make_weight(1000, G), KG), make_weight(1, KG), 1e-6);
    // todo: add tests (at least 5)
	test_within_weight(__LINE__, to_unit(make_weight(1, KG), G), make_weight(1000, G), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, T), KG), make_weight(1000, KG), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, T), G), make_weight(1000000, G), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, LB), KG), make_weight(0.453592, KG), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, LB), G), make_weight(453.592, G), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, KG), KG), make_weight(1, KG), 1e-6);
	test_within_weight(__LINE__, to_unit(make_weight(1, KG), LB), make_weight(2.20462, LB), 1e-6);
}

// Weight, Weight -> int
// Compares two weights. Returns 0 if the two weights are equal, 
// -1 if w is smaller than v and +1 otherwise.
int compare(Weight w, Weight v) {
    // todo
	w = to_unit(w, KG);
	v = to_unit(v, KG);
	if(v.amount == w.amount){
		return 0;
	} 
	if(v.amount > w.amount){
		return 1;
	}
	if(v.amount < w.amount){
		return -1;
	} else{
		return 2;
	}
	
}

void compare_test(void) {
    test_equal_i(compare(make_weight(1000, G), make_weight(1, KG)), 0);
    // todo: add tests (at least 5)
	test_equal_i(compare(make_weight(1, KG), make_weight(1000, G)), 0);
    test_equal_i(compare(make_weight(1, T), make_weight(1000, KG)), 0);
    test_equal_i(compare(make_weight(1, T), make_weight(1000000, G)), 0);
    test_equal_i(compare(make_weight(1, LB), make_weight(0.453592, KG)), 0);
    test_equal_i(compare(make_weight(1, LB), make_weight(453.592, G)), 0);
    test_equal_i(compare(make_weight(1, KG), make_weight(1, KG)), 0);
    test_equal_i(compare(make_weight(1, KG), make_weight(0.5, KG)), -1);
    test_equal_i(compare(make_weight(1, KG), make_weight(1.5, KG)), 1);
}

int main(void) {
    print_weight_test();
    to_unit_test();
    compare_test();
    return 0;
}
