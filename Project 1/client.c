/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:    Kee-Bum Philip Phiri 
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */

    char *cmd;                  /* Command of BAL | WITHDRAW | TRANSFER*/
    // char *accountName;		    /* Account Name  */
    char *curAccount;           /* From Account: myChecking|mySavings|myCD|my401k */
    char *transferAccount;      /* To Account: myChecking|mySavings|myCD|my401k */
    int balance;            /* Account balance */
    int amount;             /* CMD Amount */
    char *servIP;		    /* Server IP address  */
    unsigned short servPort;	    /* Server Port number */
    

    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    

    /* Get the Account Name from the command line */
    
    //Make sure memory blocks are clear 
    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);
    memset(&serv_addr, 0, sizeof(serv_addr));

    cmd = argv[1];              //second argument 
    curAccount = argv[2];      //third argument 
    servIP = argv[argc - 2];     //second to last argument
    servPort = (unsigned short) atoi(argv[argc - 1]);   //last argument
    amount = 0;

     if (argc < 5 || argc > 7) 
    {
        printf("Incorrect number of arguments. The correct format is:\taccountName serverIP serverPort\n");
        exit(1);
    } else if (strcmp(cmd, "BAL") != 0 && strcmp(cmd, "WITHDRAW") != 0 && strcmp(cmd, "TRANSFER") != 0) {
        printf("Incorrect command. Valid commands are (BAL | WITHDRAW | TRANSFER) \n");
        exit(1);
     } else if (strcmp(cmd, "BAL") == 0 && argc != 5) {
        printf("Incorrect BALANCE format. The correct format is:  BAL accountName serverIP serverPort\n");
        exit(1);
     } else if (strcmp(cmd, "WITHDRAW") == 0 && argc != 6) {
        printf("Incorrect WITHDRAW format. The correct format is:  WITHDRAW accountName amount serverIP serverPort\n");
        exit(1);
     } else if (strcmp(cmd, "TRANSFER") == 0 && argc != 7) {
        printf("Incorrect TRANSFER format. The correct format is:  TRANSFER accountName amount serverIP serverPort\n");
        exit(1);
     }  else if (strcmp(curAccount, "myChecking") != 0 && strcmp(curAccount, "mySavings") && strcmp(curAccount, "myCD") && strcmp(curAccount, "my401k") && strcmp(curAccount, "my529")) {
        printf("Incorrect account name. Valid account names are (myChecking | mySavings | myCD | my401k | my529)\n");
        exit(1);
     } 





    if (argc == 6) {            //CMD: WITHDRAW
        amount =(int) atoi(argv[3]);
    } else if (argc == 7) {     //CMD: TRANSFER 
        transferAccount = argv[3]; //myChecking|mySavings|myCD|my401k
        amount = (int) atoi(argv[4]);
    }

    /* Create a new TCP socket*/
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock < 0) {
        printf("Could not create socket.");
        exit(1);
    }

    /* Construct the server address structure */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(servIP);
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(servPort);

    /* Establish connecction to the server */
    if (connect(clientSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("Could not connect to server.");
        exit(1);
    }
    

    /* Send the string to the server */

    //Creating sendBug
    //Adding cmd to sendBug
    int send_length = 0;
    int i = 0;
    for(i = 0; i < strlen(cmd); i++) {

        sndBuf[send_length] = cmd[i];
        send_length++;
    }
    sndBuf[send_length++] = '-';




    //Adding account
    for(i = 0; i < strlen(curAccount); i++) {

        sndBuf[send_length] = curAccount[i];
        send_length++;
    }
    sndBuf[send_length++] = '-';

    //Adding second account
    if (strcmp(cmd, "TRANSFER") == 0) {

        for(i = 0; i < strlen(transferAccount); i++) {
            sndBuf[send_length] = transferAccount[i];
            send_length++;
        }
        sndBuf[send_length++] = '-';
    }


    //Adding amount to sendBug
    if(strcmp(cmd, "WITHDRAW") == 0 || strcmp(cmd, "TRANSFER") == 0) {
        sprintf(sndBuf + send_length,"%d",amount);
    }

    int bytes_received = send(clientSock, sndBuf, SNDBUFSIZE, 0);
    if (bytes_received < 0) {
        printf("send() failed.");
        exit(1);
    }

    /* Receive and print response from the server */
    unsigned int total_bytes_received = 0;

    while (total_bytes_received < REPLYLEN) {
        memset(&rcvBuf, 0, RCVBUFSIZE);
        bytes_received = recv(clientSock, rcvBuf, RCVBUFSIZE - 1, 0);
        if (bytes_received < 0) {
            printf("recv() failed.");
            exit(1);
        } else if (bytes_received == 0) {
            printf("recv() connection closed prematurely.");
            exit(1);
        }

        int received_amount = (int) atoi(rcvBuf);

        if(strcmp(cmd, "BAL") == 0) {
            printf("%s has a balance of %d\n", curAccount, received_amount);
        } else if(strcmp(cmd, "WITHDRAW") == 0) {
            if (strlen(rcvBuf) == 0) {
                printf("Failed withdraw from %s.\n", curAccount);
            } else {
                printf("Successful withdraw from %s of amount %d and the new balance is %d\n", curAccount, amount, received_amount);
            }
              } else if(strcmp(cmd, "TRANSFER") == 0) {
                if (strlen(rcvBuf) == 0) {
                    printf("Failed transfer from %s to %s. Did not have sufficient funds.", curAccount, transferAccount);
                } else {
                    printf("Successful transfer from %s to %s of amount %d and the new balance is %d\n", curAccount, transferAccount, amount, received_amount);
                }    
            } 
        return 0;

    }

    
}

