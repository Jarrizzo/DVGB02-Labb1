#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>


#define PORT 37 //45 i ocktal

int main(){

    int sockfd;
    struct sockaddr_in server_address;
    uint32_t TIME;
    socklen_t address_size;

    //Creating Socket
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Could not create socket");
        exit(1);
    }

    // Construct local address structure
    memset(&server_address,0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //Sending data to server
    sendto(sockfd,NULL,0,0, (const struct sockaddr *) &server_address, sizeof(server_address));

    //Reciving data from server
    recvfrom(sockfd, &TIME, sizeof(TIME), 0,NULL,NULL);

    //Converting "network byte" to "host byte"
    time_t current_time = ntohl(TIME);

    printf("Current time: %s", ctime(&current_time));

    close(sockfd);

    return 0;
}