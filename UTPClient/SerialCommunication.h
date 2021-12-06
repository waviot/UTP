#ifndef UTP_CLIENT_SERIAL_H
#define UTP_CLIENT_SERIAL_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "SerialPort.h"
#include "ITransport.h"

class SerialCommunication : public ITransport{
public:
    SerialCommunication(Serial::SerialDevice device)
    {
        _port = new Serial::SerialPort(device);
        _port->Open(115200);
    }
    virtual std::vector<uint8_t> Request(std::vector<uint8_t> request_data);
private:
    void Send(std::vector<uint8_t> data);
    std::vector<uint8_t> Read(uint32_t length);
    Serial::SerialPort * _port;
};
#endif //UTP_CLIENT_NETWORK_H
