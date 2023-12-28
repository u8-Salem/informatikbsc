#include <stdlib.h>
#include "plist.h"


void walkList(list *list, int (*callback) (pid_t, const char *) ) {
	// TODO: implement me, Subtask d
	if (!list) return; 

	list_element *elem = list->head; 

	while (elem && !callback(elem->pid, elem->cmdLine)) {
		elem = elem->next; 
	}
}