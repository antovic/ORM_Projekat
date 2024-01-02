#include<stdio.h>      //printf
#include<string.h>     //strlen

#include "constants.h"


void makeLoginRequest(char clientMessage[])
{
    char username[DEFAULT_LEN];
    char password[DEFAULT_LEN];
    printf("Enter a username: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");
    printf("Enter a password: ");
    fgets(password, sizeof(password), stdin);
    strtok(password, "\n");
    strcat(clientMessage, "LOGIN");
    strcat(clientMessage, " ");
    strcat(clientMessage, username);
    strcat(clientMessage, " ");
    strcat(clientMessage, password);
}

void makeLogoutRequest(char clientMessage[])
{
    strcat(clientMessage, "LOGOUT");
}

void makeSendRequest(char clientMessage[])
{
    char username[DEFAULT_LEN];
    char message[DEFAULT_MESLEN];
    printf("Who do you wish to send a message: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");
    printf("Enter a message (MAX 128 characters): ");
    fgets(message, sizeof(message), stdin);
    strtok(message, "\n");
    strcat(clientMessage, "SEND");
    strcat(clientMessage, " ");
    strcat(clientMessage, username);
    strcat(clientMessage, " ");
    strcat(clientMessage, message);
}

void makeCheckRequest(char clientMessage[])
{
    strcat(clientMessage, "CHECK");
}

void makeReceiveRequest(char clientMessage[])
{
    strcat(clientMessage, "RECEIVE");
}