//
// Created by seven on 16-7-23.
//
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAXLINE 100
char userLine[MAXLINE];
char command[MAXLINE];
//split the command
int splitCommand(char* userLine, char* argv[]);
void getUserNameLine(char* userLine);
int userType();
int splitCommand(char* userLine, char* argv[]) {
    int i = 0;
    char* string;
    argv[i++] = strtok(userLine, " ");
    while ((string = strtok(NULL, " \n")) != NULL) {
        argv[i++] = string;
        printf("arg:%s\n", argv[i-1]);
    }
    return 0;
}
void getUserNameLine(char* userLine) {
    userLine[0]='\0';
    struct passwd *psw;
    psw = getpwuid(getuid());
    strcat(userLine, psw->pw_name);
    strcat(userLine, "@");
    char hostName[MAXLINE];
    char wd[MAXLINE];
    getwd(wd);
    if(gethostname(hostName, sizeof(hostName)))
        return;
    else
        strcat(userLine, hostName);
    strcat(userLine, ":");
    strcat(userLine, wd);
    userType();
}
int userType() {
    if(strcmp("root", getenv("USER")) == 0)
        strcat(userLine, "#");
    else
        strcat(userLine, "$");
}
int main() {
    int pipe_fd[2];
    int status;
    pipe(pipe_fd);
    char *argv[MAXLINE];
    pid_t child1, child2;
    while (1) {
        getUserNameLine(userLine);
        printf("[%s]", userLine);
        fgets(command, MAXLINE, stdin);
        if(splitCommand(command, argv)) {
            printf("split failed!\n");
        }
        if ((child1 = fork())) {
            if ((child2) = fork() == 0) {
                close(pipe_fd[1]);
                close(fileno(stdin));
                dup2(pipe_fd[0] ,fileno(stdin));
                close(pipe_fd[0] );
                execvp(argv[0], argv);
            }
            else {
                close(pipe_fd[0]);
                close(pipe_fd[1]);
                waitpid(child2, &status , 0);
            }
            waitpid(child1, &status, 0);
        }
        else {
            printf ("subshell 3cmd %d\n", getpid());
            close (pipe_fd[0]);
            close (fileno(stdout));
            dup2 (pipe_fd[1], fileno(stdout));
            close(pipe_fd[1]);
            execvp(argv[0], argv);
        }
    }
    return 0;
}