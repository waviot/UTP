#include "WVT_UTP.h"
#include "WVT_UTP_Memory.h"
#include <stdio.h>
WVT_UTP_Init_t utp_struct;

WVT_UTP_Data_Header_t current_segment;
uint32_t received_data_counter = 0;

void (* wvt_utp_handlers[9])(uint8_t *request, uint32_t request_length,
                             uint8_t *response, uint32_t *response_length);

static void WVT_UTP_uint32_to_array(uint8_t *arr, uint32_t val){
    arr[0] = val >> 24;
    arr[1] = val >> 16;
    arr[2] = val >> 8;
    arr[3] = val & 0xFF;
}

static void WVT_UTP_array_to_uint32(uint8_t *arr, uint32_t *val){
    *val = 0;
    *val  = arr[0] << 24;
    *val |= arr[1] << 16;
    *val |= arr[2] << 8;
    *val |= arr[3] << 0;
}

static void WVT_UTP_array_to_uint16(uint8_t *arr, uint16_t *val){
    *val = 0;
    *val |= arr[0] << 8;
    *val |= arr[1] << 0;
}

static void WVT_UTP_uint16_to_array(uint8_t *arr, uint16_t val){
    arr[0] = val >> 8;
    arr[1] = val & 0xFF;
}

/*!
 * Обработка входящих пакетов
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_packet_handler(uint8_t *request, uint32_t request_length,
                        uint8_t *response, uint32_t *response_length)
{
    wvt_utp_handlers[ request[0] ](&request[1], request_length-1, &response[1], response_length);
    response[0] = request[0];
    (*response_length)++;
}

/*!
 * Обработка запроса на выделение сегмента памяти под новые данные
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_create_space_handler(uint8_t *request, uint32_t request_length,
                            uint8_t *response, uint32_t *response_length)
{
    WVT_UTP_array_to_uint32(&request[0], &(current_segment.size));
    WVT_UTP_array_to_uint16(&request[4], &(current_segment.crc));
    WVT_UTP_array_to_uint16(&request[6], &(current_segment.uid));

    if(WVT_UTP_Memory_reserve_sector(&current_segment) == WVT_UTP_OK){
        received_data_counter = 0;
        WVT_UTP_uint32_to_array(&response[0], current_segment.start_address);
        *response_length = 4;
    }else{
        response[0] = WVT_UTP_ERROR;
        *response_length = 1;
    }
}

/*!
 * Обработка запроса на запись данные в текущий сегмент памяти
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_append_data_handler(uint8_t *request, uint32_t request_length,
                                  uint8_t *response, uint32_t *response_length)
{
    if(current_segment.size == 0 && current_segment.start_address == 0){
        response[0] = WVT_UTP_ERROR;
        *response_length = 1;
    }else{
        WVT_UTP_Memory_write(&current_segment, received_data_counter, request, request_length);
        received_data_counter += request_length;
        *response_length = -1;
    }
}

/*!
 * Обработка запроса на закрытие записи данных
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_save_data_handler(uint8_t *request, uint32_t request_length,
                                  uint8_t *response, uint32_t *response_length)
{
    if(WVT_UTP_Memory_freeze(&current_segment) == WVT_UTP_OK){
        WVT_UTP_uint32_to_array(&response[0], current_segment.crc);
        WVT_UTP_uint32_to_array(&response[4], current_segment.crc_of_this_struct);
        *response_length = 8;
        current_segment.size = 0;
        current_segment.start_address = 0;
    }else{
        response[0] = WVT_UTP_ERROR;
        *response_length = 1;
    }
}

/*!
 * Обработка запроса на количество сегментов
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_segment_count_handler(uint8_t *request, uint32_t request_length,
                               uint8_t *response, uint32_t *response_length)
{
    WVT_UTP_uint32_to_array(&response[0], WVT_UTP_Memory_get_segments_count());
    WVT_UTP_uint32_to_array(&response[4], WVT_UTP_Memory_get_free_space());
    *response_length = 8;
}

/*!
 * Обработка запроса на информацию о сегменте памяти по порядковому номеру
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_get_segment_handler(uint8_t *request, uint32_t request_length,
                                   uint8_t *response, uint32_t *response_length)
{
    uint16_t segment_id = 0;
    *response_length = 0;
    WVT_UTP_array_to_uint16(request, &segment_id);
    WVT_UTP_Data_Header_t header;
    WVT_UTP_Memory_get_header_by_serial_number(&header, (int)segment_id);

    WVT_UTP_uint16_to_array(&response[*response_length], header.uid);
    *response_length += sizeof(header.uid);
    WVT_UTP_uint32_to_array(&response[*response_length], header.start_address);
    *response_length += sizeof(header.start_address);
    WVT_UTP_uint32_to_array(&response[*response_length], header.size);
    *response_length += sizeof(header.size);
    WVT_UTP_uint16_to_array(&response[*response_length], header.crc);
    *response_length += sizeof(header.crc);
    WVT_UTP_uint16_to_array(&response[*response_length], header.crc_of_this_struct);
    *response_length += sizeof(header.crc_of_this_struct);
}

/*!
 * Обработка запроса на информацию о сегменте памяти по уникальному номеру
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_get_segment_by_uid_handler(uint8_t *request, uint32_t request_length,
                                 uint8_t *response, uint32_t *response_length)
{
    uint16_t segment_uid = 0;
    *response_length = 0;
    WVT_UTP_array_to_uint16(request, &segment_uid);
    WVT_UTP_Data_Header_t header;
    if(WVT_UTP_Memory_find_by_uid(&header, segment_uid) == WVT_UTP_OK) {
        WVT_UTP_uint16_to_array(&response[*response_length], header.uid);
        *response_length += sizeof(header.uid);
        WVT_UTP_uint32_to_array(&response[*response_length], header.start_address);
        *response_length += sizeof(header.start_address);
        WVT_UTP_uint32_to_array(&response[*response_length], header.size);
        *response_length += sizeof(header.size);
        WVT_UTP_uint16_to_array(&response[*response_length], header.crc);
        *response_length += sizeof(header.crc);
        WVT_UTP_uint16_to_array(&response[*response_length], header.crc_of_this_struct);
        *response_length += sizeof(header.crc_of_this_struct);
    }else{
        response[0] = WVT_UTP_ERROR;
        *response_length = 1;
    }
}

/*!
 * Обработка запроса на информацию о сегменте памяти по порядковому номеру
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_erase_segment_handler(uint8_t *request, uint32_t request_length,
                                 uint8_t *response, uint32_t *response_length)
{
    uint16_t segment_uid = 0;
    WVT_UTP_array_to_uint16(request, &segment_uid);
    response[0] = (uint16_t) WVT_UTP_Memory_free_by_uid(segment_uid);
    *response_length = 1;
}

/*!
 * Обработка запроса на полную очистку памяти хранилища
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_erase_all_handler(uint8_t *request, uint32_t request_length,
                                   uint8_t *response, uint32_t *response_length)
{
    response[0] = (uint16_t) WVT_UTP_Memory_erase_all();
    *response_length = 1;
}

/*!
 * Обработка запроса на чтение сегмента по уникальному идентфикатору
 * @param request массив входящих байт
 * @param request_length длинна входящих данных
 * @param response массив выходных данных
 * @param response_length длинна выходных данных
 */
