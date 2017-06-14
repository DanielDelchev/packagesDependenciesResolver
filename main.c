#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "Stack.h"

# define MAX_NODE_COUNT 128

char names [MAX_NODE_COUNT];
int black [MAX_NODE_COUNT];
int grey [MAX_NODE_COUNT];

struct node{
    char name; //assuming all packages have names consisting of a snigle char (for easier parsing) !!!!!!!!!!
    char neighboursNames [MAX_NODE_COUNT];
    int id;
    int neighboursID [MAX_NODE_COUNT];
    int neighboursCount;
};

struct node DAG [MAX_NODE_COUNT];
int nodesCount = 0;

void parseInput(size_t FD);
Stack DAG_DFS(char startingNodeName);
void printResult(Stack* result);

int main(int argc, const char* argv[])
{
    if (argc < 3){
        perror("Usage: ./dependency_resolver 'filename' 'PackageName (single char)' ");
        return 1;
    }

    int fileFD = open(argv[1],O_RDONLY);
    if (fileFD <= 0){
        perror("Could not open file!\n");
        return 1;
    }
    char startingNodeName = *argv[2];

    parseInput(fileFD);
    Stack result = DAG_DFS(startingNodeName);
    printResult(&result);
    return 0;
}

//parsing json input
void parseInput(size_t FD){
    int idAssigner = 0;

    char reader = '\0';

    while(read(FD,&reader,sizeof(char))>0){
        if (reader == '{')
            {
                DAG[idAssigner].neighboursCount = 0;
                int cnt = 0;
                while (cnt < 3){
                    char temp = '\0';
                    int check = read(FD,&temp,sizeof(char));
                    if (temp == '"' && check>0 ){cnt++;}
                }
                read(FD,&DAG[idAssigner].name,sizeof(char));
                names[idAssigner] = DAG[idAssigner].name;
                nodesCount++;

                while (reader!='[' && read(FD,&reader,sizeof(char))>0 ){}

                while (read(FD,&reader,sizeof(char))>0)
                {
                    if (reader == '}'){
                        break;
                    }
                    if (reader == '"'){
                        read(FD,&DAG[idAssigner].neighboursNames[DAG[idAssigner].neighboursCount++],sizeof(char));
                        lseek(FD,2,SEEK_CUR);
                    }
                }

                DAG[idAssigner].id = idAssigner;
                idAssigner++;
            }
    }


// assign number values
    for (int i=0;i<nodesCount;i++)//for each node
    {
        for (int j=0;j<DAG[i].neighboursCount;j++){ // for each of its neighbours
            for (int k=0;k<nodesCount;k++){ //search linearly
                if (DAG[k].name == DAG[i].neighboursNames[j])
                {
                    DAG[i].neighboursID[j] = DAG[k].id;
                }
            }
        }
    }

    if ( close(FD)!=0){
        perror("error closing file!\n");
    }

}

//prints the result in the manner shown in the example of the task
void printResult(Stack* result)
{

    char output [1024];
    int index = 0;
    output[index] = '[';
    int count = size(result);
    for (int i=0;i<count-1;i++)
    {
        output[++index] = '"';
        output[++index] = names[top(result)];
        output[++index] = '"';
        output[++index] = ',';
        output[++index] = ' ';
        pop(result);
    }
    output[++index] = '"';
    output[++index] = names[top(result)];
    output[++index] = '"';
    output[++index] = ']';
    output[++index] = '\0';
    pop(result);
    printf("%s",output);
}


// DFS on the DAG using 3 colours for marking
// white - all nodes are initially white, no such array is needed (if it is not grey or black it is white)
// grey - currently in stack
// black - already visited (has been in stack and has been popped)
// if we ever need to push a grey node in the stack
// the DAG(directed acyclic graph) actually has a cycle which is a problem
// that means there is a circular dependency in the packages which cannot be resolved


Stack DAG_DFS(char startingNodeName)
{
    memset(black,0,MAX_NODE_COUNT);
    memset(grey,0,MAX_NODE_COUNT);

    // finding out the ID of the starting node
    int start = -1;
    for (int i=0;i<nodesCount;i++)
    {
        if (names[i] != startingNodeName)
        {
            continue;
        }
        else
        {
            start = i;
        }
    }
    if (start == -1)
    {
        perror("No such package!\n");
        exit(-1);
    }

    Stack reversedResult;
    Stack traversingStack;
    init(&reversedResult);
    init(&traversingStack);
    push(&traversingStack,start);

    grey[start] = 1;

    while (!empty(&traversingStack))
    {
        int current = top(&traversingStack);
        int neighboursCount = DAG[current].neighboursCount;
        bool enstacked = false;

        for (int i=0;i<neighboursCount && ! enstacked ;i++)
        {
            int neighbour = DAG[current].neighboursID[i];
            if (grey[neighbour]==1)
            {
                perror("Circular dependency between packages found!\n");
                exit(1);
            }
            else if (black[neighbour]==1)
            {
                continue;
            }

            else if (grey[neighbour]==0 && black[neighbour]==0)
            {
                grey[neighbour] = 1;
                enstacked = true;
                push(&traversingStack,neighbour);
            }
        }

        if (!enstacked)
        {
            grey[top(&traversingStack)] = 0;
            black[top(&traversingStack)] = 1;
            push(&reversedResult,top(&traversingStack));
            pop(&traversingStack);
        }
    }

    // flipping the result so that it is correct
    // this could have been avoided if a queue
    // was used instead of a stack to store the result
    // but i didnt feel like implementing both stack and queue
    Stack result;
    init(&result);
    while (!empty(&reversedResult))
    {
        push(&result,top(&reversedResult));
        pop(&reversedResult);
    }

    return result;
}
