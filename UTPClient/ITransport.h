
#ifndef UTP_CLIENT_ITRANSPORT_H
#define UTP_CLIENT_ITRANSPORT_H
class ITransport{
public:
    virtual std::vector<uint8_t> Request(std::vector<uint8_t> request_data) = 0;
};
#endif
