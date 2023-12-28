#include "argumentParser.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
// TODO: includes

// TODO: global vars
char** console_input; 
int num_args = -1; 

int num_paths = -1; 
int num_options = -1; 

// TODO: helpers

int contains(char* str, char c){

	while (*str) {
		if (*str == c) return 1; 
		str++; 
	}

	return 0; 
}

/**
 * Compares the first 'len' chars
*/
int strcmplen(char* s1, char* s2, int len) {
	for (int i = 0; i < len; i++) {
		if (s1[i] != s2[i]) return 0;
	}

	return 1; 
}


int checkArguments(int argc, char* argv[]) {

	short paths_finished = 0; 
	
	// make sure that there are no paths after any options, etc. 
	for (int i = 1; i < argc; i++) {
		
		// if we find an option we know that from now on no paths are allowed 
		if (argv[i][0] == '-') {

			// if the option doesnt contain a '=' sign, we know that this is a path and not an option
			if (contains(argv[i], '=')) {
				// remember that from now on only paths are allowed
				paths_finished = 1;
			} else {
				if (paths_finished) return -1; 
			}

		} else {

			// if a path comes after an option return -1
			if (paths_finished) return -1; 
		}
	}

	return 0; 
}

/**
 * extracts the number of paths
 * extracts the number of options
*/
void extractInfo(int argc, char* argv[]) {

	num_paths = 0; 
	num_options = 0; 

	// make sure that there are no paths after any options, etc. 
	for (int i = 1; i < argc; i++) {
		
		// if we find an option we know that from now on no paths are allowed 
		if (argv[i][0] == '-' && contains(argv[i], '=')) {
			num_options++;
		} else {
			num_paths++; 
		}
	}

}

int initArgumentParser(int argc, char* argv[]) { 

	// check if the contents of argv have the correct format
	int result = checkArguments(argc, argv); 

	if (result == -1) return -1; 

	console_input = argv; 
	num_args = argc; 

	extractInfo(argc, argv); 


	return 0;
}

char* getCommand(void) {
	return console_input[0];
}

int getNumberOfArguments(void) {
	return num_paths;
}

char* getArgument(int index) {

	if (index < 0 || index >= num_args) return NULL; 

	return console_input[index + 1]; 
}

char* getValueForOption(char* keyName) {

	int keyNameLen = strlen(keyName); 

	for (int i = 1; i < num_args; i++) {
		if (console_input[i][0] == '-') {
			
			// compare the two strings
			if (strcmplen(keyName, console_input[i]+1, keyNameLen) == 0) continue; 

			// if we found the option, get its value
			// find the '=' sign and get the value after that 
			return console_input[i] + keyNameLen + 2;  
		}
	}

	return NULL;

}

// PUNKTE: 4