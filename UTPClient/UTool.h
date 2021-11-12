#ifndef UTP_CLIENT_UTOOL_H
#define UTP_CLIENT_UTOOL_H

static uint16_t GetCrc16OfVector(std::vector<uint8_t> & v)
{
    uint16_t crc = 0;
    for (auto item : v) {
        crc  = (crc >> 8) | (crc << 8);
        crc ^= item;
        crc ^= (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0xff) << 5;
    }
    return crc;
}

char* GetCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool CmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

#endif
