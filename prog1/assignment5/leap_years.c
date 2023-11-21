#include "base.h"
/*<Return the number of days in the given year.A leap year has 366 days, a
non-leap year has 365 days.The input represents any year A. D. as an integer, the
return value is the number of days.*/

int number_of_days(int year){
	// Leap Years are any year that can be exactly divided by 4
	int multiple_of_four = (year % 4 == 0);
	// except if it can be exactly divided by 100, then it isn't
	int multiple_of_hundred = (year % 100 == 0);
	// except if it can be exactly divided by 400, then it is
	int multiple_of_fourhundred = (year % 400 == 0);
	
	if((multiple_of_four && !multiple_of_hundred)  ||  multiple_of_fourhundred) {
		return 366;
	} else {
		return 365;
	}
}	

// Test cases
void number_of_days_test(){
	test_equal_i(number_of_days(400), 366);
	test_equal_i(number_of_days(100), 365);
	test_equal_i(number_of_days(4), 366);
	test_equal_i(number_of_days(1), 365);
	test_equal_i(number_of_days(2023), 365); 
}

int main(void){
	number_of_days_test();
	/*
	// Um Ergebnis in Konsole anzeigen zu lassen
	int year = i_input();
	printi (number_of_days(year));
	*/
	return 0;
}