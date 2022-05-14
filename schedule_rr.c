#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "cpu.h"
//Round Robin process scheduling algorithm
//Linked list initialization with task in each node
Task pheadTask ={0,0,0,0};
struct node head={&pheadTask,0};
struct node *phead = &head;
int taskCounter = 0;

void add(char *name, int priority, int burst){
    //Allocates memory to the new task to be added to the list
    Task *newTask = (Task *) malloc(sizeof(Task));
    //Takes the name, priority and burst from the parameter to put into the new task
    newTask->name = malloc(sizeof(name));
    strcpy(newTask->name,name);
    newTask->priority = priority;
    newTask->burst = burst;
    taskCounter++;
    //Using the insert function specified in list.c, adds the new task to head (from the front)
    insert(&phead, newTask);
}
struct node *reverse(struct node *src){
    struct node *curr = src;
    struct node destHead={&pheadTask,0};
    struct node *dest = &destHead;
    int destCounter = 0;
    while(taskCounter != destCounter){
        insert(&dest, curr->task);
        destCounter++;
        curr = curr->next;
    }
    return dest;
}
void schedule(){
    Task reverseHeadTask ={0,0,0,0};
    struct node reversehead={&reverseHeadTask,0};
    struct node *reversedList = &reversehead;
    int quantum = 10;
    int tasksDone = 0;
    //Variables related to finding times
    int numberOfProcesses = taskCounter;
    int waitTime = 0;
    int burstTotal = 0;
    int timeTotal = 0;
    int turnTime = 0;
    int processDuration = 0;
    reversedList = reverse(phead);
    struct node *iterate = reversedList;
    for (int i = 0; i < taskCounter; i++){
        burstTotal = burstTotal + iterate->task->burst;
        iterate = iterate->next;
    }
    while (taskCounter > 0){
        iterate = reversedList;
        for (int i = 0; i < taskCounter; i++) {
            if(iterate->task->burst > quantum){
                run(iterate->task, quantum);
                iterate->task->burst =iterate->task->burst - quantum;
                processDuration = processDuration + quantum;
            }
            else {
                processDuration = processDuration + iterate->task->burst;
                timeTotal = timeTotal +processDuration;
                run(iterate->task, iterate->task->burst);
                delete(&reversedList, iterate->task);
                tasksDone++;
            }
            iterate = iterate->next;
        }
        for (int i = 0; i < tasksDone; i++){
            taskCounter--;
        }
        tasksDone = 0;
    }
    waitTime = timeTotal - burstTotal;
    turnTime = waitTime + burstTotal;
    double averageWaitTime = (double)waitTime / (double)numberOfProcesses;
    double averageTurnTime = (double)turnTime / (double)numberOfProcesses;
    printf("\nAverage waiting time =  %.2f\n", averageWaitTime);
    printf("Average turnaround time = %.2f\n", averageTurnTime);
    printf("Average response time = \n");
}
