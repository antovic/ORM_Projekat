
#include<stdio.h>      //printf
#include<string.h>     //strlen

typedef enum {LOGIN = 1, LOGOUT, SEND, CHECK, RECEIVE, EXIT} Command;

Command stringToCommand(char input[])
{  
    Command ret = EXIT;
    if(strcmp(input, "LOGIN") == 0)
        ret = LOGIN;
    else if(strcmp(input, "LOGOUT") == 0)
        ret = LOGOUT;
    else if(strcmp(input, "SEND") == 0)
        ret = SEND;
    else if(strcmp(input, "CHECK") == 0)
        ret = CHECK;
    else if(strcmp(input, "RECEIVE") == 0)
        ret = RECEIVE;
    return ret; 
}