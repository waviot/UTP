#include <stdio.h>
#include "WVT_UTP.h"
#include "network.h"
#include <unistd.h>

void packet_handler(uint8_t *request, uint32_t request_length,
                        uint8_t *response, uint32_t *response_length)
{
    WVT_UTP_packet_handler(request, request_length, response, response_length);
}

/**
 * Предварительная инициализация хранилища
 * @param memory_size Размер используемой ROM памяти
 * @return WVT_UTP_OK при успешной процедуре инициализации
 */
WVT_UTP_Status_t memory_init(uint32_t memory_size){
    FILE *fp;
    if ((fp=fopen("storage.bin", "wb"))==NULL) {
        fclose(fp);
        printf("Cannot open file.\n");
        return WVT_UTP_ERROR;
    }
    uint8_t tbyte = 0xFF;
    for(int i=0;i<memory_size;i++)fwrite(&tbyte,1,1,fp);
    fclose(fp);
    printf("Memory initialized successfully. Size: 0x%X\n", memory_size);
    return WVT_UTP_OK;
}

/*!
 * Чтение области памяти
 * @param address относительный адрес памяти
 * @param buffer указатель на буфер данных
 * @param size количество байт
 * @return WVT_UTP_OK при успешной процедуре инициализации
 */
WVT_UTP_Status_t memory_read(uint32_t address, uint8_t * buffer, uint32_t size) {
    FILE *fp;
    if ((fp=fopen("storage.bin", "rb"))==NULL) {
        fclose(fp);
        printf("Cannot open file.\n");
        return WVT_UTP_ERROR;
    }
    fseek(fp, address, SEEK_SET);
    fread(buffer, 1, size, fp);
    fclose(fp);
    //printf("Memory reading from 0x%X. Size: 0x%X\n", address, size);
    return WVT_UTP_OK;
}

/*!
 * Записать область памяти
 * @param address относительный адрес начала записи данных
 * @param buffer входящий буффер данных
 * @param size количество байт для записи
 * @return WVT_UTP_OK при успешной процедуре инициализации
 */
WVT_UTP_Status_t memory_write(uint32_t address, uint8_t * buffer, uint32_t size) {
    FILE *fp;
    if ((fp=fopen("storage.bin", "r+b"))==NULL) {
        fclose(fp);
        printf("Cannot open file.\n");
        return WVT_UTP_ERROR;
    }
    fseek(fp, address, SEEK_SET);
    fwrite(buffer, 1, size, fp);
    fclose(fp);
    return WVT_UTP_OK;
}

/*!
 * Стереть область памяти
 * @param address стартовый адрес участка памяти
 * @param size размер стираемой области памяти
 * @return WVT_UTP_OK при успешной процедуре инициализации
 */
WVT_UTP_Status_t memory_erase(uint32_t address, uint32_t size) {
    //printf("Memory erasing from 0x%X. Size: 0x%X\n", address, size);
    uint8_t d = 0xFF;
    for(uint32_t i=0;i<size;i++){
        memory_write(address+i, &d, 1);
    }
    return WVT_UTP_OK;
}

void main() {
    WVT_UTP_Init_t settings;
    settings.memory_size        = 0x10000;   //4kB size
    settings.memory_page_size = 128;          //128 bytes
    settings.init = &memory_init;
    settings.memory_erase = &memory_erase;
    settings.memory_read = &memory_read;
    settings.memory_write = &memory_write;
    settings.storage_initial_value = 0xFF;
    WVT_UTP_init(settings);
    network_init(8000);
    while(1){
        int socket_fd = network_accept_client(); //Ждем подключения нового клиента
        network_io_process(socket_fd, packet_handler);    //Обработка данных
        network_close(socket_fd);                         //Закрываем соединение
    }
}