void WVT_UTP_read_handler(uint8_t *request, uint32_t request_length,
                               uint8_t *response, uint32_t *response_length)
{
    uint16_t segment_uid = 0;
    uint32_t offset = 0;
    uint32_t length = 0;
    WVT_UTP_array_to_uint16(&request[0], &segment_uid);
    WVT_UTP_array_to_uint32(&request[2], &offset);
    WVT_UTP_array_to_uint32(&request[6], &length);
    if(WVT_UTP_Memory_read_by_uid(segment_uid, offset, length, response)==WVT_UTP_OK){
        *response_length = length;
    }else{
        response[0] = WVT_UTP_ERROR;
        *response_length = 1;
    }
}

/*!
 * Инициализация UTP
 * @param init_struct структура с колбэками и параметрами работы
 */
void WVT_UTP_init(WVT_UTP_Init_t init_struct)
{
    //printf("size %ld", sizeof(wvt_utp_handlers));
    utp_struct =  init_struct;
    utp_struct.init(utp_struct.memory_size);

    current_segment.size = 0;
    current_segment.start_address = 0;

    wvt_utp_handlers[WVT_UTP_CMD_CREATE_SPACE]              = &WVT_UTP_create_space_handler;
    wvt_utp_handlers[WVT_UTP_CMD_APPEND_DATA]               = &WVT_UTP_append_data_handler;
    wvt_utp_handlers[WVT_UTP_CMD_SAVE_DATA]                 = &WVT_UTP_save_data_handler;
    wvt_utp_handlers[WVT_UTP_CMD_GET_SEGMENT_COUNT]         = &WVT_UTP_segment_count_handler;
    wvt_utp_handlers[WVT_UTP_CMD_GET_SEGMENT_INFO]          = &WVT_UTP_get_segment_handler;
    wvt_utp_handlers[WVT_UTP_CMD_GET_SEGMENT_INFO_BY_UID]   = &WVT_UTP_get_segment_by_uid_handler;
    wvt_utp_handlers[WVT_UTP_CMD_ERASE_SEGMENT]             = &WVT_UTP_erase_segment_handler;
    wvt_utp_handlers[WVT_UTP_CMD_ERASE_ALL]                 = &WVT_UTP_erase_all_handler;
    wvt_utp_handlers[WVT_UTP_CMD_READ_SEGMENT]              = &WVT_UTP_read_handler;
}


