#include <stdint.h>
#include <malloc.h>
#include "WVT_UTP_Memory.h"

extern WVT_UTP_Init_t utp_struct;

static WVT_UTP_Status_t WVT_UTP_Memory_left_shift(uint32_t address, uint32_t shift, uint32_t count);
static WVT_UTP_Status_t WVT_UTP_Memory_erase(uint16_t address, uint16_t size);

/*!
 * Обновляет текущее значение CRC16 с учетом новых данных
 * @param crc текущее значени CRC16
 * @param buffer указатель на начало данных
 * @param count количество данных
 * @return обновленное значение значение CRC16
 */
static uint16_t WVT_UTP_Memory_update_crc16(uint16_t crc, uint8_t *buffer, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++) {
        crc  = (crc >> 8) | (crc << 8);
        crc ^= buffer[i];
        crc ^= (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0xff) << 5;
    }
    return crc;
}

/*!
 * ВЫчисляет и обновляет значение crc_of_this_struct в структуре WVT_UTP_Data_Header_t
 * @param header указатель на переменную типа WVT_UTP_Data_Header_t
 */
static void WVT_UTP_Memory_update_header_crc(WVT_UTP_Data_Header_t *header){
    uint16_t crc = 0;
    crc = WVT_UTP_Memory_update_crc16(crc, (uint8_t *)header, sizeof(WVT_UTP_Data_Header_t)-sizeof(header->crc_of_this_struct));
    header->crc_of_this_struct = crc;
}

/*!
 * Вычисляет значение CRC16 сегмента памяти, описанной в WVT_UTP_Data_Header_t
 * @param header Заголовок с описанием сегмента памяти
 * @return CRC16 области памяти, описанной в структуре заголовка
 */
static uint16_t WVT_UTP_Memory_get_segment_crc(WVT_UTP_Data_Header_t *header){
    uint8_t buffer[16];
    uint16_t crc = 0;
    uint32_t sector_size = (header->size);
    uint32_t tail = 0;
    uint32_t bytes_count = 0;
    while(bytes_count != sector_size){
        tail = sector_size - bytes_count;
        if(tail > sizeof(buffer)) tail = sizeof(buffer);
        utp_struct.memory_read(header->start_address+bytes_count, buffer, tail);
        bytes_count += tail;
        crc = WVT_UTP_Memory_update_crc16(crc, buffer, tail);
    }
    return crc;
}

/*!
 * Прочитать структуру описания сегмента памяти по её порядковому номеру
 * @param header Указатель на структуру WVT_UTP_Data_Header_t
 * @param sn Порядковый номер в памяти
 * @return WVT_UTP_GetHeader_Status_t
 */
WVT_UTP_GetHeader_Status_t WVT_UTP_Memory_get_header_by_serial_number(WVT_UTP_Data_Header_t *header, uint16_t sn){
    if(((sn+1)*sizeof (WVT_UTP_Data_Header_t))>utp_struct.memory_size) return WVT_UTP_GH_NOT_FOUND;
    uint16_t crc = 0;
    uint16_t offset = utp_struct.memory_size - sizeof(WVT_UTP_Data_Header_t)*(sn + 1);
    utp_struct.memory_read(offset, (uint8_t *)header, sizeof(WVT_UTP_Data_Header_t));
    crc = WVT_UTP_Memory_update_crc16(crc, (uint8_t *)header, sizeof(WVT_UTP_Data_Header_t)-sizeof(header->crc_of_this_struct));
    if(crc != header->crc_of_this_struct) return WVT_UTP_GH_DATA_BROKEN;
    return WVT_UTP_GH_OK;
}

/*!
 * Записать структуру описания сегмента памяти по её порядковому номеру.
 * @param header Указатель на структуру WVT_UTP_Data_Header_t
 * @param sn Порядковый номер в памяти
 * @return WVT_UTP_Status_t
 */
