#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "task.h"
#include "cpu.h"

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
double avgWaitTime(struct node *h, int numTasks, int waitingTime[], double avgWT){
    //First process doesn't wait, therefore
    waitingTime[0] = 0;
    struct node * jNode = h;
    for (int i = 1; i < numTasks; i++) {
        for (int j = numTasks - i; j > 0; j--) {
            jNode = jNode->next;
        }
        waitingTime[i] = waitingTime[i - 1] + (jNode->task->burst);
        jNode = h;
    }
    int sumWT = 0;
    for (int i = 0; i < numTasks; i++){
        sumWT = sumWT + waitingTime[i];
    }
    avgWT = (double)sumWT / (double)numTasks;
    return avgWT;
}
double avgTurnaroundTime(struct node *h, int numTasks, int waitingTime[], int turnTime[], double averageTT){
    struct node * jNode = h;
    for (int i = 0; i < numTasks; i++){
        for (int j = numTasks - i - 1 ; j > 0; j--) {
            jNode = jNode->next;
        }
        turnTime[i] = waitingTime[i] + (jNode->task->burst);
        jNode = h;
    }
    int sumTT = 0;
    for (int i = 0; i < numTasks; i++){
        sumTT = sumTT + turnTime[i];
    }
    averageTT = (double)sumTT / (double)numTasks;
    return averageTT;
}
//void totalResponseTime(){}
void schedule(){
    //Current node starting at the head
    struct node * currNode = phead;
    int waitingTime[taskCounter];
    int turnTime[taskCounter];
    double avgWT;
    double averageTT;
    //Before manipulating the list, the average times will be found
    avgWT = avgWaitTime(phead, taskCounter, waitingTime, avgWT);
    averageTT = avgTurnaroundTime(phead, taskCounter, waitingTime, turnTime, averageTT);
    //While there are still tasks left, perform the following
    while (taskCounter > 0) {
        //Since the tasks are added backwards, we need to get to the end to reach task 1
        while (currNode->next->next!= NULL) {
            currNode = currNode->next;
        }
        //Once the first task(s) is(/are) reached, run them and remove them from the list and adjust accordingly to another
        //cycle.
        run(currNode->task, currNode->task->burst);
        delete(&phead, currNode->task);
        taskCounter--;
        currNode = phead;
    }
    printf("\nAverage waiting time =  %.2f\n", avgWT);
    printf("Average turnaround time = %.2f\n", averageTT);
    printf("Average respones time = %.2f\n", avgWT);
}

