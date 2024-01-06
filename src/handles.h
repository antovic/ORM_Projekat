#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

typedef struct userData
{
	int *socket;
    char username[DEFAULT_LEN];
    int loggedIn;
} userData;

int handleLogin(char *clientMessage, userData *userInfo)
{
    char username[DEFAULT_LEN], password[DEFAULT_LEN], clientUsername[DEFAULT_LEN], clientPassword[DEFAULT_LEN], response[DEFAULT_BUFLEN];
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
    char username[DEFAULT_LEN], password[DEFAULT_LEN], recipient[DEFAULT_LEN], message[DEFAULT_MESLEN], mailboxPath[DEFAULT_PATHLEN], response[DEFAULT_BUFLEN];
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
    char mailboxPath[DEFAULT_PATHLEN], response[DEFAULT_BUFLEN];
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
    mailbox = fopen(mailboxPath, "a+");
    if(mailbox == NULL)
    {
        strcpy(response,"Mailbox is unavailable at the moment. Please try again later.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }

    fseek (mailbox, 0, SEEK_END);
    if(ftell(mailbox) == 0)
    {
        strcpy(response,"Your mailbox is empty. Please try again later.\n");
    }
    else
    {
        strcpy(response,"You have some messages in your mailbox.\n");
    }
    if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
        puts("Send failed");
    fclose(mailbox);
    return 1;
}

int handleReceive(userData* userInfo)
{
    char mailboxPath[DEFAULT_MESLEN], mailboxCopyPath[DEFAULT_MESLEN], content[DEFAULT_MESLEN + DEFAULT_LEN + 2], response[DEFAULT_BUFLEN];
    FILE *mailbox, *mailboxCopy;
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
        strcpy(response,"Mailbox is unavailable at the moment. Please try again later.\n");
        if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
            puts("Send failed");
        return 0;
    }
    // while(fgets(content, DEFAULT_MESLEN + DEFAULT_LEN + 2, mailbox)) //reads the entire line(a message) from mailbox into content
    // {
    //     strcpy(response,content);
    //     if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
    //         puts("Send failed"); 
    // }
    if(fgets(content, DEFAULT_MESLEN + DEFAULT_LEN + 2, mailbox))
    {
        strcpy(response,content);
    }
    else
    {
        strcpy(response, "No available messages at the moment. Please try again later\n");
    }
    if( send(*(userInfo->socket) , response , strlen(response), 0) < 0)
        puts("Send failed");
    strcpy(mailboxCopyPath, "../files/");
    strcat(mailboxCopyPath, userInfo->username);
    strcat(mailboxCopyPath, "_copy.txt");
    mailboxCopy = fopen(mailboxCopyPath, "w");
    while(fgets(content, DEFAULT_MESLEN + DEFAULT_LEN + 2, mailbox)) //reads the entire line(a message) from mailbox into content
    {
        fprintf(mailboxCopy, "%s", content);
    }
    fclose(mailbox);
    fclose(mailboxCopy);
    remove(mailboxPath);
    rename(mailboxCopyPath, mailboxPath);
    return 1;

}