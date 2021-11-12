#ifndef UTP_CLIENT_UTPTOOL_H
#define UTP_CLIENT_UTPTOOL_H
#include <stdint.h>
#include <vector>
#include "ITransport.h"
#include "WVT_UTP_Types.h"

class UtpTool{
public:
    UtpTool(ITransport * net): _net(net){}
    WVT_UTP_Allocator_Status_t CreateSpace(uint16_t uid, uint32_t size, uint16_t crc, uint32_t & startAddress);
    WVT_UTP_Append_Status_t AppendData(std::vector<uint8_t> &data);
    WVT_UTP_Freeze_Status_t SaveData(uint16_t & crc);
    uint32_t GetFreeSpace(uint32_t & segmentsCount);
    WVT_UTP_GetHeader_Status_t GetHeaderBySerial(uint16_t id, WVT_UTP_Data_Header_t & header);
    WVT_UTP_Data_Header_t GetHeaderByUid(uint16_t uid);
    std::vector<uint8_t> ReadSegment(uint16_t uid, uint32_t offset, uint32_t length);
    void EraseSegmentByUid(uint16_t uid);
    void EraseAll();

private:
    ITransport *_net;
};
#endif
