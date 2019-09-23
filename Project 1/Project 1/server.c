/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Kee-Bum Philip Phiri 
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <time.h>

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/



struct TimeNode {
    time_t time;
    struct TimeNode *next;
};


struct TimeQueue {
    struct TimeNode *front;
    struct TimeNode *last;
    unsigned int size;
};


void createQueue(struct TimeQueue *q) {
    q->size = 0;
    q->front = NULL;
    q->last = NULL;
    
}

int getFrontTime(struct TimeQueue *q) {
    return q->front->time;
}

void pop(struct TimeQueue *q) {
    struct TimeNode *tmp = q->front;
    q->front = q->front->next;
    q->size = q->size - 1;
    free(tmp);
}

void push(struct TimeQueue *q, time_t time) {
    if (q->front == NULL) {
        q->front = (struct TimeNode *) malloc(sizeof(struct TimeNode));
        q->front->time = time;
        q->front->next = NULL;
        q->last = q->front;
    } else {
        q->last->next = (struct TimeNode *) malloc(sizeof(struct TimeNode));
        q->last->next->time = time;
        q->last->next->next = NULL;
        q->last = q->last->next;
    }
    q->size = q->size + 1;
}


void removeOldNode(struct TimeQueue *myChecking_q, struct TimeQueue *mySavings_q, struct TimeQueue *myCD_q, struct TimeQueue *my529_q, time_t cur_time) {
    if (myChecking_q->size > 0)    {
        time_t prev_time = myChecking_q->front->time;
        while (myChecking_q->size > 0 && difftime(cur_time, prev_time) > 60) {
            prev_time = myChecking_q->front->time;
            pop(myChecking_q);

            printf("removeOldNode() size is: %d prev_time %s",  myChecking_q->size, ctime(&prev_time));
            printf("Difference is  %.2f seconds\n",difftime(cur_time, prev_time));
        }
    }
    if (mySavings_q->size > 0)    {
        time_t prev_time = mySavings_q->front->time;
        while (mySavings_q->size > 0 && difftime(cur_time, prev_time) > 60) {
            prev_time = mySavings_q->front->time;
            pop(mySavings_q);
        }
    }

    if (myCD_q->size > 0)    {
        time_t prev_time = myCD_q->front->time;
        while (myCD_q->size > 0 && difftime(cur_time, prev_time) > 60) {
            prev_time = myCD_q->front->time;
            pop(myCD_q);
        }
    }
    if (my529_q->size > 0)    {
        time_t prev_time = my529_q->front->time;
        while (my529_q->size > 0 && difftime(cur_time, prev_time) > 60) {
            prev_time = my529_q->front->time;
            pop(my529_q);
        }
    }
}



