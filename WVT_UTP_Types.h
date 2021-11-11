#ifndef UTP_WVT_UTP_TYPES_H
#define UTP_WVT_UTP_TYPES_H

/*!
 * Структура, описыващая сегмент памяти
 */
typedef struct {
    uint16_t      uid;                  //Уникальный номер данных
    uint32_t      start_address;        //Адрес хранения выделяет приёмная сторона
    uint32_t      size;                 //Вычисляется по длинне данных
    uint16_t      crc;                  //CRC данных вычисляет передающая стороная
    uint16_t      crc_of_this_struct;   //Адрес хранения выделяет приёмная сторона
} __attribute__((packed)) WVT_UTP_Data_Header_t;

/*!
 * Результат выполнения операции
 */
typedef enum
{
    WVT_UTP_OK      = 0x00U,
    WVT_UTP_ERROR   = 0x01U,
} WVT_UTP_Status_t;

/*!
 * Структра для инициализации UTP. Содержит в себе параметры работы и HAL-коллбэки
 */
typedef struct
{
    WVT_UTP_Status_t(*init)(uint32_t memory_size);  /*!< Функция записи данных в память */
    WVT_UTP_Status_t(*memory_read)(uint32_t address, uint8_t * buffer, uint32_t size);   /*!< Функция чтения данных из памяти */
    WVT_UTP_Status_t(*memory_write)(uint32_t address, uint8_t * buffer, uint32_t size);  /*!< Функция записи данных в память */
    WVT_UTP_Status_t(*memory_erase)(uint32_t address, uint32_t size);  /*!< Функция записи данных в память */
    uint32_t memory_size;
    uint32_t memory_page_size;
} WVT_UTP_Init_t;

#endif