WVT_UTP_Status_t WVT_UTP_Memory_set_header_by_serial_number(WVT_UTP_Data_Header_t *header, uint16_t sn){
    int offset = utp_struct.memory_size - (sn+1)*sizeof(WVT_UTP_Data_Header_t);
    utp_struct.memory_erase(offset, sizeof(WVT_UTP_Data_Header_t));
    return utp_struct.memory_write(offset, (uint8_t *)header, sizeof(WVT_UTP_Data_Header_t));
}

/*!
 * Получить размер свободной области памяти
 * @return Размер свободной области памяти
 */
uint32_t WVT_UTP_Memory_get_free_space(void){
    uint32_t size = utp_struct.memory_size;
    int count = 0;
    WVT_UTP_Data_Header_t header;
    while(1){
        if(WVT_UTP_Memory_get_header_by_serial_number(&header, count) != WVT_UTP_OK) break;
        size-= sizeof(WVT_UTP_Data_Header_t);
        size-= header.size;
        count++;
    }
    return size;
}

/*!
 * Резервирование памяти хранилища.
 * @param header Описание папаметров сегмента памяти: его стратовый адрес и размер
 * @return WVT_UTP_Allocator_Status_t
 */
WVT_UTP_Allocator_Status_t WVT_UTP_Memory_reserve_sector(WVT_UTP_Data_Header_t *header) {
    //все заголовки располагаем в конец выделенной области памяти
    WVT_UTP_Data_Header_t tmp;
    tmp.start_address = 0;
    tmp.size = 0;
    int count = WVT_UTP_Memory_get_segments_count();
    if(WVT_UTP_Memory_find_by_uid(&tmp, header->uid) == WVT_UTP_OK)return WVT_UTP_ALLOC_UID_ALREADY_EXISTS; //header with uid exists
    if(count>0)WVT_UTP_Memory_get_header_by_serial_number(&tmp, count-1);
    if((header->size+sizeof(WVT_UTP_Data_Header_t))>WVT_UTP_Memory_get_free_space()){
        return WVT_UTP_ALLOC_NOT_ENOUGH_MEMORY;
    }
    header->start_address = tmp.start_address + tmp.size;
    WVT_UTP_Memory_erase(header->start_address, header->size);
    return WVT_UTP_OK;
}

/*!
 * Сохраняем зарезервированную область в памяти
 * @param header Описание области памяти
 * @return WVT_UTP_Freeze_Status_t
 */
WVT_UTP_Freeze_Status_t WVT_UTP_Memory_freeze(WVT_UTP_Data_Header_t *header) {
    int count = WVT_UTP_Memory_get_segments_count();
    int offset = utp_struct.memory_size - (count + 1)*sizeof(WVT_UTP_Data_Header_t);
    if(WVT_UTP_Memory_get_segment_crc(header) == header->crc){
        WVT_UTP_Memory_update_header_crc(header);
        utp_struct.memory_write(offset, (uint8_t *)header, sizeof(WVT_UTP_Data_Header_t));
        return WVT_UTP_FREEZE_OK;
    }
    return WVT_UTP_FREEZE_CRC_ERROR;
}

/*!
 * Возвращает количество вегментов внутри выделенного пространства
 * @return Кличество занятых сегметов памяти
 */
int WVT_UTP_Memory_get_segments_count(void) {
    int count = 0;
    WVT_UTP_Data_Header_t header;
    while(1){
        if(WVT_UTP_Memory_get_header_by_serial_number(&header, count) != WVT_UTP_OK) break;
        count++;
    }
    return count;
}

int WVT_UTP_Memory_find_serial_number(WVT_UTP_Data_Header_t *header)
{
    int count = WVT_UTP_Memory_get_segments_count();
    int sn = -1;
    WVT_UTP_Data_Header_t temp;
    for(int i=0;i<count;i++){
        WVT_UTP_Memory_get_header_by_serial_number(&temp, i);
        if(header->start_address == temp.start_address &&
           header->size == temp.size){
            sn = i;
            break;
        }
    }
    return sn;
}