/* The main function */
int main(int argc, char *argv[])
{

    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store account name from client */
    int  balance;				/* Place to record account balance result */

    char cur_cmd[10];
    char cur_account[12];
    char second_account[12];
    int cur_amount = 0;

    char *servIP = "127.0.0.1";
    changeServPort = 1234;
    int myChecking_balance = 5000;
    int mySavings_balance= 6000;
    int myCD_balance= 7000;
    int my401k_balance= 8000;
    int my529_balance= 9000;

    //CREATING TIMING QUEUES
    struct TimeQueue myChecking_q;
    createQueue(&myChecking_q);

    struct TimeQueue mySavings_q;
    createQueue(&mySavings_q);

    struct TimeQueue myCD_q;
    createQueue(&myCD_q);

    struct TimeQueue my401k_q;
    createQueue(&my401k_q);

    struct TimeQueue my529_q;
    createQueue(&my529_q);

    if (argc == 3) {
        servIP = argv[1];
        changeServPort = (unsigned short) atoi(argv[2]);
        printf("Server is running on %s %d\n", servIP, changeServPort);
    } else {
        printf("Server is running on default ip/port %s %hu\n", servIP, changeServPort);
    }



    /* Create new TCP Socket for incoming requests*/
    int opt = 1; 
    char sndBuf[SNDBUFSIZE];


    if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 


    /* Construct local address structure*/
    if (setsockopt ( serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt) )) 
    { 
        perror("setsockopt failed"); 
        exit(EXIT_FAILURE); 
    } 

    

    /* Construct local address structure*/
    changeServAddr.sin_family = AF_INET; 
    changeServAddr.sin_addr.s_addr = inet_addr(servIP); 
    changeServAddr.sin_port = htons( changeServPort );


    
    
    /* Bind to local address structure */
    if (bind(serverSock, (struct sockaddr *) &changeServAddr, sizeof(changeServAddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    /* Listen for incoming connections */
    if (listen(serverSock, 4) < 0) 
    { 
        perror("listen failed"); 
        exit(EXIT_FAILURE); 
    } 

    // CREATE TIME QUEUE 

    /* Loop server forever*/
    while(1)
    {
    	/* Accept incoming connection */
    	if ((clientSock = accept(serverSock, (struct sockaddr *)&changeServAddr, (socklen_t*)&changeServAddr))<0) 
            { 
                perror("accept failed"); 
                exit(EXIT_FAILURE); 
            }

    	/* Extract the account name from the packet, store in nameBuf */
    	/* Look up account balance, store in balance */
        memset(nameBuf, 0, BUFSIZE);
    	int total_bytes_received = recv(clientSock, nameBuf, BUFSIZE - 1, 0);
        printf("RECEIVED: %s", nameBuf);

        

        if (total_bytes_received < 0) {
            printf("recv failed.");
            exit(1);
        } else if (total_bytes_received == 0) {
            printf("recv() connection closed prematurely.");
            exit(1);
        } 

        //  GET COMMAND
        memset(cur_cmd, 0, 10);
        int nameBuf_point = 0;
        memset(cur_cmd, 0, 10);
        while (nameBuf[nameBuf_point] != '-' ){
            cur_cmd[nameBuf_point] = nameBuf[nameBuf_point];
            nameBuf_point++;
        }
        nameBuf_point++;

        //  GET ACCOUNT NAME
        memset(cur_account, 0, 12);
        int cur_account_point = 0;
        while (nameBuf[nameBuf_point] != '-' ){
            cur_account[cur_account_point] = nameBuf[nameBuf_point];
            nameBuf_point++;
            cur_account_point++;
        }
        nameBuf_point++;


        //  GET SECOND ACCOUNT NAME
        if(strcmp(cur_cmd, "TRANSFER") == 0) {  
            memset(second_account, 0, 12);
            int second_account_point = 0;
            while (nameBuf[nameBuf_point] != '-' ){
                second_account[second_account_point] = nameBuf[nameBuf_point];
                nameBuf_point++;
                second_account_point++;
            }
            nameBuf_point++;
        }

        //  GET COMMAND AMOUNT
        if(strcmp(cur_cmd, "WITHDRAW") == 0 || strcmp(cur_cmd, "TRANSFER") == 0) {  
            cur_amount = (int) atoi(nameBuf + nameBuf_point);
        }
        



        memset(sndBuf, 0, SNDBUFSIZE);
        int sendBuf_point =0;
        
        time_t cur_time;
        time(&cur_time); 
        removeOldNode(&myChecking_q, &mySavings_q, &myCD_q, &my529_q, cur_time);
        
        if(strcmp(cur_cmd, "BAL") == 0) {       
            if (strcmp(cur_account, "myChecking") == 0) {
                sprintf(sndBuf,"%d",myChecking_balance);
            } else if (strcmp(cur_account, "mySavings") == 0) {
                sprintf(sndBuf,"%d",mySavings_balance);
            } else if (strcmp(cur_account, "myCD") == 0) {
                sprintf(sndBuf,"%d",myCD_balance);
            } else if (strcmp(cur_account, "my401k") == 0) {
                sprintf(sndBuf,"%d",my401k_balance);
            } else if (strcmp(cur_account, "my529") == 0) {
                sprintf(sndBuf,"%d",my529_balance);
            }
        }else if(strcmp(cur_cmd, "WITHDRAW") == 0) {
            int stopWithdraw = 0;
            if (strcmp(cur_account, "myChecking") == 0) {
                printf("TIME SIZE %d\n", myChecking_q.size);
                if (myChecking_q.size > 2) {
                    stopWithdraw = 1;
                }
                int b = 10;
                push(&myChecking_q, cur_time);
            } else if (strcmp(cur_account, "mySavings") == 0) {
                if (mySavings_q.size > 2) {
                    stopWithdraw = 1;
                }
                push(&mySavings_q, cur_time);
            } else if (strcmp(cur_account, "myCD") == 0) {
                if (myCD_q.size > 2) {
                    stopWithdraw = 1;
                }
                push(&myCD_q, cur_time);
            } else if (strcmp(cur_account, "my401k") == 0) {
                if (my401k_q.size > 2) {
                    stopWithdraw = 1;
                }
                push(&my401k_q, cur_time);
            } else if (strcmp(cur_account, "my529") == 0) {
                if (my529_q.size > 2) {
                    stopWithdraw = 1;
                }
                push(&my529_q, cur_time);
            }     

            if (stopWithdraw == 0) {
                if (strcmp(cur_account, "myChecking") == 0 && myChecking_balance >= cur_amount) {
                    myChecking_balance -= cur_amount;
                    sprintf(sndBuf,"%d",myChecking_balance);
                } else if (strcmp(cur_account, "mySavings") == 0 && mySavings_balance >= cur_amount) {
                    mySavings_balance -= cur_amount;
                    sprintf(sndBuf,"%d",mySavings_balance);
                } else if (strcmp(cur_account, "myCD") == 0 && myCD_balance >= cur_amount) {
                    myCD_balance -= cur_amount;
                    sprintf(sndBuf,"%d",myCD_balance);
                } else if (strcmp(cur_account, "my401k") == 0 && my401k_balance >= cur_amount) {
                    my401k_balance -= cur_amount;
                    sprintf(sndBuf,"%d",my401k_balance);
                } else if (strcmp(cur_account, "my529") == 0 && my529_balance >= cur_amount) {
                    my529_balance -= cur_amount;
                    sprintf(sndBuf,"%d",my529_balance);
                }
            }
    

        } else if (strcmp(cur_cmd, "TRANSFER") == 0) {
            // GET AMOUNT FROM CUR ACCOUNT
            int amount_removed = 0;
            if (strcmp(cur_account, "myChecking") == 0 && myChecking_balance >= cur_amount) {
                amount_removed = cur_amount;
                myChecking_balance -= cur_amount;
                sprintf(sndBuf,"%d",myChecking_balance);
            } else if (strcmp(cur_account, "mySavings") == 0 && mySavings_balance >= cur_amount) {
                amount_removed = cur_amount;
                mySavings_balance -= cur_amount;
                sprintf(sndBuf,"%d",mySavings_balance);
            } else if (strcmp(cur_account, "myCD") == 0 && myCD_balance >= cur_amount) {
                amount_removed = cur_amount;
                myCD_balance -= cur_amount;
                sprintf(sndBuf,"%d",myCD_balance);
            } else if (strcmp(cur_account, "my401k") == 0 && my401k_balance >= cur_amount) {
                amount_removed = cur_amount;
                my401k_balance -= cur_amount;
                sprintf(sndBuf,"%d",my401k_balance);
            } else if (strcmp(cur_account, "my529") == 0 && my529_balance >= cur_amount) {
                amount_removed = cur_amount;
                my529_balance -= cur_amount;
                sprintf(sndBuf,"%d",my529_balance);
            }


            // PUT AMOUNT TO SECOND ACCOUNT
            if (amount_removed > 0) {
                if (strcmp(second_account, "myChecking") == 0) {
                    myChecking_balance += amount_removed;
                } else if (strcmp(second_account, "mySavings") == 0) {
                    mySavings_balance += amount_removed;
                } else if (strcmp(second_account, "myCD") == 0) {
                    myCD_balance += amount_removed;
                } else if (strcmp(second_account, "my401k") == 0) {
                    my401k_balance += amount_removed;
                } else if (strcmp(second_account, "my529") == 0) {
                    my529_balance += amount_removed;
                }
            }
        }


        // printf("Received string: %s \nCommand is: %s \nAccount is: %s \nAmount is: %d\n\n", nameBuf, cur_cmd, cur_account, cur_amount);

    	// /* Return account balance to client */
    	total_bytes_received = send(clientSock, sndBuf, SNDBUFSIZE, 0);
        if (total_bytes_received < 0) {
            printf("send failed.");
            exit(1);
        }

    }

}

