/**
 * Simple shell interface starter kit program.
 * Operating System Concepts
 * Project 1 - Creating a 'Custom' shell with new commands
 */
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LINE      80 /* 80 chars per line, per command */

int main(void)
{
    char *args[MAX_LINE/2 + 1];    /* command line (of 80) has max of 40 arguments */
    int should_run = 1;
    char input[MAX_LINE];
    int counter = 0;
    char * x;
    char *chkstr;
    char * index;
    int fileFlag = 0;
    int parentWaitFlag = 0;
    int backupCount = 0;

    //Pipe Variables
    int p[2];
    int forkPID;

    while (should_run){
        printf("mysh:~$ ");
        fflush(stdout);

        /**
          * After reading user input, the steps are:
          * (1) fork a child process
          * (2) the child process will invoke execvp()
          * (3) if command includes &, parent and child will run concurrently
          */
        index = fgets(input, MAX_LINE, stdin);
        chkstr = input;
        index = chkstr;
        //Cheks if user inputted nothing and then restarts
        if (strcmp(input, "\n") == 0) {
            continue;
        }
        //Checks if user wishes to exit shell and changes loop state
        if (strcmp(input, "exit\n") == 0) {
            should_run = 0;
        }
        else {
            //Checks if user did not put !!
            if (strcmp(input, "!!\n") > 0){
                if (counter > 0) {
                    for (int i = 0; i < counter; i++){
                        //Frees space
                        free(args[i]);
                    }
                }
                counter = 0;
                //While loop responsible for parsing input
                while (1) {
                    //Index to be used in parsing in between words
                    x = strchr(chkstr, ' ');
                    //Responsible for the very last argument assignment
                    if (x == NULL) {
                        if (index < input + strlen(input)) {
                            int lastLength = strchr(chkstr, '\n') - index;
                            args[counter] = malloc(lastLength+1);
                            memcpy(args[counter], index, lastLength);
                            args[counter][lastLength] = '\0';
                            counter = counter + 1;
                            args[counter] = 0;
                            counter = counter + 1;
                            //Checks whether an & is included and if to signal the wait later on
                            if (args[counter - 2][0] != '&') {
                                parentWaitFlag = 1;
                            } else {
                                parentWaitFlag = 0;
                                args[counter - 2] = 0;
                                counter--;
                            }
                            backupCount = counter;
                            break;
                        }
                    }
                    //Allocates memory and assigns the arguments appropriately
                    args[counter] = malloc(x - index + 1);
                    memcpy(args[counter], index, x - index);
                    args[counter][x - index] = '\0';
                    index = x + 1;
                    chkstr = index;
                    counter = counter + 1;
                }
            }

            //Fork a child process, process ID is kept in a variable for potential use later
            int pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork Failed");
                return 1;
            }

                //Child process
            else if (pid == 0) {
                //If user wants to repeat but no command is in the history, display error
                if (strcmp(input, "!!\n") == 0 && backupCount == 0) {
                    printf("No commands in history.\n");
                }
                //Else just perform argument with given arguments
                else {
                    for (int i = 0; i < backupCount - 1; i++) {
                        //Checks for the > symbol
                        if (args[i][0] == '>') {
                            //Creates a reference to a file with the ability to create, write and append at the end
                            int fpw = open(args[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0666);
                            if (fpw > 0) {
                                char **secArgs = malloc((i + 1) * sizeof(char *));
                                //Assigns the arguments to a new reference up to but not including >
                                for (int j = 0; j < i; j++) {
                                    secArgs[j] = args[j + 1];
                                }
                                //Sets the last argument for the reference to null
                                secArgs[i] = 0;
                                //Executes the left side of the > and stores it into the new file
                                dup2(fpw, 1);
                                close(fpw);
                                execvp(args[0], secArgs);
                                free(secArgs);
                                fileFlag = 1;
                            } else {
                                //Error message
                                fprintf(stderr, "Error writing to file");
                            }
                        }
                        //Checks for the < symbol
                        if (args[i][0] == '<') {
                            //Creates a reference to a file that can only be read
                            int fpr = open(args[i + 1], O_RDONLY);
                            if (fpr > 0) {
                                char **secArgs = malloc((i + 1) * sizeof(char *));
                                //Assigns the arguments to a new reference up to but not including <
                                for (int j = 0; j < i; j++) {
                                    secArgs[j] = args[j + 1];
                                }
                                secArgs[i] = 0;
                                //Executes the left side of the < with the right side as the input
                                dup2(fpr, 0);
                                close(fpr);
                                execvp(args[0], secArgs);
                                free(secArgs);
                                fileFlag = 1;
                            } else {
                                //Error message
                                fprintf(stderr, "Error reading from file");
                            }
                        }
                        //Checks for the pipe symbol
                        if (args[i][0] == '|'){
                            //Assigns the left part of the pipe into a reference
                            char **leftArgs = malloc((i + 1) * sizeof(char *));
                            for (int j = 0; j < i; j++) {
                                leftArgs[j] = args[j];
                            }
                            leftArgs[i] = 0;
                            //Assigns the right part of the pipe into a reference
                            char **rightArgs = malloc((backupCount - i) * sizeof(char *));
                            for (int j = 0; j < backupCount - i - 1; j++) {
                                rightArgs[j] = args[i + j + 1];
                            }
                            rightArgs[backupCount - i - 1] = 0;
                            //Write to pipe
                            pipe(p);
                            forkPID = fork();
                            //Another fork for reading one half of the pipe and feeding to the other
                            if (forkPID == 0){ //Child process
                                //Write to the pipe, using output from left part to the right part as input
                                dup2(p[0], 0);
                                close(p[1]);
                                execvp(rightArgs[0], rightArgs);
                            }
                            else { //Parent process
                                //Read to pipe for the left part of the pipe
                                dup2(p[1], 1);
                                close(p[0]);
                                execvp(args[0], leftArgs);
                            }
                            free(rightArgs);
                            free(leftArgs);
                            fileFlag = 1;

                        }
                    }
                    if (fileFlag == 0) {
                        execvp(args[0], args);
                    }
                    fileFlag = 0;
                }
                return 0;
            }
                //Parent process
            else {
                if (parentWaitFlag == 1) {
                    wait(NULL);
                }
            }
        }
    }
    return 0;
}
