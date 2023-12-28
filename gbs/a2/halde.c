#include "halde.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

/// Canary wrapper around the memory
struct canary {
	char mem[SIZE];
	int can;
};
struct canary canary = {.mem={0},.can = 0xdeadb33f};

/// Heap-memory area. Due to the conversion sizeof(memory) does not work, sizeof(canary.mem) works.
char *memory = (char *) canary.mem;

/// Pointer to the first element of the free-memory list.
static struct mblock *head;

/// Helper function to visualise the current state of the free-memory list.
void halde_print(void) {
	struct mblock* lauf = head;

	// Empty list
	if ( head == NULL ) {
		fprintf(stderr, "(empty)\n");
		return;
	}

	// Print each element in the list
    fprintf(stderr, "HEAD:  ");
	while ( lauf ) {
		fprintf(stderr, "(addr: 0x%08zx, off: %7zu, ", (uintptr_t) lauf, (uintptr_t)lauf - (uintptr_t)memory);
		fflush(stderr);
		fprintf(stderr, "size: %7zu)", lauf->size);
		fflush(stderr);

		if ( lauf->next != NULL ) {
			fprintf(stderr, "\n  -->  ");
			fflush(stderr);
		}
		lauf = lauf->next;
	}
	fprintf(stderr, "\n");
	fflush(stderr);
}

void *halde_malloc (size_t size) {
	if (size <= 0){
		return NULL;
	} 

	if (head == NULL && ((struct mblock*)memory)->next != MAGIC) {
		head = (struct mblock*) memory; 
		head->next = NULL;
		head->size = SIZE - sizeof(struct mblock);
	}

	if (head == NULL){
		errno = ENOMEM;
		return NULL;
	}

	struct mblock* pre = NULL;
	struct mblock* it = head;
	while (it->size < size){
		if (it->next == NULL){
			errno = ENOMEM;
			return NULL;
		}
		pre = it;
		it = it->next;
	}

	if (pre == NULL){
		head = it->next;
		pre = head;
	}else{
		pre->next = it->next;
	}

	it->next = MAGIC;
	size_t temp_size = it->size;

	if ((temp_size - size) <= sizeof(struct mblock)){
		return it->memory;
	}

	it->size = size;
	struct mblock* new = (struct mblock*)(it->memory + it->size);
	new->size = temp_size - size - sizeof(struct mblock);

	if (pre == NULL){
		new->next = NULL;
	}else{
		new->next = pre->next;
	}

	if (pre == NULL){
		head = new;
	}else{
		pre->next = new;
	}

	return it->memory;
}

void halde_merge(){
	struct mblock *pre = head;
	struct mblock *it = head->next;

	while (it != NULL) {
		if ((void*)(pre->memory + pre->size) == (void*)it){
			pre->next = it->next;
			pre->size = pre->size + sizeof(struct mblock) + it->size;
			it = pre->next;
		}else{
			pre = it;
			it = it->next;
		}
	}
}

void halde_free (void *ptr) {
	if (ptr == NULL){
		return;
	}

	struct mblock* to_free = (struct mblock*)(((char *)ptr) - sizeof(struct mblock));

	if (to_free->next != MAGIC){
		abort();
	}

	if (head == NULL){
		head = to_free;
		head->next = NULL;
		return;
	}

	if (to_free < head){
		to_free->next = head;
		head = to_free;
		halde_merge();
		return;
	}

	struct mblock *pre = NULL;
	struct mblock *it = head;
	
	while (to_free > it){
		if (it->next == NULL) break;
		pre = it;
		it = it->next;
	}

	if (to_free < it){
		to_free->next = it;
		if (pre == NULL){
			head = to_free;
		}else{
			pre->next = to_free;
		}
	}else{
		to_free->next = it->next;
		it->next = to_free;
	}

	halde_merge();
	return;
}

// PUNKTE: 12
