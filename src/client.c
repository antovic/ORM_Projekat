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
        selectedCommand = Prompt();

        if(selectedCommand < REGISTER || selectedCommand > EXIT)
            continue;
        if(selectedCommand == EXIT)
            break;

        MakeRequest(selectedCommand, clientMessage);

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

    }while(1);
    

    close(sock);
    return 0;
}

