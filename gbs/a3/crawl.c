// TODO: includes, global vars, etc
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <dirent.h>
#include <libgen.h>
#include <fnmatch.h>

#include <fcntl.h>

#include "argumentParser.h"

bool first_Run;
char* original_path;
char temp_path[512];
int Context = 0;


// TODO: implement helper functions: makes constraint checking easier

bool size_Test(struct stat file_info, int size, int sizeMode){
	if (sizeMode == 0 && file_info.st_size == size){
		return true;
	}else if (sizeMode == 1 && file_info.st_size <= size){
		return true;
	}else if (sizeMode == 2 && file_info.st_size >= size){
		return true;
	}
	return false;
}

bool name_Test(struct stat file_info, const char pattern[], char* file_name){
	if (pattern != NULL){
		if (fnmatch(pattern, file_name, 0) == FNM_NOMATCH){
			return false;
		}else if (!S_ISDIR(file_info.st_mode)){
			return true;
		}
	}
	return true;
}

bool type_Test(struct stat file_info, char type){
	if (type == 'd' && S_ISDIR(file_info.st_mode)){
		return true;
	}else if (type == 'f' && S_ISREG(file_info.st_mode)){
		return true;
	}else if (type == '0'){
		return true;
	}
	return false;
}

void line_Test(const char* filePath, regex_t * line_regex){
	FILE *file;

    char line[Context + 1][1024];
	file = fopen(filePath, "r");
    if (file == 0)
    {	
		return;
    }

	int lineCount = 0;
	fpos_t temp_position;
	while ((fgets(line[Context], 1024, file)) != NULL){
		lineCount++;

		if (line[Context][strlen(line[Context]) - 1] == '\n'){
			line[Context][strlen(line[Context]) - 1] = '\0';
		}

        if (regexec(line_regex, line[Context], 0, NULL, 0) == 0){
			for(int i = 0; i < Context; i++ ){ //Zeilen vorher
				if(strlen(line[i]) != 0 && lineCount - (Context - i) > 0){
					printf("%s/%s:%d\"%s\n", temp_path, filePath, lineCount - (Context - i), line[i]);
					fflush(stdout);
				}
			}

			printf("%s/%s:%d\"%s\n", temp_path, filePath, lineCount, line[Context]); //Zeile mit Ausdruck
			fflush(stdout);

			char temp[1024];
			fgetpos(file, &temp_position);
			int temp_lineCount = lineCount;
			for(int i = 1; i <= Context; i++){ //Zeilen nachher
				if((fgets(temp, 1024, file)) == NULL){
					break;
				}

				if (temp[strlen(temp)] == '\n'){
					temp[strlen(temp) - 1] = '\0';
				}

				temp[strlen(temp) - 1] = '\0';
				lineCount++;
				printf("%s/%s:%d\"%s\n", temp_path, filePath, lineCount, temp);
				fflush(stdout);
			}
			lineCount = temp_lineCount;
			fsetpos(file, &temp_position);
		}
		for(int i = 0; (i + 1) <= Context; i++){ //Zeilen aufschieben
			strcpy(line[i], line[i + 1]);
			strcpy(line[i + 1], "");
		}
		
    }
	fclose(file);
}

void depthZero(char path[], bool isItAFile, const char pattern[], char type, int sizeMode, off_t size, regex_t * line_regex){

	char* file_name = basename(path);
	if (isItAFile){
		path = dirname(path);
	}
	int control = chdir(path);
	if (control == -1){
		return;
	}

	DIR * dir = opendir(".");
  	if(!dir){
		exit(1);
  	}

	char buffer[512];
    char *wdir = getcwd(buffer, 512);
	// KORREKTUR: Achtet immer darauf, dass alle Funktionen failen können! (-0.5P)

	struct stat info;

	if (isItAFile){
		control = lstat(file_name, &info);
		if (control == -1){
			exit(1);
		}

		if(file_name[0] == '.'){ //verstecktes Element wird ignoriert
			return;
		}

		if((strcmp(file_name, ".") == 0) || strcmp(file_name, "..") == 0){ //Dieses und letztes Verzeichnis ignorieren
        	return;
		}

		if (!(S_ISDIR(info.st_mode) || S_ISREG(info.st_mode))){ //Überprüft ob es sich um eine Zieldatei handelt
			return;
		}

		bool type_test = type_Test(info, type);
		if (!type_test){
			if(S_ISDIR(info.st_mode)){

			}else{
				return;
			}
		}

		bool size_test = size_Test(info, size, sizeMode);
		if (!size_test){
			if(S_ISDIR(info.st_mode)){

			}else{
				return;
			}
		}

		bool name_test = name_Test(info, pattern, file_name);
		if (!name_test){
			if(S_ISDIR(info.st_mode)){

			}else{
				return;
			}
		}

		if (size_test && name_test && type_test){
			if (S_ISREG(info.st_mode) && line_regex != NULL){
				line_Test(file_name, line_regex);
			}else if (line_regex == NULL){
				printf("%s/%s\n", path, file_name);
				fflush(stdout);
			}
		}
	}else{
		control = lstat(wdir, &info);
		if (control == -1){
			exit(1);
		}

		if (size_Test(info, size, sizeMode) && line_regex == NULL && pattern == NULL && type_Test(info, type)){
			printf("%s\n", temp_path);
			fflush(stdout);
		}
	}
}


