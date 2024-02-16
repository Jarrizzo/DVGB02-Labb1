#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

#define PORTNUMBER 8081
#define BUFFERSIZE 4096

void handle_TCP_server(int client_Socket, const char *file_name,const char *file_type);

int main(int argc, char *argv[]){

    char Buffer[BUFFERSIZE];

    //Creating Socket
    int servSocket;
    if((servSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0){
        perror("-- Could not create Socket\n");
        exit(1);
    }
    else
        printf("-- Socket is now created\n");

    // Construct local address structure
    struct sockaddr_in serverAdress;                                // Local address
    memset(&serverAdress, 0, sizeof(serverAdress));                 // Zero out structure
    serverAdress.sin_family = AF_INET;                              // IPv4 address family
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);               // Any incoming interface
    serverAdress.sin_port = htons(PORTNUMBER);                      // Local port


    //Bind to local address
    if(bind(servSocket, (struct sockaddr*) &serverAdress, sizeof(serverAdress)) < 0){
        perror("-- Could not bind to local address\n");
        exit(1);
    }
    else
        printf("-- Server address is now bound to socket\n");

    //Make socket listen for incomming connection
    if(listen(servSocket, SOMAXCONN) < 0){
        perror("-- Could not establish listen function\n");
        exit(1);
    }
    else
        printf("-- Server is now listening for connections\n");

    while(true){
        //Client address
        struct sockaddr_in clientAddr; 
        int clientAddrLength = sizeof(clientAddr);

        //wait for client to connect
        int clientSocket = accept(servSocket, (struct sockaddr *)&clientAddr, &clientAddrLength);
        if(clientSocket < 0){
            perror("Error when connecting");
            exit(1);
        }
        else
            printf("-- Client is connected\n");

        //Reading data from socket
        read(clientSocket,Buffer,BUFFERSIZE);

        //printf("\n\n%s\n\n",Buffer);

        char * request = strtok(Buffer, " ");
        if (!request) {
            perror("-- Invalid request\n");
            exit(1);
        }
        //Extracting file path
        char *file = strtok(NULL, " ");
        if (!file) {
            perror("-- Invalid file name\n");
            exit(1);
        }

        //Copying "file" to be able to extract content_type
        memmove(file,file+1,strlen(file));

        char *file_copy = malloc(strlen(file) + 1);
        strcpy(file_copy, file);
        char *tmp = malloc(strlen(file_copy) + 1);
        strcpy(tmp, file_copy);

        //Extracting content type
        char * content_type = strtok(tmp, ".");
        content_type = strtok(NULL, " ");
        if (!content_type) {
            perror("-- Invalid type");
            exit(1);
        }

        //printf("\nThis is file: %s\nThis is content_type: %s\n", file,content_type);

        //Method to handle the request of the client
        if(content_type)
            handle_TCP_server(clientSocket,file,content_type);
        

        close(clientSocket);
        free(tmp);
        free(file_copy);
    }

    close(servSocket);
    printf("-- Sockets are now closed\n");
}

void handle_TCP_server(int client_Socket, const char *file_name, const char *content_type) {
    FILE *f = fopen(file_name, "rb");
    
    printf("-- Client is now beeing handled\n");

    if (f) {

        fseek(f, 0, SEEK_END);
        long fileLength = ftell(f);
        fseek(f, 0, SEEK_SET); // seek back to beginning of file

        //Sends formatted data to the client socket
        dprintf(client_Socket, "HTTP/1.1 200 OK\r\nServer: Demo Web Server\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n",fileLength, content_type);
        //dprintf(client_Socket, " 200 OK\r\nServer: A Web Server\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", fileLength, content_type);
       
        char buffer[BUFFERSIZE];
        size_t number_of_bytes;

        //Streams content of the file to the client
        while ((number_of_bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
            write(client_Socket, buffer, number_of_bytes);
        }

    } 
    else{
        //In case of failiur, sends the 404NotFound message to the client socket
        dprintf(client_Socket, "HTTP/1.1 404 Not Found\r\nServer: Demo Web Server\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n404 Not Found", (int)strlen("404 Not Found"));
        }
    
    fclose(f);
}