/*!
 * Освобождает сегмент хранилища
 * @param header Описание удаляемого сегмента хранилища
 * @return WVT_UTP_Erase_Status_t
 */
WVT_UTP_Erase_Status_t WVT_UTP_Memory_free(WVT_UTP_Data_Header_t *header){
    //uint32_t width = header->size;
    int header_sn = WVT_UTP_Memory_find_serial_number(header);
    if(header_sn == -1) return WVT_UTP_ERASE_NOT_FOUND;
    int count = WVT_UTP_Memory_get_segments_count();
    WVT_UTP_Memory_erase(utp_struct.memory_size - (header_sn+1)*sizeof(WVT_UTP_Data_Header_t), sizeof (WVT_UTP_Data_Header_t));
    WVT_UTP_Data_Header_t tmp;
    for(int i=header_sn+1;i<count;i++){
        WVT_UTP_Memory_get_header_by_serial_number(&tmp, i);
        WVT_UTP_Memory_left_shift(tmp.start_address-header->size,header->size,tmp.size);
        tmp.start_address -= header->size;
        WVT_UTP_Memory_update_header_crc(&tmp);
        WVT_UTP_Memory_set_header_by_serial_number(&tmp,i-1);
    }
    WVT_UTP_Memory_erase(utp_struct.memory_size - (count)*sizeof(WVT_UTP_Data_Header_t), sizeof (WVT_UTP_Data_Header_t));
    return WVT_UTP_ERASE_OK;
}

/*!
 * Получить инфармацию о сегменте памяти по его уникальному идентифкатору
 * @param header Указатель на структуру заголовка
 * @param uid Уникальный идентификатор
 * @return WVT_UTP_GetHeader_Status_t
 */
WVT_UTP_GetHeader_Status_t WVT_UTP_Memory_find_by_uid(WVT_UTP_Data_Header_t *header, uint16_t uid)
{
    int count = WVT_UTP_Memory_get_segments_count();
    for(int i=0;i<count;i++){
        WVT_UTP_Memory_get_header_by_serial_number(header, i);
        if(header->uid == uid){
            return WVT_UTP_GH_OK;
        }
    }
    header->size = 0;
    header->start_address = 0;
    return WVT_UTP_GH_NOT_FOUND;
}

/*!
 * Освобождает сегмент хранилища по UID
 * @param header Описание удаляемого сегмента хранилища
 * @return WWVT_UTP_Erase_Status_t
 */
WVT_UTP_Erase_Status_t WVT_UTP_Memory_erase_by_uid(uint16_t uid) {
    WVT_UTP_Data_Header_t temp;
    if(WVT_UTP_Memory_find_by_uid(&temp, uid)==WVT_UTP_GH_OK) {
        return WVT_UTP_Memory_free(&temp);
    }
    return WVT_UTP_ERASE_NOT_FOUND;
}

/*!
 * Запись данных в выделенный сегмент памяти
 * @param header Описание выделенной области
 * @param offset Смещение относительно начала сегмента
 * @param buffer Данные для записи
 * @param length Длина данных
 * @return WVT_UTP_Append_Status_t
 */
WVT_UTP_Append_Status_t WVT_UTP_Memory_write(WVT_UTP_Data_Header_t *header, uint32_t offset, uint8_t *buffer, uint32_t length)
{
    if((length+offset) > header->size) return WVT_UTP_APPEND_NOT_ENOUGH_MEMORY;
    return utp_struct.memory_write(header->start_address+offset, buffer, length);
}
/*!
 * Стирает область памяти. Функция учитывает особенности постраничной работы с FLASH-памятью
 * @param address Адрес области очистки
 * @param size Размер очищаемой области
 * @return WVT_UTP_Status_t
 */
