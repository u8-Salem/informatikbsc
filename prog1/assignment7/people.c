/*
Compile: make people
Run: ./people
*/

#include "base.h"

// Represents statistics of a people table.
struct Statistics {
	int mean_year;
	int number_males;
	int number_females;
	int number_diverse;
	double mean_height_males; // in m
	double mean_height_females; // in m
	double mean_height_diverse; // in m
};

typedef struct Statistics Statistics;


//Returns substring of s from character i to character j
String s_sub(String s, int i, int j) {
    require_not_null(s);
    int n = strlen(s);
    if (i < 0) i = 0;
    if (j > n) j = n;
    if (i >= j || i >= n || j <= 0) {
        char *a = xmalloc(1 * sizeof(char));
        a[0] = '\0';
        return a;
    }
    // assert i < j && i < n && j > 0
    n = j - i;
    char *a = xmalloc((n + 1) * sizeof(char));
    memcpy(a, s + i, n * sizeof(char));
    a[n] = '\0';
    return a;
}


Statistics make_statistics(void) {
	Statistics ps = { 0, 0, 0, 0, 0.0, 0.0, 0.0 };
	return ps;
}

void print_statistics(Statistics s) {
	printf("mean year: %d\n", s.mean_year);
	printf("number males: %d\n", s.number_males);
	printf("number females: %d\n", s.number_females);
	printf("number diverse: %d\n", s.number_diverse);
	printf("mean height males: %.2f\n", s.mean_height_males);
	printf("mean height females: %.2f\n", s.mean_height_females);
	printf("mean height diverse: %.2f\n", s.mean_height_diverse);

}

Statistics compute_statistics(String table) {
	Statistics ps = make_statistics();
	int n = s_length(table);
	int i = 0; //aktuelle Position im String

	while (s_get(table, i) != '\n') i++; // skip first row
	
	int temp = i;
	int lines = 0;
	while (++temp < n) {
		if (s_get(table, temp) == '\n') lines++;
	}


	for (int j = 0; j < lines; j++) {
		int first = j * 12 + 15;
		ps.mean_year += i_of_s(s_sub(table, first, first + 5));
		if (s_sub(table, first + 6, first + 7)[0] == 'f') {
			ps.number_females++;
			ps.mean_height_females += d_of_s(s_sub(table, first + 7, first + 12));
		} else if (s_sub(table, first + 6, first+7)[0] == 'm') {
			ps.number_males++;
			ps.mean_height_males += d_of_s(s_sub(table, first + 7, first + 12));
		} else {
			ps.number_diverse++;
			ps.mean_height_diverse += d_of_s(s_sub(table, first + 7, first + 12));
		}
	}

	ps.mean_year /= lines;
	ps.mean_height_males /= ps.number_males;
	ps.mean_height_females /= ps.number_females;
	ps.mean_height_diverse /= ps.number_diverse;

	return ps;
}


int main(void) {
	String table = s_read_file("people.txt");
	Statistics s = compute_statistics(table);
	print_statistics(s);
	
	/*
	Erwartete Ausgabe:
	
	mean year: 1974
	number males: 488
	number females: 501
	number diverse: 11
	mean height males: 1.79
	mean height females: 1.64
	mean height diverse: 1.69
	*/
	
	return 0;
}
