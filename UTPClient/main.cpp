#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "Network.h"
#include "UtpTool.h"
#include "UTool.h"
#include "SerialPort.h"
#include "SerialCommunication.h"

void PrintMemoryInfo(ITransport * transport){
    UtpTool utp(transport);
    uint32_t count;
    auto freeSpace = utp.GetFreeSpace(count);
    std::cout << "Segments count: " << std::dec << count << std::endl;
    std::cout << "Free: 0x" << std::hex<< freeSpace << std::endl;
    if(count>0){
        std::cout << "_______________________________________________________";
        std::cout << "_______________________" << std::endl;
        std::cout << std::left << std::setw(16) << "|   UID" << std::setw(16) << "|Address"
                  << std::setw(16) << "|Size" << std::setw(16) << "|Data CRC16" << "|Struct CRC16|\n";
        std::cout << "=======================================================";
        std::cout << "=======================" << std::endl;
    }
    for(uint16_t i=0;i<count;i++){
        WVT_UTP_Data_Header_t header;
        auto status = utp.GetHeaderBySerial(i, header);
        if(status == WVT_UTP_GH_OK) {
            std::cout << std::left
                      << "|   " << std::setw(12) << std::dec << header.uid
                      << "|0x" << std::setw(13) << std::hex << header.start_address
                      << "|0x" << std::setw(13) << std::hex << header.size
                      << "|0x" << std::setw(13) << std::hex << header.crc
                      << "|0x" << std::setw(4)<< std::hex << header.crc_of_this_struct << "      |" << std::endl;
            std::cout << "=======================================================";
            std::cout << "=======================" << std::endl;
        }
    }
}

void FullEraseMemory(ITransport * transport)
{
    UtpTool utp(transport);
    utp.EraseAll();
    std::cout << "Memory full erasing" << std::endl;
}

void EraseMemorySegment(ITransport * transport, char *uidString)
{
    UtpTool utp(transport);
    std::stringstream strValue;
    strValue << uidString;
    uint16_t uid;
    strValue >> uid;
    utp.EraseSegmentByUid(uid);
}

void Upload(ITransport * transport, char *file, uint16_t uid){
    std::cout << "File: " << file << std::endl;
    std::ifstream fin(file);

    fin.seekg(0, std::ios::end);
    size_t length = fin.tellg();
    fin.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(length);
    fin.read(reinterpret_cast<char *>(data.data()), length);
    std::cout << "\tFile size: 0x" << std::hex << data.size() << std::endl;
    auto crc_value = GetCrc16OfVector(data);
    std::cout << "\tData CRC16: 0x" << std::hex << crc_value << std::endl;
    UtpTool utp(transport);

    uint32_t  start_address;
    auto result = utp.CreateSpace(uid, data.size(), crc_value, start_address);
    if(result == WVT_UTP_ALLOC_UNKNOWN_ERROR){
        std::cout << "\tAn unknown error has occurred" << std::endl;
    } else if(result == WVT_UTP_ALLOC_NOT_ENOUGH_PARAMETERS){
        std::cout << "\tInsufficient number of parameters to process the request" << std::endl;
    } else if(result == WVT_UTP_ALLOC_NOT_ENOUGH_MEMORY){
        std::cout << "\tInsufficient memory on device" << std::endl;
    } else if(result == WVT_UTP_ALLOC_UID_ALREADY_EXISTS){
        std::cout << "\tThe same UID already exists. the parameter must be unique " << std::endl;
    }else if(result == WVT_UTP_ALLOC_OK){
        auto appendResult = utp.AppendData(data, 64);
        if(appendResult == WVT_UTP_APPEND_WRONG_PARAMETERS){
            std::cout << "\tError. Perhaps the length of the transmitted data has exceeded the permissible value" << std::endl;
        }else if(appendResult == WVT_UTP_APPEND_NOT_ENOUGH_MEMORY){
            std::cout << "\tMore data has been transferred than the remaining free memory in the current segment" << std::endl;
        }else{
            uint16_t crc;
            auto saveResult = utp.SaveData(crc);
            if(saveResult == WVT_UTP_FREEZE_CRC_ERROR){
                std::cout << "\tData CRC does not match the CRC in the data header" << std::endl;
            }
        }
    }
}



void ExportMemoryToFile(ITransport * transport, char *filename, uint16_t uid, uint32_t size)
{
    UtpTool utp(transport);
    auto response = utp.ReadSegment(uid, 0, size);
    std::ofstream file;
    file.open(filename);
    file.write(reinterpret_cast<const char *>(response.data()), response.size());
    file.close();
    std::cout << "Reading memory segment by UID " << uid << std::endl;
}

void ExportMemoryToFile(ITransport * transport, char *filename, uint16_t uid)
{
    UtpTool utp(transport);
    auto header = utp.GetHeaderByUid(uid);
    ExportMemoryToFile(transport, filename, uid, header.size);
}

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

void Test(ITransport * transport)
{  
    srand(time(0));
    int filesCount = rand() % 100 + 3;
    FullEraseMemory(transport);
    for(int i=0;i<filesCount;i++)
    {
        char filename[64];
        sprintf(filename,"../../content/input_%d.txt", i);       
        std::ofstream out(filename);
        out << random_string(rand() % 512);
        out.close();
        Upload(transport, filename, i);
    }

    for(int i=0;i<filesCount;i++)
    {
        char filename[64];
        sprintf(filename,"../../content/output_%d.txt", i); 
        ExportMemoryToFile(transport, filename, i);
    }  
    PrintMemoryInfo(transport);
}

int main(int argc, char * argv[]) {
    if(CmdOptionExists(argv, argv+argc, "-serial"))
    {
        ITransport * transport;
        auto detectedDevices = Serial::GetSerialPort();
        for(auto & device : detectedDevices){
            if(device.driver == "cp210x")
            {
                std::cout << device << std::endl;
                transport = new SerialCommunication(device);
                Test(transport); 
                free(transport);
            }
        }
         
    }
    if(CmdOptionExists(argv, argv+argc, "-tcp"))
    {
        ITransport * transport = new Network("127.0.0.1", 8000);
        Test(transport); 
    }
   
}
