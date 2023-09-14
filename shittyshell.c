#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// added this for pid_t 
#include <sys/types.h>

// create default path
#define DEFAULT_PATH "/bin"
#define MAX_PATH_LENGTH 1024 // set max path length 

// parse command for path

// build path print, path add, path remove, and path clear

void printPath(char *path) {
    printf("PATH: %s\n", path);
}

void addPath(char *path, char *addDir) {
    strcat(path, "/");
    strcat(path, addDir);
}

void removePath(char *path, char *removeDir) {

}

void clearPath(char *path) {
    path[0] = '\0';
}

int main(int argc, char **argv) {

    char *line, **argz;
    pid_t pid, wpid;
    size_t bufsize = 0;

    // path variables
    char path[MAX_PATH_LENGTH]; // max length array
    strcpy(path, DEFAULT_PATH); // copy default path into path

    while(1){

        // 1. Call printf to print a prompt. Something like $ or > is fine.
        printf(">> ");
        
        // line to null, call getline with address of line and bufsize, include stdin
        line = NULL; // initialize line as null BEFORE

        // read line of input but exit if end of file
        if (getline(&line, &bufsize, stdin) == -1) {
            printf("EOF Reached. Exit\n");
            exit(EXIT_SUCCESS);
        } 

        // trim text
        line[strlen(line) - 1] = '\0';

        // exit command
        if (strcmp(line, "exit") == 0) {
            printf("Exit Success\n");
            exit(EXIT_SUCCESS); // exit with 0 as the parameter
        }     

        // path commands 
        if (strcmp(line, "path print") == 0) {
            printPath(path);
            continue;
        } else if (strncmp(line, "path add", 9) == 0) { // skip 9 charachters to directory
            // add new directory onto line, increase size
            char *directory = line+9;
            addPath(path, directory);
            printf("Directory added to PATH: %s\n", directory);
            
        }  else if (strncmp(line, "path remove", 12) == 0) {// skip 12 charachters to  just directory
            continue;
        } else if (strcmp(line, "path clear") == 0) {
            clearPath(path);
            continue;
        }

        // compare first 3 characters to search for cd_
        if (strncmp(line, "cd ", 3) == 0) { // cd was foound
            char *dir = malloc(sizeof(char) * 100);
            strcpy(dir, line+3);
            // cd isnt found
            if (chdir(dir) != 0) {
                perror("nsh");
            } 
            free(dir);
            continue;
        } 

        // parse command string (tokenized string)
        char *parsedString = strtok(line, " \t"); // split by space and tab
        int count = 0; // start count
        argz = malloc(sizeof(char *) * (count + 1)); // allocate space for argz
        // while input exists
        while (parsedString != NULL) {
            argz[count] = strdup(parsedString); // copy and store the split input
            count++; // increase count  
            argz = realloc(argz, sizeof(char *) * (count + 1)); // resize and reallocate argz
            parsedString = strtok(NULL, " \t"); // make null
        }
        argz[count] = NULL;

        // call fork() which returns parent ID of the new child process(input) and setup child/parent process
        pid = fork(); 
        // child process (execute command from input)
        if (pid == 0) {
            // (call execv / error = perror nsh)
            if (execvp(argz[0], argz) == -1) {
                perror("nsh"); // print error
                exit(EXIT_FAILURE); // exit due to error
            }
        // parent, let command execute and loop
        } else { 
            // parent process waits for child to finish
            wpid = wait(NULL);
            // free parse count
            for (int k = 0; k < count; k++) {
                free(argz[k]);
            }
            // free argz and free line
            free(argz);
            free(line);
            }
        }
    return 0;
}