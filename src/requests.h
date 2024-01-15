#include<stdio.h>      //printf
#include<string.h>     //strlen

#include "constants.h"

void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void MakeRegisterRequest(char clientMessage[])
{
    char username[DEFAULT_LEN];
    char password[DEFAULT_LEN];
    printf("Enter a username: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");
    printf("Enter a password: ");
    fgets(password, sizeof(password), stdin);
    strtok(password, "\n");
    strcat(clientMessage, "REGISTER");
    strcat(clientMessage, " ");
    strcat(clientMessage, username);
    strcat(clientMessage, " ");
    strcat(clientMessage, password);
}

void MakeLoginRequest(char clientMessage[])
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

void MakeLogoutRequest(char clientMessage[])
{
    strcat(clientMessage, "LOGOUT");
}

void MakeSendRequest(char clientMessage[])
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

void MakeCheckRequest(char clientMessage[])
{
    strcat(clientMessage, "CHECK");
}

void MakeReceiveRequest(char clientMessage[])
{
    strcat(clientMessage, "RECEIVE");
}



//Chooses the correct request format to send to the server
void MakeRequest(Command input, char clientMessage[])
{
    flushStdin();
    switch(input)
    {
        case REGISTER:
            MakeRegisterRequest(clientMessage);
            break;
        case LOGIN: 
            MakeLoginRequest(clientMessage);
            break;
        case LOGOUT: 
            MakeLogoutRequest(clientMessage);
            break;
        case SEND:
            MakeSendRequest(clientMessage);
            break;
        case CHECK:
            MakeCheckRequest(clientMessage);
            break;
        case RECEIVE:
            MakeReceiveRequest(clientMessage);
            break;
        default:
            break;
    }
}