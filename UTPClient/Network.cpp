#include "Network.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

int network_tcp_connect(const char *ip, int port)
{
    int sockfd;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ip);
    servaddr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    return (sockfd);
}

void network_send(int sockfd, uint8_t *buffer, uint16_t length)
{
    write(sockfd, buffer, length);
}

int network_read(int sockfd, uint8_t *buffer, int length)
{
    int data_length = 0;
    int n = 0;
    while(data_length<length){
        n = read(sockfd, buffer, length - data_length);
        if(n==-1)break;
        data_length += n;
    }
    return data_length;
}

std::vector<uint8_t> Network::Request(std::vector<uint8_t>  request_data) {
    int socketFd = network_tcp_connect(_ip, _port);
    uint16_t length = request_data.size();
    uint8_t len_arr[2] = {(uint8_t)(length>>8), (uint8_t)(length & 0xFF)};
    network_send(socketFd, len_arr, 2);
    network_send(socketFd, request_data.data(), request_data.size());
    if(network_read(socketFd, len_arr, 2)!=2){
        std::cout << "Cant read response length" << std::endl;
    }else{
        length = len_arr[0] << 8 | len_arr[1];
        std::vector<uint8_t> resp(length);
        if(network_read(socketFd, resp.data(), resp.size())==resp.size())return resp;
    }
    return {};
}
