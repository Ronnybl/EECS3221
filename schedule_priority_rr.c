#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "cpu.h"
//Priority-based Round Robin process scheduling algorithm (Project 2)
//Linked list initialization with task in each node
Task pheadTask ={0,0,0,0};
struct node head={&pheadTask,0};
struct node *phead = &head;
int taskCounter = 0;
int prioCounter = 0;

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
struct node *pickNextTask(){
    //Begin with the first node
    struct node *curr = phead;
    struct node nextPrioHead = {&pheadTask,0};
    struct node *nextPrio = &nextPrioHead;
    int largestPrio = 0;
    //Run across the entire linked list until the end is reached while performing the following check
    while (curr->next != NULL){
        //If the current task's priority is bigger than the currently recorded most important job's burst, then swap
        if ((curr->task->priority) > largestPrio){
            largestPrio = curr->task->priority;
        }
        //Iterate to the next node
        curr = curr->next;
    }
    curr = phead;
    while (curr->next != NULL){
        //If the current task's priority is bigger than the currently recorded most important job's burst, then swap
        if ((curr->task->priority) == largestPrio){
            insert(&nextPrio, curr->task);
            prioCounter++;
        }
        //Iterate to the next node
        curr = curr->next;
    }
    largestPrio = 0;
    return nextPrio;
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
    struct node *iterate = phead;
    int tasksDone = 0;
    int quantum = 10;
    int numberOfProcesses = taskCounter;
    int waitTime = 0;
    int burstTotal = 0;
    int timeTotal = 0;
    int turnTime = 0;
    int processDuration = 0;

    iterate = phead;
    for (int i = 0; i < taskCounter; i++){
        burstTotal = burstTotal + iterate->task->burst;
        iterate = iterate->next;
    }
    while (taskCounter > 0){
        //printf("Before picknextTask\n");
        iterate = pickNextTask();
        //if (prioCounter > 1){}
        for (int i = 0; i < prioCounter; i++) {
            //printf("Begin for loop\n");
            if(iterate->task->burst > quantum && prioCounter == 1){
                run(iterate->task, iterate->task->burst);
                processDuration = processDuration + iterate->task->burst;
                delete(&phead, iterate->task);
                tasksDone++;
            }
            else if(iterate->task->burst > quantum){
                run(iterate->task, quantum);
                iterate->task->burst =iterate->task->burst - quantum;
                processDuration = processDuration + quantum;
            }
            else {
                processDuration = processDuration + iterate->task->burst;
                timeTotal = timeTotal +processDuration;
                run(iterate->task, iterate->task->burst);
                delete(&phead, iterate->task);
                tasksDone++;
            }
            iterate = iterate->next;
        }
        for (int i = 0; i < tasksDone; i++){
            taskCounter--;
        }
        tasksDone = 0;
        prioCounter = 0;
    }
    waitTime = timeTotal - burstTotal;
    turnTime = waitTime + burstTotal;
    double averageWaitTime = (double)waitTime / (double)numberOfProcesses;
    double averageTurnTime = (double)turnTime / (double)numberOfProcesses;
    printf("\nAverage waiting time =  %.2f\n", averageWaitTime);
    printf("Average turnaround time = %.2f\n", averageTurnTime);
    printf("Average response time = \n");
}
