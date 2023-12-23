#include<stdio.h>      //printf
#include<string.h>     //strlen



#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define DEFAULT_LEN 12
#define DEFAULT_MESLEN 128


void makeLoginRequest(char clientMessage[])
{
    char username[DEFAULT_LEN];
    char password[DEFAULT_LEN];
    printf("Unesite korisnicko ime: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");
    printf("Unesite lozinku: ");
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
    printf("Kome saljete: ");
    fgets(username, sizeof(username), stdin);
    strtok(username, "\n");
    printf("Unesite poruku (MAX 128 karaktera): ");
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