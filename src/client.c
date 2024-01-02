/* 
    ********************************************************************
    Odsek:          Elektrotehnika i racunarstvo
    Departman:      Racunarstvo i automatika
    Katedra:        Racunarska tehnika i racunarske komunikacije (RT-RK)
    Predmet:        Osnovi Racunarskih Mreza 1
    Godina studija: Treca (III)
    Skolska godina: 2021/22
    Semestar:       Zimski (V)
    
    Ime fajla:      client.c
    Opis:           TCP/IP klijent
    
    Platforma:      Raspberry Pi 2 - Model B
    OS:             Raspbian
    ********************************************************************
*/

#include <sys/socket.h> //socket
#include <arpa/inet.h>  //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include <pthread.h> //threads
#include <unistd.h> //sleep
#include <stdlib.h>


#include "command.h"
#include "requests.h"


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015
#define DEFAULT_LEN 12

void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}




Command mainMenu()
{
    Command command;
    printf("Odaberite jednu od komandi: \n");
    printf("1. Login\n");
    printf("2. Logout\n");
    printf("3. Send\n");
    printf("4. Check\n");
    printf("5. Receive\n");
    printf("6. Exit\n");
    scanf("%u", &command);
    return command;
}

void commandMenu(Command input, char clientMessage[])
{
    flushStdin();
    switch(input)
    {
        case LOGIN: 
            makeLoginRequest(clientMessage);
            break;
        case LOGOUT: 
            makeLogoutRequest(clientMessage);
            break;
        case SEND:
            makeSendRequest(clientMessage);
            break;
        case CHECK:
            makeCheckRequest(clientMessage);
            break;
        case RECEIVE:
            makeReceiveRequest(clientMessage);
            break;
        default:
            break;
    }
}

void* recv_thread(void* args)
{
    int sock = *((int*)args);
    int readSize;
    char serverMessage[DEFAULT_BUFLEN] = "";
    while((readSize = recv(sock, serverMessage, DEFAULT_BUFLEN, 0)) > 0)
    {
        serverMessage[readSize] = '\0';
        printf("%s", serverMessage);
    }
    return 0;
}

int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    Command selectedCommand;

    system("clear");
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
        return 0;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 0;
    }

    pthread_t hReceiver;
    pthread_create(&hReceiver, NULL, recv_thread, (void*)&sock);


    do{
        
        char clientMessage[DEFAULT_BUFLEN] = "";
        selectedCommand = mainMenu();
        commandMenu(selectedCommand, clientMessage);
        system("clear");

        //Send some data
        if( send(sock , clientMessage , strlen(clientMessage), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }

        usleep(50000);
    }while(selectedCommand != EXIT);
    

    close(sock);

    return 0;
}

