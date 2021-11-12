#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "Network.h"
#include "UtpTool.h"
#include "UTool.h"

void PrintHelp()
{
    std::cout << "Options:" << std::endl;
    std::cout << "-l filename -uid id\t\tUpload file to storage" << std::endl;
    std::cout << "-i\t\t\t\t\tGet memory info" << std::endl;
    std::cout << "-ea\t\t\t\t\tErase all" << std::endl;
    std::cout << "-eu uid\t\t\t\tErase by uid" << std::endl;
    std::cout << "-r uid -s size -f filename\t\t\t\tExport memory to file" << std::endl;
}

void PrintMemoryInfo(){
    Network net("127.0.0.1", 8000);
    UtpTool utp(&net);
    uint32_t count;
    auto freeSpace = utp.GetFreeSpace(count);
    std::cout << "Segments count: " << count << std::endl;
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
                      << "|0x" << std::hex << header.crc_of_this_struct << "      |" << std::endl;
            std::cout << "=======================================================";
            std::cout << "=======================" << std::endl;
        }
    }
}

void FullEraseMemory()
{
    Network net("127.0.0.1", 8000);
    UtpTool utp(&net);
    utp.EraseAll();
    std::cout << "Memory full erasing" << std::endl;
}

void EraseMemorySegment(char *uidString)
{
    Network net("127.0.0.1", 8000);
    UtpTool utp(&net);
    std::stringstream strValue;
    strValue << uidString;
    uint16_t uid;
    strValue >> uid;
    utp.EraseSegmentByUid(uid);
}

void Upload(char *file, char *uidString){
    std::cout << "File: " << file << std::endl;
    std::ifstream fin(file);

    std::stringstream strValue;
    strValue << uidString;
    uint16_t uid;
    strValue >> uid;

    fin.seekg(0, std::ios::end);
    size_t length = fin.tellg();
    fin.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(length);
    fin.read(reinterpret_cast<char *>(data.data()), length);
    std::cout << "\tFile size: 0x" << std::hex << data.size() << std::endl;
    auto crc_value = GetCrc16OfVector(data);
    std::cout << "\tData CRC16: 0x" << std::hex << crc_value << std::endl;

    Network net("127.0.0.1", 8000);
    UtpTool utp(&net);

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
        auto appendResult = utp.AppendData( data);
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

void ExportMemoryToFile(char *filename, char *uidString, char *sizeString)
{
    Network net("127.0.0.1", 8000);
    UtpTool utp(&net);
    std::stringstream strValue;
    strValue << uidString;
    uint16_t uid;
    strValue >> uid;
    strValue.clear();
    strValue << sizeString;
    uint16_t size;
    strValue >> size;
    auto response = utp.ReadSegment(uid, 0, size);
    std::ofstream file;
    file.open(filename);
    /*for(auto v : response){
        file<<v;
    }*/
    file.write(reinterpret_cast<const char *>(response.data()), response.size());
    file.close();
    std::cout << "Reading memory segment by UID " << uid << std::endl;
}
int main(int argc, char * argv[]) {
    if(CmdOptionExists(argv, argv+argc, "-h"))
    {
        PrintHelp();
    }
    if(CmdOptionExists(argv, argv+argc, "-l"))
    {
        Upload(GetCmdOption(argv, argv + argc, "-l"),
               GetCmdOption(argv, argv + argc, "-uid"));
    }
    if(CmdOptionExists(argv, argv+argc, "-i"))
    {
        PrintMemoryInfo();
    }
    if(CmdOptionExists(argv, argv+argc, "-ea"))
    {
        FullEraseMemory();
    }
    if(CmdOptionExists(argv, argv+argc, "-eu"))
    {
        EraseMemorySegment(GetCmdOption(argv, argv + argc, "-eu"));
    }
    if(CmdOptionExists(argv, argv+argc, "-r"))
    {
        ExportMemoryToFile(GetCmdOption(argv, argv + argc, "-f"),
                           GetCmdOption(argv, argv + argc, "-r"),
                           GetCmdOption(argv, argv + argc, "-s"));
    }
}
