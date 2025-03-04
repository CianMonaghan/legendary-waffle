/*
 * dsh.c
 *
 *  Created on: Aug 2, 2013
 *      Author: chiu
 */
#include "dsh.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>

/**
 * Gets the command line from user
 */
char* getCmdline(){
    char *line = (char*) malloc(MAXBUF); // create an empty buffer to store the input
    printf("dsh> ");
    fgets(line, MAXBUF, stdin);
    return line;
}

/**
 * Determines whether or not the process is background or foreground and runs the process
 * 
 * Yes it's a bad name for a function i know
 */
void dshDetermine(char *args[]){
    pid_t pid = fork();
    if(pid != 0){//parent
        if(args[2] != "&"){
            wait(NULL);
        }
        dshCheck(getCmdline());
    } else{//child
        char *argsExec[2]; //execv path
        argsExec[0] = args[1];
        argsExec[1] = NULL;
        execv(args[0], argsExec);
    }
}
/**
 * Runs mode 2
 * it probably doesn't work
 */
int dshRunMode2(char *path, char *dir, char *args[]){
    if (access(path, F_OK | X_OK) == 0) {
        // File exists and is executable! Can run!
        dshDetermine(args);
        return 0;
    }
    else {
        char *localPath; //edited path var
        strcpy(localPath, PATH);
        char *splitPath[MAXBUF] = (char*)malloc(MAXBUF*sizeof(char));
        for(int i=0; i<MAXBUF; i++){
            splitPath[i]=strtok(localPath, ":");
            strcat(splitPath[i], dir);
            while(splitPath[i] != NULL){
                dshRunMode2(path, dir, args);
            }
            if(splitPath[i] == NULL){
                return 1; //error
            }
        }
        free(splitPath);
    }
}


/**
 * Determines if code is mode1 or mode2 or built in
 */
void dshProcess(char *args[]){
    char path[] = args[0];
    if(strstr(args[0], "/") != NULL){//mode 1
        //contains "/"
        if (access(path, F_OK | X_OK) == 0) {
            // File exists and is executable! Can run!
            dshDetermine(args);
        }
        else {
            // No good! File doesn't exist or is not executable!
            perror("ERROR: File error found\n");
            // Alert the user and re-prompt
            dshCheck(getCmdline());
        }
    }else if(args[0] == "exit"){//built in
        exit(0);
    }else if(args[0] == "pwd"){
        printf(getcwd(args[0], MAXBUF*sizeof(char)));
        dshCheck(getCmdline());
    }else if(args[0] == "cd"){
        if(args[1] != NULL){
            chdir(args[1]);
        }else{
            perror("ERROR: No such file or directory");
        }
        dshCheck(getCmdline());
    } else {//mode 2
        char *dir = getcwd(args[0], MAXBUF*sizeof(char));
        char *path = strcat(dir, args[0]);
        if(dshRunMode2(path, dir, args) != 0){
            perror("ERROR: Command not found.");
            dshCheck(getCmdline());
        }
    }
}

/**
 * checks if input is valid and attempts to run command
 */
void dshCheck(char *cmdline){
    char* line = cmdline;
    // reads up to 256 characters into the buffer
    if (line == NULL) {
        exit(0);  // exit the program if EOF is input
    }
    if(line >= MAXBUF){
        perror("ERROR: Command too long\n");
        dshCheck(getCmdline());
    }
    
    //split input into different commands
    char *args[3] = (char*)malloc(MAXBUF*sizeof(char));
    char *command = (char*)malloc(MAXBUF*sizeof(char));
    command = strtok(line, " ");
    for(int i=0; i<3; i++){
        args[i]= command;
    }
    dskProcess(args);
    free(args);
    free(command);
}