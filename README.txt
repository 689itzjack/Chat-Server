README
Author: Ytzhak Ariel Armoni 

Chatserver

===Description ===

Program files:
chatserver.c- file that implement a chatserver and its Actions. Implementation of a simple chat server using TCP and select().
The server waits for clients’ request. Once the server reads a message from the client, it reads it till a new line appears. 
The server can talk with many clients, each on a different socket.
The server gets a message from the client and send it to all clients except the one who sends it.
The server assigns names to each client, the name is ‘guest<sd>’ where sd is the socket descriptor assigned to this client after ‘accept()’ returns. 
F/e: If client guest3 writes the message: “hello everyone\r\n”, you should print to all other clients:
guest3: hello everyone\r\n

Running:

The server should be run like this:
./server <port> when the port must to be a positive number.

Funcation:

typedef struct node_chat // its a node of the messages, its what make the queue.

int* cleanArray(int* arr,int arrSIZE)// this function clean every past message that was writed to the client.

char* messageResponse(char* mess,int fd)//this method its the output for the another clients. in other words its the message to the clients.

int* cleanArray(int* arr,int arrSIZE)// this function clean the array of file descriptors from garbage values cause it will 
                                      //contains the file descriptors from the program and clients.

void handler (int signum) //this is the handle function that frees the memory allocated by the program when the signal ^C occurres and finish it.

   

