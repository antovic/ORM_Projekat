
#include<stdio.h>      //printf
#include<string.h>     //strlen

typedef enum {LOGIN = 1, LOGOUT, SEND, CHECK, RECEIVE, EXIT} Command;

Command stringToCommand(char input[])
{  
    Command ret = EXIT;
    if(strcmp(input, "LOGIN"))
        ret = LOGIN;
    else if(strcmp(input, "LOGOUT"))
        ret = LOGOUT;
    else if(strcmp(input, "SEND"))
        ret = SEND;
    else if(strcmp(input, "CHECK"))
        ret = CHECK;
    else if(strcmp(input, "RECEIVE"))
        ret = RECEIVE;
    return ret; 
}