#include "base.h"
int main (void){
	int input;
	int guess;
	printf ("Guess my Number! ");
	guess = i_rnd(101);
	input = i_input();
	while (input != guess){
		if (input > guess){
			printf ("Too Large! ");
		} else {
			printf ("To Low! ");
		} 	
		input = i_input();
	}
	printf ("Match!\n");
	return 0;
}