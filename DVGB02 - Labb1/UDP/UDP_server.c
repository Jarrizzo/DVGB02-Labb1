#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 37 //45 i ocktal

int main(){

    int sockfd;
    struct sockaddr_in si_me, si_other;
    time_t current_time;
    uint32_t TIME;
    socklen_t address_size = sizeof(si_other);

        //Creating socket
        if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
            perror("-- Could not create socket\n");
            exit(1);
        }
        printf("-- Socket is now created\n");

        //Construct local address structure
        memset(&si_me, 0, sizeof(si_me));
        si_me.sin_family = AF_INET;
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);
        si_me.sin_port = htons(PORT);

        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        //Binding the socket with the serveraddress
        if(bind(sockfd, (const struct sockaddr *)&si_me, sizeof(si_me)) < 0){
            perror("-- Could not bind socket\n");
            exit(1);
        }
        printf("-- Server address is now bound to socket\n");

    while(1){

        printf("-- Waiting to recive...\n");
        //Recive datagram from client
        recvfrom(sockfd, NULL, 0, 0, (struct sockaddr*)&si_other, &address_size);

        time(&current_time);

        //Converting unsigned integer hostlong from hostbyte to networkbyte
        TIME = htonl((uint32_t)current_time);     

        sendto(sockfd,&TIME, sizeof(TIME), 0, (const struct sockaddr *)&si_other,sizeof(si_other));
    }
    
    close(sockfd);
    
    return 0;
}