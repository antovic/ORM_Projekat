
#include<stdio.h>      //printf
#include<string.h>     //strlen

typedef enum {REGISTER = 1, LOGIN, LOGOUT, SEND, CHECK, RECEIVE, EXIT} Command;

//Displays the main dialogue
Command Prompt()
{
    Command command;
    system("clear");
    printf("Select one command:\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Logout\n");
    printf("4. Send\n");
    printf("5. Check\n");
    printf("6. Receive\n");
    printf("7. Exit\n");
    scanf("%u", &command);
    system("clear");
    return command;
}


Command StringToCommand(char input[])
{  
    Command ret = EXIT;
    if(strcmp(input, "REGISTER") == 0)
        ret = REGISTER;
    else if(strcmp(input, "LOGIN") == 0)
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


