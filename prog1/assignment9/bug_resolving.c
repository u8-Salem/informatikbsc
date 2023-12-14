/*
Compile: make bug_resolving
Run: ./bug_resolving
Compile & run: make bug_resolving && ./bug_resolving
*/

#include "base.h"

#define N 5

// struct of a spacecraft
typedef struct Spacecraft{
	char* name;
	int passengers;
	double reach;
	double load_capacity;	
}Spacecraft;

// struct of the fleet. 
typedef struct Fleet{
	Spacecraft** ships;
	int maximum_count; //initialize to N, size of ships
	int current_count; //current count of spacecrafts in ships
}Fleet;
	

// returns a pointer to a dynamically allocated ship
Spacecraft* new_spacecraft(int passengers, double reach, double load_capacity, char* name){
	Spacecraft *spc = xmalloc(sizeof(Spacecraft));
	spc->passengers = passengers;
	spc->reach = reach;
	spc->load_capacity = load_capacity;
	spc->name = s_copy(name);
	return spc;
}

// prints a spacecraft
void print_spacecraft(Spacecraft* spc){
	printf("%12s with %3d passengers, a reach of %8.1f au and a load capacity of %4.1f t\n", spc->name, spc->passengers, spc->reach, spc->load_capacity);
}

//s_sub implementiert
//Sorgt dafür, dass man die einzenlden Infos des Inputs/ der Liste 
//kategorisieren kann
char* s_sub(const char* str, int start, int end) {
    int length = end - start;
    char* substr = malloc(length + 1);
	//Kopiert nur die Sachen, die zwischen start und end stehen
    strncpy(substr, str + start, length);
    substr[length] = '\0';
    return substr;
}

// imports spacecrafts from file.
Fleet* read_spacecrafts(char* file_name){
	char* s = s_read_file(file_name);
	
	char* start = s;
	int ships = -1;
	while (*start != '\0'){
		if(*start == '\n'){
			ships++;
		}
		start++;
	}
	
	Fleet *fleet = xmalloc(sizeof(Fleet)); 
	fleet->current_count = ships;
	fleet->maximum_count = N;
	fleet->ships = xcalloc(N, sizeof(Spacecraft)); 
	
	while(*s != '\0'){
		Spacecraft *spc = NULL;

		//skip until line end
		while (*s != '\n'){
			s++;
		}
		s++;
		
		//Ende des Arrays
		if(*s == '\0'){
			break;
		}
		char* token[4];
		for(int i = 0; i < 4; i++){
			//lehrzeichen werden übersprungen
			while(*s == ' '){
				s++;
			}
			char* s2 = s;
			while (*s != ' ' && *s != '\n'){
				s++;
			}
		
			token[i] = s_sub(s2, 0, s - s2);
		}

		spc = new_spacecraft(i_of_s(token[1]), d_of_s(token[2]), d_of_s(token[3]), token[0]);
		//print_spacecraft(spc);

		fleet->ships[ships-1] = spc;
		ships--;	
	}
	return fleet;
}

// prints the fleet.
void print_spacecrafts(Fleet* f){
	for(int i = 0; i< f->maximum_count; i++){	
		if(f->ships[i]){
			printf("%2d\t", i);
			Spacecraft* spc = f->ships[i];
			printf("%12s with %3d passengers, a reach of %8.1f au and a load capacity of %4.1f t\n", spc->name, spc->passengers, spc->reach, spc->load_capacity);
			
		}
	}
}