static WVT_UTP_Status_t WVT_UTP_Memory_erase(uint16_t address, uint16_t size)
{
    if(size == 0) return WVT_UTP_OK;
    uint32_t destination_shift;
    uint32_t destination_page_address;
    uint32_t processed_bytes_count = 0;
    uint8_t *buffer = malloc(utp_struct.memory_page_size);
    while(processed_bytes_count != size){
        destination_shift = (address) % utp_struct.memory_page_size;  //адрес относительно начала страницы
        destination_page_address   = address - destination_shift;                             //текущий адрес страницы для записи
        utp_struct.memory_read(destination_page_address, buffer, utp_struct.memory_page_size);//вычитываем страницу
        utp_struct.memory_erase(destination_page_address, utp_struct.memory_page_size);       //очищаем страницу
        uint16_t tail = utp_struct.memory_page_size - destination_shift;                      //длина данных для записи
        if(tail > (size - processed_bytes_count))tail = (size - processed_bytes_count);
        if(tail != utp_struct.memory_page_size){
            for(uint32_t i=0;i<tail;i++){
                buffer[destination_shift+i] = 0xFF;
            }
            utp_struct.memory_write(destination_page_address, buffer, utp_struct.memory_page_size);
        }
        processed_bytes_count += tail;
        address +=tail;
    }
    free(buffer);
    return WVT_UTP_OK;
}

WVT_UTP_Status_t WVT_UTP_Memory_erase_all()
{
    return utp_struct.memory_erase(0, utp_struct.memory_size);
}
/*!
 * Игра в тетрис с харнилищем. Смещает данные в памяти на величину shift к указанному адресу
 * Необходима для эффективного использования памятми после удаления данных
 * @param address Адрес в памяти к которому будут смещены все элементы начиная с адреса (address+shift)
 * @param shift Величина сдвига данных
 * @param shift Количество сдвигаемых байт
 */
static WVT_UTP_Status_t WVT_UTP_Memory_left_shift(uint32_t address, uint32_t shift, uint32_t count)
{
    if(shift == 0) return WVT_UTP_OK;
    uint32_t destination_shift;
    uint32_t destination_page_address;
    uint32_t processed_bytes_count = 0;
    uint8_t *buffer = malloc(utp_struct.memory_page_size);
    while(processed_bytes_count != count){
        destination_shift = (address /*+ processed_bytes_count*/) % utp_struct.memory_page_size;  //адрес относительно начала страницы
        destination_page_address   = address - destination_shift;                             //текущий адрес страницы для записи
        utp_struct.memory_read(destination_page_address, buffer, utp_struct.memory_page_size);
        uint16_t tail = utp_struct.memory_page_size - destination_shift;
        if(tail > (count - processed_bytes_count))tail = (count - processed_bytes_count);
        utp_struct.memory_read(address+shift, &buffer[destination_shift], tail);
        utp_struct.memory_erase(destination_page_address, utp_struct.memory_page_size);
        utp_struct.memory_write(destination_page_address, buffer, utp_struct.memory_page_size);
        processed_bytes_count += tail;
        address +=tail;
    }
    free(buffer);
    return WVT_UTP_OK;
}


/*!
 * Прочитать данные из сегмента памяти
 * @param uid Уникальный идентификатор, задданный пользователем
 * @param offset Смещение от начала сегмента
 * @param length Длинна данных
 * @param buffer Указатель на буффер данных
 * @return WVT_UTP_ERROR если область прочитать не удалось, WVT_UTP_OK при успешном чтении
 */
WVT_UTP_Read_Status_t WVT_UTP_Memory_read_by_uid(uint16_t uid, uint32_t offset, uint32_t length, uint8_t *buffer){
    WVT_UTP_Data_Header_t temp;
    if(WVT_UTP_Memory_find_by_uid(&temp, uid)==WVT_UTP_OK) {
        if(temp.size<(offset+length)) return WVT_UTP_READ_SIZE_OUT_OF_RANGE;
        if(utp_struct.memory_read(temp.start_address+offset, buffer, length) != WVT_UTP_OK){
            return WVT_UTP_READ_HAL_ERROR;
        }
    }else{
        return WVT_UTP_READ_SEGMENT_NOT_FOUND;
    }
    return WVT_UTP_READ_OK;
}
