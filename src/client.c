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
#include <semaphore.h> //semaphore
#include <unistd.h> //sleep
#include <stdlib.h>


#include "command.h"
#include "requests.h"

static pthread_mutex_t cs_mutex;
static sem_t semaphore;

void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void makeRequest(Command input, char clientMessage[])
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



Command mainMenu()
{
    Command command;
    pthread_mutex_lock(&cs_mutex);
    system("clear");
    printf("Select one command:\n");
    printf("1. Login\n");
    printf("2. Logout\n");
    printf("3. Send\n");
    printf("4. Check\n");
    printf("5. Receive\n");
    printf("6. Exit\n");
    scanf("%u", &command);
    system("clear");
    pthread_mutex_unlock(&cs_mutex);
    return command;
}



void* recvServerResponses(void* args)
{
    int sock = *((int*)args);
    int readSize;
    char dummy;
    char serverMessage[DEFAULT_BUFLEN] = "";
    while((readSize = recv(sock, serverMessage, DEFAULT_BUFLEN, 0)) > 0)
    {
        pthread_mutex_lock(&cs_mutex);
        system("clear");
        serverMessage[readSize] = '\0';
        printf("%s", serverMessage);
        printf("Press any key to continue.\n");
        scanf("%c", &dummy);
        sem_post(&semaphore);
        pthread_mutex_unlock(&cs_mutex);
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

    //Start the thread that listens to server responses
    pthread_mutex_init(&cs_mutex, NULL);
    sem_init(&semaphore, 0, 1);
    pthread_t hReceiver;
    pthread_create(&hReceiver, NULL, recvServerResponses, (void*)&sock);


    do{
        char clientMessage[DEFAULT_BUFLEN] = "";
        sem_wait(&semaphore);
        selectedCommand = mainMenu();
        makeRequest(selectedCommand, clientMessage);

        //Send some data
        if( send(sock , clientMessage , strlen(clientMessage), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
    }while(selectedCommand != EXIT);
    

    close(sock);
    pthread_mutex_destroy(&cs_mutex);
    sem_destroy(&semaphore);
    return 0;
}

