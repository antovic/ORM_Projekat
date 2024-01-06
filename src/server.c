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
#include "handles.h"
#include "constants.h"


//Listens  to client messages and calls the correct handle
void* recvThread(void* args)
{
    int i;
	userData *userInfo = (userData*)args; 
	int *clientSock = userInfo->socket;
	char clientMessage[DEFAULT_BUFLEN] = "";
    char commandString[DEFAULT_BUFLEN];
	int read_size;
    while( (read_size = recv(*clientSock , clientMessage , DEFAULT_BUFLEN , 0)) > 0 )
    {
        //client-side output
        printf("Bytes received: %d\n", read_size);
        printf("Client %d message: '%s'\n", *clientSock, clientMessage);
        //Message to command
        strcpy(commandString, clientMessage);
        strtok(commandString, " ");
        Command userCommand = stringToCommand(commandString);

        switch(userCommand)
        {
        case LOGIN:
            handleLogin(clientMessage, userInfo);
            break;
        case LOGOUT: 
            handleLogout(userInfo);
            break;
        case SEND:
            handleSend(clientMessage, userInfo);
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
            clientMessage[i] = '\0';
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
    close(*clientSock);
    return 0;
}



int main(int argc , char *argv[])
{
    int socket_desc , c;
    struct sockaddr_in server , client;
    pthread_t hReciever;
    //Clear the terminal
    system("clear");

    //Creating the socket
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

    //Lets socket be reusable
    const int enable = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const void*) &enable, sizeof(enable));

    //Binding the socket
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    listen(socket_desc , 3);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //For each connection call the thread to receive messages
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
        pthread_create(&hReciever, NULL, recvThread, (void*)&userInfo);
        puts("Waiting for additional incoming connections...");
    }
    close(socket_desc);
    return 0;
}