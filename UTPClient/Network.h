#ifndef UTP_CLIENT_NETWORK_H
#define UTP_CLIENT_NETWORK_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "ITransport.h"

class Network : public ITransport{
public:
    Network(const char *ip, int port) : _ip(ip), _port(port) {};
    virtual std::vector<uint8_t> Request(std::vector<uint8_t> request_data);
private:
    void Send(std::vector<uint8_t> data);
    std::vector<uint8_t> Read(uint32_t length);
    const char *  _ip;
    const int   _port;
};
#endif //UTP_CLIENT_NETWORK_H
