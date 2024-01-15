#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

//Necessary for tracking different clients
typedef struct userData
{
	int *socket;
    char username[DEFAULT_LEN];
    int loggedIn;
} userData;

void SendResponse(userData *userInfo, char *response)
{
    if(send(*(userInfo->socket) , response , strlen(response), 0) < 0)
        puts("Send failed");
}

int HandleRegister(char *clientMessage, userData *userInfo)
{
    char username[DEFAULT_LEN], password[DEFAULT_LEN], clientUsername[DEFAULT_LEN], clientPassword[DEFAULT_LEN];
    FILE* users = fopen("../files/users.txt", "r+");
    //Checks usual client errors
    if(users == NULL)
    {
        SendResponse(userInfo, "Error while trying to register. Please try again\n");
        return 0;
    }
    if(userInfo->loggedIn == 1)
    {
        SendResponse(userInfo, "Already logged in. Logout to proceed\n");
        return 0;
    }
    //Checks if the user already exists
    strtok(clientMessage, " ");
    strcpy(clientUsername, strtok(NULL, " "));
    strcpy(clientPassword, strtok(NULL, " \n"));
    while(fscanf(users, "%s %s", username, password) == 2)
    {
        if((strcmp(username, clientUsername) == 0))
        {
            SendResponse(userInfo, "Username already taken. Please try another one.\n");
            return 0;
        }
    }
    fprintf(users, "%s %s\n", clientUsername, clientPassword);
    strcpy(userInfo->username, clientUsername);
    userInfo->loggedIn = 1;
    SendResponse(userInfo, "Your account has been created.\nYou are logged in.\n");
    fclose(users);
    return 1;
}


int HandleLogin(char *clientMessage, userData *userInfo)
{
    char username[DEFAULT_LEN], password[DEFAULT_LEN], clientUsername[DEFAULT_LEN], clientPassword[DEFAULT_LEN];
    FILE* users = fopen("../files/users.txt", "r");
    //Checks usual client errors
    if(users == NULL)
    {
        SendResponse(userInfo, "Encountered an error while logging in.\nPlease try again later\n");
        return 0;
    }
    if(userInfo->loggedIn == 1)
    {
        SendResponse(userInfo, "Already logged in.\nLogout to proceed\n");
        return 0;
    }
    //Checks if login info is correct
    strtok(clientMessage, " ");
    strcpy(clientUsername, strtok(NULL, " "));
    strcpy(clientPassword, strtok(NULL, " \n"));
    while(fscanf(users, "%s %s", username, password) == 2)
    {
        if((strcmp(username, clientUsername) == 0) && (strcmp(password, clientPassword) == 0))
        {
            strcpy(userInfo->username, username);
            userInfo->loggedIn = 1;
            SendResponse(userInfo, "Login successful.\n");
            return 1;
        }
    }
    SendResponse(userInfo, "Wrong username or password.\nTry again.\n");
    fclose(users);
    return 0;
}

int HandleLogout(userData *userInfo)
{
    if(userInfo->loggedIn == 0)
    {
        SendResponse(userInfo, "You are not logged in.\n");
        return 0;
    }
    userInfo->username[0] = '\0';
    userInfo->loggedIn = 0;
    SendResponse(userInfo, "Logout successful.\n"); 
    return 1;
}

int HandleSend(char *clientMessage, userData *userInfo)
{
    int found = 0;
    char username[DEFAULT_LEN], password[DEFAULT_LEN], recipient[DEFAULT_LEN], message[DEFAULT_MESLEN], mailboxPath[DEFAULT_PATHLEN];
    FILE *mailbox, *users;
    //Checks usual client errors
    if(userInfo->loggedIn == 0)
    {
        SendResponse(userInfo, "You are not logged in.\n");
        return 0;
    }
    strtok(clientMessage, " ");
    strcpy(recipient, strtok(NULL, " "));
    strcpy(message, strtok(NULL, "\n"));
    users = fopen("../files/users.txt", "r");
    if(users == NULL)
    {
        SendResponse(userInfo, "Encountered an error while sending the message.\n");
        return 0;
    }
    //Looks for the named user
    while(fscanf(users, "%s %s", username, password) == 2)
    {
        if(strcmp(username, recipient) == 0)
        {
            found = 1;
        }
    }
    if(found == 0)
    {
        SendResponse(userInfo, "User does not exist.\n");
        return 0;
    }
    //Sends the message if user exists
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, recipient);
    strcat(mailboxPath, ".txt");
    mailbox = fopen(mailboxPath, "a+");
    if(mailbox == NULL)
    {
        SendResponse(userInfo, "Encountered an error while sending the message.\n");
        return 0;
    }
    fprintf(mailbox, "%s: %s\n", userInfo->username, message);
    SendResponse(userInfo, "Message sent.\n");
    fclose(users);
    fclose(mailbox);
    return 1;
}

