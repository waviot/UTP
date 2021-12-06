//
// Created by bess on 11.11.2021.
//
#include "UtpTool.h"
#include <iostream>


template <typename T>
static void DecodeValue(uint8_t *buffer, T & value){
    value = 0;
    for(int i=0; i<sizeof(T);i++){
        value = value << 8 | buffer[i];
    }
}

template <typename T>
static void EncodeValue(uint8_t *buffer, T & value){
    for(int i=0; i<sizeof(T);i++){
        buffer[i] = (uint8_t)(value >> ((sizeof(T) - 1 - i)*8));
    }
}

WVT_UTP_Allocator_Status_t UtpTool::CreateSpace(uint16_t uid, uint32_t size, uint16_t crc, uint32_t & address) {
    std::vector<uint8_t> cmd = {0,
                                (uint8_t)(size >> 24),
                                (uint8_t)(size >> 16),
                                (uint8_t)(size >> 8),
                                (uint8_t)(size & 0xFF),
                                (uint8_t)(crc >> 8),
                                (uint8_t)(crc & 0xFF),
                                (uint8_t)(uid >> 8),
                                (uint8_t)(uid & 0xFF)};
    auto response = _net->Request(cmd);
    if(response.size()==5){
        DecodeValue(&response.data()[1], address);
    }else{
        if(response.size()==2)return (WVT_UTP_Allocator_Status_t)response[1];
    }
    return WVT_UTP_ALLOC_OK;
}


WVT_UTP_Append_Status_t UtpTool::AppendData(std::vector<uint8_t> & data){
    std::vector<uint8_t> request;
    request.push_back(1);
    request.insert(request.end(), data.begin(), data.end());
    auto response = _net->Request(request);
    if(response.size() == 2){
        return (WVT_UTP_Append_Status_t)response[1];
    }
    return WVT_UTP_APPEND_OK;
}

WVT_UTP_Append_Status_t UtpTool::AppendData(std::vector<uint8_t> & data, int partSize)
{
    std::vector<uint8_t> request;
    int byteCounter = 0;
    while (byteCounter < data.size()){
        int size = data.size() - byteCounter;
        if(size > partSize) size = partSize;
        request.insert(request.begin(), data.begin()+byteCounter, data.begin()+(byteCounter+size));
        auto result = AppendData(request);
        if(result != WVT_UTP_APPEND_OK) return result;
        byteCounter += request.size();
        request.clear();
    }   
    return WVT_UTP_APPEND_OK;
}


WVT_UTP_Freeze_Status_t UtpTool::SaveData(uint16_t & crc){
    std::vector<uint8_t> request;
    request.push_back(2);
    auto response = _net->Request(request);
    if(response.size() == 2){
        std::cout << "Can't save data to current memory space. Code: " << response.at(1) << std::endl;
        return (WVT_UTP_Freeze_Status_t)response[1];
    }else{
        DecodeValue(&response.data()[1], crc);
    }
    return WVT_UTP_FREEZE_OK;
}

uint32_t UtpTool::GetFreeSpace(uint32_t & segmentsCount)
{
    std::vector<uint8_t> request;
    request.push_back(3);
    auto response = _net->Request(request);
    if(response.size() != 9){
        std::cout << "Can't get memory info. Code: " << response.at(1) << std::endl;
    }else{
        uint32_t freeSpace;
        DecodeValue(&response.data()[1], segmentsCount);
        DecodeValue(&response.data()[5], freeSpace);
        return freeSpace;
    }
    return 0;
}

