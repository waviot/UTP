#ifndef UTP_WVT_UTP_MEMORY_H
#define UTP_WVT_UTP_MEMORY_H
#include "WVT_UTP_Types.h"

int              WVT_UTP_Memory_get_segments_count(void);
uint32_t         WVT_UTP_Memory_get_free_space(void);
WVT_UTP_Status_t WVT_UTP_Memory_erase_all();
WVT_UTP_Status_t WVT_UTP_Memory_free_by_uid(uint16_t uid);
WVT_UTP_Status_t WVT_UTP_Memory_read_by_uid(uint16_t uid, uint32_t offset, uint32_t length, uint8_t *buffer);
WVT_UTP_Status_t WVT_UTP_Memory_find_by_uid(WVT_UTP_Data_Header_t *header, uint16_t uid);
WVT_UTP_Status_t WVT_UTP_Memory_freeze(WVT_UTP_Data_Header_t *header);
WVT_UTP_Status_t WVT_UTP_Memory_get_header_by_serial_number(WVT_UTP_Data_Header_t *header, uint16_t sn);
WVT_UTP_Status_t WVT_UTP_Memory_reserve_sector(WVT_UTP_Data_Header_t *header);
WVT_UTP_Status_t WVT_UTP_Memory_write(WVT_UTP_Data_Header_t *header, uint32_t offset,
                                      uint8_t *buffer, uint32_t length);
#endif
