#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "cpu.h"
//Priority based process scheduling algorithm (Project 2)
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

Task *pickNextTask(){
    //Begin with the first task and initialize the node to be the next one for comparison
    struct node *curr = phead->next;
    //Unless changed, the current most "important" job is the first one we have
    Task *prioJob = phead->task;
    //Run across the entire linked list until the end is reached while performing the following check
    while (curr->next != NULL){
        //If the current task's priority is bigger than the currently recorded most important job's burst, then swap
        if ((curr->task->priority) >= (prioJob->priority)){
            prioJob = curr->task;
        }
        //Iterate to the next node
        curr = curr->next;
    }
    return prioJob;
}
double avgWaitTime(int waitTime[], int bursts[], int numTask) {
    //First task never waits, therefore waiting time is 0
    waitTime[0] = 0;
    //For the rest, to find the waiting time, the waiting time for the previous task needs to be added with the previous
    //burst
    for (int i = 1; i < numTask; i++){
        waitTime[i] = bursts[i -1] + waitTime[i - 1];
    }
    int sumWait = 0;
    //Add all the wait times together
    for (int i = 0; i < numTask; i++){
        sumWait = sumWait + waitTime[i];
    }
    //Find average
    double avgWT = (double)sumWait / (double)numTask;
    return avgWT;
}
double avgTurnTime(int waitTime[], int turnTime[], int bursts[], int numTask){
    //To find the turnaround time, simply add the wait time with the burst
    for(int i = 0; i < numTask; i++){
        turnTime[i] = waitTime[i] + bursts[i];
    }
    int sumTT = 0;
    //add all the turnaround times together
    for(int i = 0; i < numTask; i++){
        sumTT = sumTT + turnTime[i];
    }
    //Find average
    double avgTT = (double)sumTT / (double)numTask;
    return avgTT;
}
void schedule() {
    Task *nextJob;
    int burstTime[taskCounter];
    int waitingTime[taskCounter];
    int turnaroundTime[taskCounter];
    int count = 0;
    //As long as the list isn't empty, keep iterating until it is to display all the tasks
    while (phead->next != NULL) {
        nextJob = pickNextTask();
        run(nextJob, nextJob->burst);
        burstTime[count] = nextJob->burst;
        delete(&phead,nextJob);
        count++;
    }
    double averageWaitT = avgWaitTime(waitingTime, burstTime,taskCounter);
    double averageTurnT = avgTurnTime(waitingTime, turnaroundTime, burstTime, taskCounter);
    printf("\nAverage waiting time =  %.2f\n", averageWaitT);
    printf("Average turnaround time = %.2f\n", averageTurnT);
    printf("Average respones time = %.2f\n", averageWaitT);
}
