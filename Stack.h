#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

# define MAX_STACK_SIZE 128


//simple Stack implementation for int-s;
struct Stack{

    int top;
    int container [MAX_STACK_SIZE];
    size_t size;

};


typedef struct Stack Stack;

int full(Stack* st)
{
    return st->size == MAX_STACK_SIZE-1;
}
int empty(Stack* st)
{
    return st->size == 0;
}
int size(Stack* st)
{
    int size = st->size;
    return size;
}

void pop(Stack* st)
{
    if (!empty(st))
    {
        st->size--;
        st->top--;
    }
    else
    {
        perror("pop called on empty stack!\n");
        exit(1);
    }
}
int top(Stack* st)
{
    if (!empty(st))
    {
        int top = st->container[st->top];
        return top;
    }
    else
    {
        perror("top called on empty stack!\n");
        exit(1);
    }
}
void push(Stack* st, int data)
{
    if (!full(st))
    {
        st->container[st->top+1] = data;
        st->top++;
        st->size++;
    }
    else{
        perror("push called on full stack!\n");
        exit(1);
    }
}
void init(Stack* st)
{
    st->size = 0;
    st->top = -1;
}


#endif // STACK_H_INCLUDED

