#include "SerialCommunication.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
std::vector<uint8_t> PrepareData(std::vector<uint8_t> & v)
{
    std::vector<uint8_t> data;
    data.push_back(0xAA); 
    data.push_back(1); 
    for(auto & dataByte : v){
        data.push_back(dataByte); 
        if (dataByte == 0xAA)
        {
            data.push_back(0);
        }
    }
    data.push_back(0xAA); 
    data.push_back(2); 
    return data;
} 

std::vector<uint8_t> SerialCommunication::Request(std::vector<uint8_t>  request_data) {
    const int timeout = 3000;
    auto data = PrepareData(request_data);
    _port->Write(data);
    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    std::vector<uint8_t> response; 
    uint8_t lastByte;
    while(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginTime).count() < timeout)
    {
        uint8_t b;
        if(_port->Read(&b,0,1,1000) == 1){
            if (lastByte == 0xAA)
            {
                if (b == 0) response.push_back(0xAA);
                if (b == 1) response.clear();
                if (b == 2) return response;
            }
            else
            {
                if (b != 0xAA) response.push_back(b);
            }
            lastByte = b;
        }
    }
    return {};
}
