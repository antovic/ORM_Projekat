/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      server.c
    Opis:           TCP/IP server
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#include <stdlib.h> //malloc
#include<pthread.h> //threads


#include "command.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015


typedef struct userData
{
	int *socket;
    char username[12];
    int loggedIn;
} userData;

int handleLogin(char *clientMessage, userData *userInfo)
{
    char response[DEFAULT_BUFLEN];
    FILE* users = fopen("../files/users.txt", "r");
    if(users == NULL)
    {
        strcpy(response,"Encountered an error while logging in. Please try again later.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    if(userInfo->loggedIn == 1)
    {
        strcpy(response,"Already logged in. Logout to proceed\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    char username[12], password[12], clientUsername[12], clientPassword[12];
    strtok(clientMessage, " ");
    strcpy(clientUsername, strtok(NULL, " "));
    strcpy(clientPassword, strtok(NULL, " \n"));
    while(fscanf(users, "%s %s", username, password) == 2)
    {
        if((strcmp(username, clientUsername) == 0) && (strcmp(password, clientPassword) == 0))
        {
            strcpy(userInfo->username, username);
            userInfo->loggedIn = 1;
            printf("CAO\n");
            strcpy(response,"Login successful.\n");
            if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
                puts("Send failed");
            return 1;
        }
    }
    strcpy(response,"Wrong username or password, try again.\n");
    if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
    // TODO: sendResponse(userInfo->socket, asjdiaspodjas, strlen(poruka))
    fclose(users);
    return 0;
}

int handleLogout(userData *userInfo)
{
    char response[DEFAULT_BUFLEN];
    if(userInfo->loggedIn == 0)
    {
        strcpy(response,"You are not logged in.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    else
    {
        userInfo->username[0] = '\0';
        userInfo->loggedIn = 0;
        strcpy(response,"Logout successful.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed"); 
        return 1;
    }
}

int handleSend(char *clientMessage, userData *userInfo)
{
    int found = 0;
    char username[12], password[12], recipient[12], message[128], mailboxPath[128], response[DEFAULT_BUFLEN];
    FILE *mailbox, *users;
    if(userInfo->loggedIn == 0)
    {
        strcpy(response,"You are not logged in.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    strtok(clientMessage, " ");
    strcpy(recipient, strtok(NULL, " "));
    strcpy(message, strtok(NULL, "\n"));
    users = fopen("../files/users.txt", "r");
    if(users == NULL)
    {
        strcpy(response,"Encountered an error while sending the message.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    while(fscanf(users, "%s %s", username, password) == 2)
    {
        if(strcmp(username, recipient) == 0)
        {
            found = 1;
        }
    }
    if(found == 0)
    {
        strcpy(response,"User does not exist.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, recipient);
    strcat(mailboxPath, ".txt");
    //printf("%s\n", mailboxPath); for debug
    mailbox = fopen(mailboxPath, "a+");
    if(mailbox == NULL)
    {
        strcpy(response,"Encountered an error while sending the message.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    strcpy(response,"Message sent.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
    fprintf(mailbox, "%s: %s\n", userInfo->username, message);
    fclose(users);
    fclose(mailbox);
    return 1;
}

int handleCheck(userData* userInfo)
{
    char mailboxPath[128], response[DEFAULT_BUFLEN];
    FILE *mailbox;
    if(userInfo->loggedIn == 0)
    {
        strcpy(response,"You are not logged in.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, userInfo->username);
    strcat(mailboxPath, ".txt");
    mailbox = fopen(mailboxPath, "r");
    if(mailbox == NULL)
    {
        strcpy(response,"Mailbox is either empty or unavailable at the moment. Please try again.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    
    strcpy(response,"You have some messages in your mailbox.\n");
    if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
        puts("Send failed");
    fclose(mailbox);
    return 1;
}

int handleReceive(userData* userInfo)
{
    char mailboxPath[128], content[256], response[DEFAULT_BUFLEN];
    FILE *mailbox;
    if(userInfo->loggedIn == 0)
    {
        strcpy(response,"You are not logged in.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, userInfo->username);
    strcat(mailboxPath, ".txt");
    mailbox = fopen(mailboxPath, "r");
    if(mailbox == NULL)
    {
        strcpy(response,"Mailbox is either empty or unavailable at the moment. Please try again.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    while(fgets(content, 256, mailbox)) //reads the entire line(a message) from mailbox into content
    {
        strcpy(response,content);
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        //delete file 
    }
    fclose(mailbox);
    return 1;

}



void* recv_thread(void* args)
{
	//Receive a message from client
    int i;
	userData *userInfo = (userData*)args; 
	int *client_sock = userInfo->socket;
	char client_message[DEFAULT_BUFLEN] = "";
    char commandString[DEFAULT_BUFLEN];
	int read_size;
    while( (read_size = recv(*client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
        printf("Client %d message: '%s'\n", *client_sock, client_message);
        //Obrada poruke
        strcpy(commandString, client_message);
        strtok(commandString, " ");
        printf("KOMANDA: '%s'\n", commandString);
        Command userCommand = stringToCommand(commandString);
        //printf("userCommand = %s\nCommand = %d\n", commandString, userCommand);
        //Obrada zahteva
        switch(userCommand)
        {
        case LOGIN: //TODO PROVERE
            handleLogin(client_message, userInfo);
            printf("%s %d\n", userInfo->username, userInfo->loggedIn);
            break;
        case LOGOUT: 
            handleLogout(userInfo);
            break;
        case SEND:
            handleSend(client_message, userInfo);
            break;
        case CHECK:
            handleCheck(userInfo);
            break;
        case RECEIVE:
            handleReceive(userInfo);
            break;
        default:
            //do nothing for exit
            break;
        }
        for(i = 0; i < DEFAULT_BUFLEN; i++)
        {
            client_message[i] = '\0';
        }
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed!");
    }
    close(*client_sock);
    return 0;
}



int main(int argc , char *argv[])
{
    int socket_desc , c;
    struct sockaddr_in server , client;
    pthread_t hReciever;
    //Create socket
    system("clear");
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    const int enable = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void*) &enable, sizeof(enable));

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    while(1)
    {
    	int* client_sock = (int*) malloc(sizeof(int*));
        *client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        userData userInfo;
        userInfo.socket = client_sock;
        userInfo.loggedIn = 0;
        if (client_sock < 0)
        {
           perror("accept failed");
           return 1;
        }
        puts("Connection accepted");
        pthread_create(&hReciever, NULL, recv_thread, (void*)&userInfo);
        puts("Waiting for additional incoming connections...");
    }
    close(socket_desc);
    return 0;
}