WVT_UTP_GetHeader_Status_t UtpTool::GetHeaderBySerial(uint16_t id, WVT_UTP_Data_Header_t & header){
    header.uid = 0;
    header.start_address = 0;
    header.size = 0;
    header.crc = 0;
    header.crc_of_this_struct = 0;
    std::vector<uint8_t> request;
    request.push_back(4);
    request.push_back(id >> 8);
    request.push_back(id & 0xFF);
    auto response = _net->Request(request);
    if(response.size()==2){
        return (WVT_UTP_GetHeader_Status_t)response[0];
    }
    DecodeValue(&response.data()[1], header.uid);
    DecodeValue(&response.data()[3], header.start_address);
    DecodeValue(&response.data()[7], header.size);
    DecodeValue(&response.data()[11], header.crc);
    DecodeValue(&response.data()[13], header.crc_of_this_struct);
    return WVT_UTP_GH_OK;
}

WVT_UTP_Data_Header_t UtpTool::GetHeaderByUid(uint16_t uid){
    WVT_UTP_Data_Header_t header;
    header.uid = 0;
    header.start_address = 0;
    header.size = 0;
    header.crc = 0;
    header.crc_of_this_struct = 0;
    std::vector<uint8_t> request;
    request.push_back(5);
    request.push_back(uid >> 8);
    request.push_back(uid & 0xFF);
    auto response = _net->Request(request);
    if(response.size() == 2){
        std::cout << "Can't read segment header. Code: " << response.at(1) << std::endl;
    }else{
        DecodeValue(&response.data()[1], header.uid);
        DecodeValue(&response.data()[3], header.start_address);
        DecodeValue(&response.data()[7], header.size);
        DecodeValue(&response.data()[11], header.crc);
        DecodeValue(&response.data()[13], header.crc_of_this_struct);
    }
    return header;
}

std::vector<uint8_t> UtpTool::ReadSegment(uint16_t uid, uint32_t offset, uint32_t length)
{
    std::vector<uint8_t> request(11);
    request[0] = 6;
    EncodeValue(&(request.data()[1]), uid);
    EncodeValue(&(request.data()[3]), offset);
    EncodeValue(&(request.data()[7]), length);
    auto response = _net->Request(request);
    if(response.size() == 2){
        WVT_UTP_Read_Status_t status = (WVT_UTP_Read_Status_t)response[1];
        if(status == WVT_UTP_READ_WRONG_PARAMETERS){
            std::cout << "Check if passed parameters are correct " << std::endl;
        } else if(status == WVT_UTP_READ_SEGMENT_NOT_FOUND){
            std::cout << "No headers found for this UID  " << std::endl;
        } else if(status == WVT_UTP_READ_SIZE_OUT_OF_RANGE){
           std::cout << "Data of specified size cannot be read at the specified address at the specified offset" << std::endl;
        } else if(status == WVT_UTP_READ_HAL_ERROR){
            std::cout << "There was a problem reading at the HAL level " << std::endl;
        }
    }else{
       std::vector<uint8_t> data;
       data.insert(data.begin(), response.begin()+1, response.end());
       return data;
    }
    return {};
}

void UtpTool::EraseSegmentByUid(uint16_t uid){
    std::vector<uint8_t> request;
    request.push_back(7);
    request.push_back(uid >> 8);
    request.push_back(uid & 0xFF);
    auto response = _net->Request(request);
    if(response.size() != 2){
        std::cout << "Can't erase segment" << std::endl;
    }else{
        WVT_UTP_Erase_Status_t status = (WVT_UTP_Erase_Status_t)response[1];
       if(status == WVT_UTP_ERASE_OK){
           std::cout << "Memory segment erased successfully" << std::endl;
       } else if(status == WVT_UTP_ERASE_WRONG_PARAMETERS){
           std::cout << "Invalid request parameters" << std::endl;
       }else if(status == WVT_UTP_ERASE_NOT_FOUND){
           std::cout << "The requested memory segment was not found by UID " << std::endl;
       }
    }
}

void UtpTool::EraseAll(){
    std::vector<uint8_t> request;
    request.push_back(8);
    auto response = _net->Request(request);
    if(response.size() != 2){
        std::cout << "Can't erase" << std::endl;
    }else{
        if(response.data()[1] != 0){
            std::cout << "Can't erase" << std::endl;
        }
    }
}