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
} userData;



void* recv_thread(void* args)
{
	//Receive a message from client
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

        //Obrada zahteva
        switch(userCommand)
        {
        case LOGIN: //TODO PROVERE
            break;
        case LOGOUT: 
            break;
        case SEND:
            break;
        case CHECK:
            break;
        case RECEIVE:
            break;
        default:
            break;
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
    return 0;
}



int main(int argc , char *argv[])
{
    int socket_desc , c;
    struct sockaddr_in server , client;
    pthread_t hReciever;
    //Create socket
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
        if (client_sock < 0)
        {
           perror("accept failed");
           return 1;
        }
        puts("Connection accepted");
        pthread_create(&hReciever, NULL, recv_thread, (void*)&userInfo);
        puts("Waiting for additional incoming connections...");
    }

    return 0;
}

