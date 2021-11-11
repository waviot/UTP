#ifndef UTP_NETWORK_H
#define UTP_NETWORK_H

void network_init(int port);
int  network_accept_client();
void network_close(int socketFd);
void network_io_process(int sockfd,
                        void (*callback)(uint8_t *request_buffer,
                                         uint32_t request_length,
                                         uint8_t *response_buffer,
                                         uint32_t *response_length));

#endif
