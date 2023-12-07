/*
	make postfix_repl && ./postfix_repl
*/

#include "base.h"

#define STACK_SIZE  10

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

typedef enum {
	OPERATION, VALUE, EMPTY
} Tag;

typedef struct{
	Tag tag;
	
	union{
		int value;
		char operation;
	};
} StackElement;

typedef struct{
	StackElement elements[STACK_SIZE];
	int stack_pointer;
} Stack;

//checks if char c belongs to an operation
bool is_operation(char c){
	return c == '+' || c == '/' || c == '-' || c == '*';
}

//checks wheter c is a digit
bool is_digit(char c){
	return c >= '0' && c <= '9';
}

//todo:
StackElement make_StackElement(Tag tag){
	StackElement ele;
	ele.tag = tag;

	return ele;
} 
//todo:
StackElement make_Value(int value){
	StackElement ele = {VALUE, {value = value}};
	return ele;
}
//todo:
StackElement make_Operation(char operation){
	StackElement ele = {OPERATION, {.operation = operation}};
	return ele;
}


bool is_empty(Stack* stack){
	return stack->stack_pointer < 0;
}

//todo:
void clear_stack(Stack* stack, int n){
	for (int i = 0; i < n; i++) {
		stack->elements[i].tag = EMPTY;
	}
	stack->stack_pointer = -1;
}

// pushes an element on top of the stack if stack isnt full. also increases stack counter.
void push(StackElement element, Stack* stack, int n){
	if(stack->stack_pointer < (n - 1)){
		stack->stack_pointer++;
		stack->elements[stack->stack_pointer] = element;
	}
}

// pops the top element from the stack (=EMPTY) and decreases stack counter. als returns that element
StackElement pop(Stack* stack, int n){
	StackElement ele = stack->elements[stack->stack_pointer ];
	stack->elements[stack->stack_pointer] = make_StackElement(EMPTY);
	stack->stack_pointer--;
	return ele;
}

//todo:
void print_stackElement(StackElement ele){
	switch(ele.tag) {
		case EMPTY:
			printf("[]");
			break;
		case VALUE:
			printf("[%d]", ele.value - 48);
			break;
		case OPERATION:
			printf("[%c]", ele.operation);
			break;
	}
}

//todo:
void print_stack(Stack* stack, int n){
	for (int i = n; i >= 0; i--) {
		print_stackElement(stack->elements[i]);
		if (i == n) { printf(" <--- stack pointer"); }
		if (i == 0)   { printf(" <--- lowest element"); }
		printf("\n");
	}
}

void compute(Stack* stack, int n){
	
	if (stack->elements[stack->stack_pointer].tag != OPERATION) {
		return;
	}

	if (stack->stack_pointer < 2
		|| stack->elements[stack->stack_pointer - 1].tag != VALUE
		|| stack->elements[stack->stack_pointer - 2].tag != VALUE)
	{
		printf("two operands required\n");
		pop(stack, stack->stack_pointer);
		return;
	}

	StackElement ret;
	ret.tag = VALUE;
	char operation = pop(stack, stack->stack_pointer).operation;
	int operand_2 =  pop(stack, stack->stack_pointer).value - 48;
	int operand_1 =  pop(stack, stack->stack_pointer).value - 48;

	switch (operation)
	{
	case '+':			
		ret.value = operand_1 + operand_2;
		break;
	case '-':			
		ret.value = operand_1 - operand_2;
		break;
	case '*':			
		ret.value = operand_1 * operand_2;
		break;
	case '/':			
		ret.value = operand_1 / operand_2;
		break;
	default:
		break;
	}

	ret.value += 48;
	
	push(ret, stack, STACK_SIZE);

}

int main (void){	
	Stack stack;
	
	clear_stack(&stack, STACK_SIZE);
	

	int input_size = 128;	
	char input[input_size];
	
	for(int i = 0; i< input_size; i++){
		input[i] = '\0';
	}
	int input_index = 0;
	
	
	int c;
	while((c = getchar()) != 'q'){

		if(c == '\n'){
			for(int i = 0; i< input_index; i++){
				char in = input[i];
				StackElement s;
				if 	    (is_empty(&stack)) {}

				if      (is_digit(in))     { s.tag = VALUE; s.value = (int)in; }
				else if (is_operation(in)) { s.tag = OPERATION; s.operation = in; }
				else    				   { continue; }

				push(s, &stack, STACK_SIZE);
				compute(&stack, stack.stack_pointer);
			}
			print_stack(&stack, stack.stack_pointer);

			for(int i = 0; i< input_size; i++){
				input[i] = '\0';
			}
			input_index = 0;
			
		}else{
			input[input_index] = c;
			input_index++;
		}

	
	}
	return 0;
}