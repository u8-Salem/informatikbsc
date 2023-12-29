#include <stdio.h>
#include <stdlib.h>

typedef struct list_element {
    struct list_element * next;
    unsigned int value;
} list_element_t;

typedef struct list {
    struct list_element * list;
} list_t;


int list_append(list_t *list, int value) {
    if (list == NULL) {
        fprintf(stderr, "List uninitialized");
        return EXIT_FAILURE;
    }

    list_element_t * ele = (list_element_t*)malloc(sizeof(list_element_t));
    if (ele == NULL) {
        fprintf(stderr, "Failed to allocated memory for list element. %s:%d in %s", 
                __FILE__,
                __LINE__,
                __func__);
        return EXIT_FAILURE;
    }

    ele->value = value;
    ele->next = NULL;
    
    if (list->list == NULL) {
        list->list = ele;
        return value;
    }

    list_element_t * l = list->list;

    while (l->next != NULL) {
        if (l->value == value) {
            free(ele);
            return -1;
        }
        l = l->next;
    }

    l->next = ele;

    return value;
}

int list_pop(list_t *list) {
    if (list == NULL || list->list == NULL) { 
        return -1;
    }

    list_element_t * l = list->list;

    if (l->next != NULL) {
        list->list = l->next;
    }

    unsigned int ret = l->value;
    
    free(l);

    return ret;
}

int main (int argc, char* argv[]) {
	list_t list; 
    list.list = NULL;

	printf("insert 47: %d\n", list_append(&list, 47));
	printf("insert 11: %d\n", list_append(&list, 11));
	printf("insert 23: %d\n", list_append(&list, 23));
	printf("insert 11: %d\n", list_append(&list, 11));
    
	printf("remove: %d\n", list_pop(&list));
	printf("remove: %d\n", list_pop(&list));
	exit(EXIT_SUCCESS);
}