int HandleCheck(userData* userInfo)
{
    char mailboxPath[DEFAULT_PATHLEN];
    FILE *mailbox;
    if(userInfo->loggedIn == 0)
    {
        SendResponse(userInfo, "You are not logged in.\n");
        return 0;
    }
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, userInfo->username);
    strcat(mailboxPath, ".txt");
    mailbox = fopen(mailboxPath, "a+");
    if(mailbox == NULL)
    {
        SendResponse(userInfo, "Mailbox is unavailable at the moment.\nPlease try again.\n");
        return 0;
    }

    fseek (mailbox, 0, SEEK_END);
    if(ftell(mailbox) == 0)
        SendResponse(userInfo, "Your mailbox is empty.\nPlease try again later.\n");
    else
        SendResponse(userInfo, "You have some messages in your mailbox.\n");
    fclose(mailbox);
    return 1;
}

int HandleReceive(userData* userInfo)
{
    char mailboxPath[DEFAULT_MESLEN], mailboxCopyPath[DEFAULT_MESLEN], content[DEFAULT_MESLEN + DEFAULT_LEN + 2];
    FILE *mailbox, *mailboxCopy;
    if(userInfo->loggedIn == 0)
    {
        SendResponse(userInfo, "You are not logged in.\n");
        return 0;
    }
    strcpy(mailboxPath, "../files/");
    strcat(mailboxPath, userInfo->username);
    strcat(mailboxPath, ".txt");
    mailbox = fopen(mailboxPath, "r");
    if(mailbox == NULL)
    {
        SendResponse(userInfo, "Mailbox is unavailable at the moment.\nPlease try again.\n");
        return 0;
    }

    //Reads the message and sends it back to client
    if(fgets(content, DEFAULT_MESLEN + DEFAULT_LEN + 2, mailbox))
        SendResponse(userInfo, content);
    else
        SendResponse(userInfo, "No messages for you at the moment.\nTry again later.\n");    

    //Removes the message after reading it
    strcpy(mailboxCopyPath, "../files/");
    strcat(mailboxCopyPath, userInfo->username);
    strcat(mailboxCopyPath, "_copy.txt");
    mailboxCopy = fopen(mailboxCopyPath, "w");
    while(fgets(content, DEFAULT_MESLEN + DEFAULT_LEN + 2, mailbox))
    {
        fprintf(mailboxCopy, "%s", content);
    }
    fclose(mailbox);
    fclose(mailboxCopy);
    remove(mailboxPath);
    rename(mailboxCopyPath, mailboxPath);
    return 1;

}

//Decides which handle to use for clients input
void HandleRequest(char *clientMessage, userData* userInfo)
{
    char commandString[DEFAULT_BUFLEN];
    strcpy(commandString, clientMessage);
    strtok(commandString, " ");
    Command userCommand = StringToCommand(commandString);
    switch(userCommand)
    {
        case REGISTER:
            HandleRegister(clientMessage, userInfo);
            break;
        case LOGIN:
            HandleLogin(clientMessage, userInfo);
            break;
        case LOGOUT: 
            HandleLogout(userInfo);
            break;
        case SEND:
            HandleSend(clientMessage, userInfo);
            break;
        case CHECK:
            HandleCheck(userInfo);
            break;
        case RECEIVE:
            HandleReceive(userInfo);
            break;
        default:
            //do nothing for exit
            break;
    }
}