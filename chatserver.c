#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    // for read/write/close
#include <sys/types.h> /* standard system types*/
#include <sys/stat.h>
#include <netinet/in.h> /* Internet address structures */
#include <sys/socket.h> /* socket interface functions  */
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/select.h>
#include <signal.h>
#define LINE 4096

typedef struct node_chat{
      char *messagge;  //the threads process function
      int file_descriptor;  //argument to the function
      struct node_chat* next;  
} node_ch;

int* cleanArray(int* arr,int arrSIZE){// this function clean the array of file descriptors from garbage values cause it will 
                                      //contains the file descriptors from the program and clients.
    int i;
    for(i=0;i<arrSIZE;i++){
        arr[i]=0;
    }
    return arr;
}

char* messageResponse(char* mess,int fd){//this method its the output for the another clients. in other words its the message to the clients.
    char* send=NULL;
    char message[LINE];
     int i;
    for(i=0;i<LINE;i++){
        message[i]='\0';
    }
    char num[100];
    num[0]='\0';
    strcat(message,"guest");
    sprintf(num, "%d", fd);
    strcat(message,num);
    strcat(message,": ");
    strcat(message,mess);
    send=message;
    return send;
}

void cleanMessages(char* message){// this function clean every past message that was writed to the client.
    for(int i=0;i<LINE;i++){
        message[i]='\0';
    }
}
int* arrayFD=NULL;
node_ch* head=NULL;
node_ch* tail=NULL;
fd_set list_fileDescriptors;
fd_set temp_list_reads;
fd_set temp_list_writes;

void handler (int signum) {//this is the handle function that frees the memory allocated by the program when the signal ^C occurres and finish it.

    for(int i=0;i<getdtablesize();i++){
        if(arrayFD[i]!=0){
            close(arrayFD[i]);
            FD_CLR(arrayFD[i],&temp_list_reads);
        }
    }
    free(arrayFD);
    arrayFD=NULL;
    exit(EXIT_SUCCESS);
} 

//////////////////////////////////////////////////////////////MAINMAINMAINMAINMAINMAIN///////////////////////////////////////////////////
int main(int argc, char const *argv[]){
    
    struct sockaddr_in srv;	
    int port=atoi(argv[1]);
    int number_of_Tasks;
    int newSocket;
    
    int fd;		// WELCOME socket descriptor 
    int checker;
    char message[LINE];
    arrayFD=(int*) malloc(getdtablesize()*sizeof(int));
    if(arrayFD==NULL){
        perror("Error: System Call malloc() Failed !!!\n");	
        exit(EXIT_FAILURE);
    }
    arrayFD=cleanArray(arrayFD,getdtablesize());

    if((port < 0)){
        fprintf(stderr,"The port is not avalible\n");
        exit(EXIT_FAILURE);
    }

    if((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error: System Call socket() Failed !!!\n");
        exit(EXIT_FAILURE);
    }
    arrayFD[fd]=fd;
    srv.sin_family = AF_INET; 
    srv.sin_port = htons(port); 
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0) {//this is the initialization bind()
        perror("Error: System Call bind() Failed !!!\n");
        exit(EXIT_FAILURE);
    }
    if(listen(fd, 5) < 0) {
	    perror("Error: System Call listen() Failed !!!\n");
	    exit(EXIT_FAILURE);
    }
/////////////////////////////////////////part of the chat//////////////////////////////////    
    FD_ZERO((&list_fileDescriptors));
    FD_SET(fd,&list_fileDescriptors);
    int arrIndx=0;
    while(1){
        signal(SIGINT,handler);
        temp_list_reads=list_fileDescriptors;
        temp_list_writes=list_fileDescriptors;

        if(head==NULL){
            FD_ZERO(&temp_list_writes);
        }
        checker=select(getdtablesize(),&temp_list_reads,&temp_list_writes,NULL,NULL);
        if(FD_ISSET(fd,&temp_list_reads)){//this part we accept all the file descriptors froom the program, everybody comes to the welcome socket

            newSocket=accept(fd,NULL, NULL);
            arrayFD[newSocket]=newSocket;
            
            if(newSocket < 0) {
	            perror("Error: System Call accept() Failed !!!\n");	
                exit(EXIT_FAILURE);
            }
            if(newSocket>0){
                FD_SET(newSocket,&list_fileDescriptors);
            }
        }
        int fd_tem;
        for(fd_tem=fd+1;fd_tem<getdtablesize();fd_tem++){
            
            if(FD_ISSET(fd_tem,&temp_list_reads)){
                printf("server is ready to read from socket %d\n",fd_tem);
                cleanMessages(message);
                checker=read(fd_tem,message,LINE);
                
                if(checker==0){
                    close(fd_tem);
                    FD_CLR(fd_tem,&temp_list_reads);
                    arrayFD[fd_tem]=0;
                    
                }
                if(checker > 0){
                   
                    if(head==NULL){
                        head=(node_ch*)malloc(sizeof(node_ch));
                        if(head==NULL){
                            perror("Error: System Call malloc() Failed !!!\n");	
                            exit(EXIT_FAILURE);
                        }
                        head->file_descriptor=fd_tem;
                        head->messagge=message;
                        head->next=NULL;
                        tail=head;
                    }
                    else{
                        node_ch* adding=(node_ch*)malloc(sizeof(node_ch));
                        if(adding==NULL){
                            perror("Error: System Call malloc() Failed !!!\n");	
                            exit(EXIT_FAILURE);
                        }
                        adding->file_descriptor=fd_tem;
                        adding->messagge=message;
                        adding->next=NULL;
                        tail->next=adding;
                        tail=adding;
                    }
                }
                if(checker < 0){
                    perror("Error: System Call read() Failed !!!\n");	
                    exit(EXIT_FAILURE);
                }
            }
        }
        if(FD_ISSET(fd+1,&temp_list_writes)){//if the file descriptors are ready for writte
            char* answer=NULL;
            int writting;
            for(fd_tem=fd+1;fd_tem<getdtablesize();fd_tem++){
                if(FD_ISSET(fd_tem,&temp_list_writes)){
                    if(fd_tem==head->file_descriptor){
                        continue;
                    }
                    printf("server is ready to write to socket %d\n",fd_tem);
                    answer=messageResponse(head->messagge,head->file_descriptor);
                    writting=write(fd_tem,answer,strlen(answer));
                    if(writting < 0){
                        perror("Error: System Call write() Failed !!!\n");	
                        exit(EXIT_FAILURE);
                    }
                }
            }
            node_ch* temp=head;
            head=head->next;
            free(temp);
            temp=NULL;
        }
    }
    return 0;
}