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
#include <unistd.h> //sleep
#include <stdlib.h>


#include "command.h"
#include "requests.h"


void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Chooses the correct request format to send to the server
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


//Displays the main dialogue
Command prompt()
{
    Command command;
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
    return command;
}


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    Command selectedCommand;

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

    //Connect to a remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 0;
    }


    do{
        char dummy, clientMessage[DEFAULT_BUFLEN] = "", serverMessage[DEFAULT_BUFLEN] = "";    
        int readSize;
        selectedCommand = prompt();
        makeRequest(selectedCommand, clientMessage);

        //Send the user input
        if( send(sock , clientMessage , strlen(clientMessage), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
	    //Receive the server response
        if((readSize = recv(sock, serverMessage, DEFAULT_BUFLEN, 0)) > 0)
        {
            system("clear");
            serverMessage[readSize] = '\0';
            printf("%s", serverMessage);
            printf("Press any key to continue.\n");
            scanf("%c", &dummy);
        }
        else if(readSize == 0)
        {
            printf("Server has closed connection.\n");
            break;
        }
        else
        {
            printf("An error occured while communicating to the server.\n");
        }

    }while(selectedCommand != EXIT);
    

    close(sock);
    return 0;
}