static void crawl(char path[], int maxDepth, const char pattern[], char type,
                  int sizeMode, off_t size, regex_t * line_regex) {
	// TODO: implement body

	if (!first_Run){
		strcat(temp_path, "/");
		strcat(temp_path, path);
	}

	if (first_Run && maxDepth == 0){
		depthZero(path, false, pattern, type, sizeMode, size, line_regex);
		return;
	}else if (maxDepth == 0){
		return;
	}

	int control = chdir(path);
	if (control == -1){ //Es handelt sich um eine Datei
		int control2 = chdir(dirname(path));
		if (control2 == -1){
			return;
		}else{
			depthZero(path, true, pattern, type, sizeMode, size, line_regex);
		}
		return;
	}

	DIR * dir = opendir(".");
  	if(!dir){
		exit(1);
  	}

	char buffer[512];
    char *wdir = getcwd(buffer, 512);

	struct stat wdir_info;

	control = lstat(wdir, &wdir_info); //beziehe info zum working directory
	if (control == -1){
		exit(1);
	}

	if (first_Run && size_Test(wdir_info, size, sizeMode) && line_regex == NULL && pattern == NULL && type_Test(wdir_info, type)){ //stelle fest ob das Startverzeichnis ausgegeben werden soll
		printf("%s\n", temp_path);
		fflush(stdout);
		first_Run = false;
	}else{
		first_Run = false;
	}
	

  	struct dirent *cdir;
	while((cdir = readdir(dir))){
		struct stat file_info;

		if(cdir->d_name[0] == '.'){ //verstecktes Element wird ignoriert
			continue;
		}

		if((strcmp(cdir->d_name, ".") == 0) || strcmp(cdir->d_name, "..") == 0){ //Dieses und letztes Verzeichnis ignorieren
        	continue;
		}

		control = lstat(cdir->d_name, &file_info); //beziehe info zur Datei
		if (control == -1){
			exit(1);
		}

		if (!(S_ISDIR(file_info.st_mode) || S_ISREG(file_info.st_mode))){ //Überprüft ob es sich um eine Zieldatei handelt
			continue;
		}

		bool type_test = type_Test(file_info, type);
		if (!type_test){
			if(S_ISDIR(file_info.st_mode)){

			}else{
				continue;
			}
		}

		bool size_test = size_Test(file_info, size, sizeMode);
		if (!size_test){
			if(S_ISDIR(file_info.st_mode)){

			}else{
				continue;
			}
		}

		bool name_test = name_Test(file_info, pattern, cdir->d_name);
		if (!name_test){
			if(S_ISDIR(file_info.st_mode)){

			}else{
				continue;
			}
		}

		if (size_test && name_test && type_test){
			if (S_ISREG(file_info.st_mode) && line_regex != NULL){
				line_Test(cdir->d_name, line_regex);
			}else if (line_regex == NULL){
				printf("%s/%s\n", temp_path, cdir->d_name);
				fflush(stdout);
			}
		}

		char t_path[512];
		strcpy(t_path, temp_path);

		if (S_ISDIR(file_info.st_mode)) {
			if (maxDepth == -1){
				strcpy(t_path, temp_path);
				crawl(cdir->d_name, maxDepth, pattern, type, sizeMode, size, line_regex);
				strcpy(temp_path, t_path);
			}else{
				strcpy(t_path, temp_path);
				crawl(cdir->d_name, (maxDepth - 1), pattern, type, sizeMode, size, line_regex);
				strcpy(temp_path, t_path);
			}
			chdir(wdir);
		}
	}
	
	closedir(dir);
}




int main(int argc, char *argv[]) {
	// TODO: use argumentParser to retrieve arguments
	// TODO: invoke crawl() with args on all given pathes
	int control = initArgumentParser(argc, argv);
	if (control == -1) {
		printf("error");
		exit(0);
	}

	//max depth option
	int maxDepth = -1;
	if (getValueForOption("maxdepth") != NULL) {
		maxDepth =  atoi(getValueForOption("maxdepth"));
	}

	//type option
	char* type;
	type = getValueForOption("type");
	char t = '0';
	if (type == NULL){
		type = &t;
	}
  
	//name option
	char* name = getValueForOption("name");

	//size option
	char* s_size = getValueForOption("size");
	int size = 0;
	int sizeMode = 2;
	if (s_size != NULL){
		if (s_size[0] == '-') {
			sizeMode = 1;
		}else if (s_size[0] == '+'){
			sizeMode = 2;
		}else{
			sizeMode = 0;
		}
		size = abs(atoi(s_size));
	}

	//line option
	char* line = getValueForOption("line");
	regex_t *p_line_regex = NULL;
	regex_t line_regex;
	if (line != NULL){
		regcomp(&line_regex, line, REG_EXTENDED);
		// KORREKTUR: Auch `regcomp()` kann versagen. Dann macht es keinen Sinn weiterzumachen. (-0.5P)
		p_line_regex = &line_regex;
	}

	//context option
	char* s_Context = getValueForOption("context");
	// KORREKTUR: Nice! (1P)
	if(s_Context != NULL) {
		int context = atoi(s_Context);
		if (context > 0){
			Context = context;
		} 
	}

	int n_paths = getNumberOfArguments();
	for (int i = 0; i < n_paths; i++){
		original_path = getArgument(i);
		strcpy(temp_path, original_path);
		first_Run = true;
		crawl(original_path, maxDepth, name, *type, sizeMode, size, p_line_regex);
	}

	if(p_line_regex != NULL){
		regfree(p_line_regex);
	}
}

// PUNKTE: 8