// Entry point with command line arguments.
int main(int argc, String argv[]) {
	//base_init();
    //base_set_memory_check(true);
	report_memory_leaks(true);
	Fleet* fleet = NULL;

	if(argc > 1){
		fleet = read_spacecrafts(argv[1]);
	}else{
		fleet = read_spacecrafts("spacecrafts.txt");
	}

	char buffer[256]; 
	for(int i = 0; i< 256; i++){
		buffer[i] = '\0';
	}

	int c = '\0'; 
	printf("Your fleet: \n");
	print_spacecrafts(fleet);
	
	while((c = getchar()) != 'p'){
		if (c == 'q'){
			for(int i = 0; i < fleet->current_count; i++){
				free(fleet->ships[i]->name);
				free(fleet->ships[i]);
			}
			free(fleet->ships);
			free(fleet);
			exit(0);
		}
		if (c == 's'){
			//send out a ship, remove it from the fleet
			//example: s1
			//removes ship of index 1
			// Wird ein Schiff entfernt, bleibt eine Lücke im Array(Wenn nicht das letzte eintfernt wird).
			//Das nächste Schiff kann nicht richtig hinzugefügt werden
			//und die Maximalanzahl an Schiffen verringert sich
			int index = getchar() - '0';

			if(index >= 0 && index < fleet->current_count){
				free(fleet->ships[index]->name);
				free(fleet->ships[index]);
				for (int i = index; i < fleet->current_count - 1; i++) {
					fleet->ships[i] = fleet->ships[i + 1];

				}
				fleet->ships[fleet->current_count - 1] = NULL;
				fleet->current_count--;
			}
			//Nun wird das Schiff aus dem Array gelöscht, alle die da hinter 
			//standen rücken auf, und das letzte wird gelöscht
			//e) Ist kein Schiff an gegebenem Ort, passiert nichts. 
			//Auch das nächste kann problemlos hinzugefügt werden
		}else if (c == 'n'){
			//change name of ship
			//example: n1 Enterprise
			//renames ship at index 1 to Enterprise
			int index = getchar() - '0';
			getchar();
			int i = 0;
			while ((c = getchar()) != '\n'){
				buffer[i] = c;
				i++;
			}
			buffer[i] = '\0';
			fleet->ships[index]->name = s_copy(buffer);
			free(fleet->ships[index]->name);
		}else if (c == 'a'){
			//add ship
			//example: a Amagedon 34 500.3 2000.0
			getchar();
			int i = 0;
			//Schiffe werden zwar nicht sofort der Flotte hinzugefügt, sondern 
			//erst, wenn ein Platz frei ist (Dabei gelten auch vorher getätigte 
			//eingaben, ähnlich wie bei einer Warteliste)
			while ((c = getchar()) != '\n'){
				buffer[i] = c;
				i++;
			}
			buffer[i] = '\0';
			
			char* name = NULL;
			i = 0;
			for(; i< s_length(buffer); i++){
				if(buffer[i] == ' '){
					break;
				}
			}
			
			name = s_sub(buffer, 0, i);
			i++;
			int j = i;
			printf("Char: '%c'\n", buffer[j]);
			for(; j< s_length(buffer); j++){
				if(buffer[j] == ' '){
					break;
				}
			}
			printf("String: %s\n", s_sub(buffer, i, j));
			
			int passengers = i_of_s(s_sub(buffer, i, j));
			i = j++;
			for(; j< s_length(buffer); j++){
				if(buffer[j] == ' '){
					break;
				}
			}
			double reach = d_of_s(s_sub(buffer, i, j));
			i = j++;
			for(; j < s_length(buffer); j++){
				if(buffer[j] == ' '){
					break;
				}
			}
			
			double load_capacity =  d_of_s(s_sub(buffer, i, j));;
			
			Spacecraft* spc = new_spacecraft(passengers, reach, load_capacity, name);
			fleet->ships[fleet->current_count] = spc;
			fleet->current_count++;
			free(name);
		}
		if(c == '\n'){
			printf("Your fleet: \n");
			print_spacecrafts(fleet);
		}
		
	}
	for(int i = 0; i < fleet->current_count; i++){
		free(fleet->ships[i]->name);
		free(fleet->ships[i]);
	}
	free(fleet->ships);
	free(fleet);
    return 0;
}
