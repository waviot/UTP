#ifndef WVT_UTP_H
#define WVT_UTP_H
#include <stdint.h>
#include "WVT_UTP_Types.h"

typedef enum
{
    WVT_UTP_CMD_CREATE_SPACE            = 0x00U,
    WVT_UTP_CMD_APPEND_DATA             = 0x01U,
    WVT_UTP_CMD_SAVE_DATA               = 0x02U,
    WVT_UTP_CMD_GET_SEGMENT_COUNT       = 0x03U,
    WVT_UTP_CMD_GET_SEGMENT_INFO        = 0x04U,
    WVT_UTP_CMD_GET_SEGMENT_INFO_BY_UID = 0x05U,
    WVT_UTP_CMD_READ_SEGMENT            = 0x06U,
    WVT_UTP_CMD_ERASE_SEGMENT           = 0x07U,
    WVT_UTP_CMD_ERASE_ALL               = 0x08U,
} WVT_UTP_Cmd_t;

void WVT_UTP_init(WVT_UTP_Init_t init_struct);
void WVT_UTP_packet_handler(uint8_t *request, uint32_t request_length,
                        uint8_t *response, uint32_t *response_length);

#endif
