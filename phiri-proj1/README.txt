/*///////////////////////////////////////////////////////////
*
* FILE:		README.txt
* AUTHOR:	Kee-Bum Philip Phiri 
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////



Run make file

Start Server:
	default: (run server on local ip/port 127.0.0.1/1234):  ./changeServer
	optional (run server with desired ip/port) :./changeServer 127.0.0.1 8888    


Start Client
	./client CMD [testaccount] [amount] ipAddr port
	CMD is one of BAL|WITHDRAW|TRANSFER


You are only allowed 3 withdraws withing a 60 second window. Failed withdraws will also be counted in the timing window. 

If withdraw amount > accounts balance or transfer amount > from accounts balance then the transaction will fail. 
