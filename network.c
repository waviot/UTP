#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int _sockfd;

void network_read(int sockfd, uint8_t *buffer, int length)
{
    int data_length = 0;
    while(data_length<length){
        data_length += read(sockfd, &buffer[data_length], length - data_length);
    }
}

void network_init(int port)
{

    struct sockaddr_in servaddr, cli;
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else{
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if ((bind(_sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    } else {
        printf("Socket successfully binded..\n");
    }
    if ((listen(_sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else {
        printf("Server listening..\n");
    }
}
void network_io_process(int sockfd,
                        void (*callback)(uint8_t *request_buffer,
                                uint32_t request_length,
                                uint8_t *response_buffer,
                                uint32_t *response_length))
{
        uint32_t request_length = 0;
        uint8_t b[2];
        network_read(sockfd, b, 2);
        request_length = b[0]<<8|b[1];
        uint8_t request[request_length];
        network_read(sockfd, request, request_length);
        uint32_t response_length = 0;
        uint8_t response[1024];
        callback(request, request_length, response, &response_length);
        b[0] = (uint8_t)(response_length >> 8);
        b[1] = (uint8_t)(response_length & 0xFF);
        write(sockfd, b, 2);
        if(response_length>0) write(sockfd, response, response_length);
}

int network_accept_client()
{
    int connfd, len;
    struct sockaddr_in cli;
    len = sizeof(cli);
    connfd = accept(_sockfd, (struct sockaddr*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    } else {
        //printf("server accept the client...\n");
    }
    return connfd;
}

void network_close(int socketFd) {
    close(socketFd);
}
