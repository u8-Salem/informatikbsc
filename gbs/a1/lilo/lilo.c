#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct list_element {
	//TODO: implement fields
	int value;
	struct list_element *next;
} list_element_t;

typedef struct list {
	// TODO: implement fields
	list_element_t *first;
} list_t;

// ANMERKUNG: Ihr dürft die TODOs gerne entfernen, ansonsten schöner Code :)

int list_append_rec(list_element_t *list, int value){
	if (list->value == value){
		return -1;
	}else if (list->next == NULL){
		list_element_t *new = (list_element_t *) malloc(sizeof(list_element_t));
		if (new == NULL){
			return -1;
		}
		new->value = value;
		new->next = NULL;
		list->next = new;
		return value;
	}else{
		return list_append_rec(list->next, value);
	}
	return -1;
}

int list_append(list_t *list, int value) {
	// TODO: implement body
	if (value < 0){
		return -1;
	}
	if (list->first == NULL){
		list_element_t *new = (list_element_t *) malloc(sizeof(list_element_t));
		if (new == NULL){
			return -1;
		}
		new->value = value;
		new->next = NULL;
		list->first = new;
		return value;
	}
	int ret = list_append_rec(list->first, value);
	
	return ret;
}

int list_pop(list_t *list) {
	// TODO: implement body
	if (list->first == NULL){
		return -1;
	}
	int ret = list->first->value;
	list_element_t *temp = list->first->next;
	free(list->first);
	list->first = temp;

	return ret;
}

int main (int argc, char* argv[]) {
	list_t list; //TODO: initialize

	printf("insert 47: %d\n", list_append(&list, 47));
	printf("insert 11: %d\n", list_append(&list, 11));
	printf("insert 23: %d\n", list_append(&list, 23));
	printf("insert 11: %d\n", list_append(&list, 11));

	printf("remove: %d\n", list_pop(&list));
	printf("remove: %d\n", list_pop(&list));
	exit(EXIT_SUCCESS);
}

// PUNKTE: 4