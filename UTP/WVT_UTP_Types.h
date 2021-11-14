#ifndef UTP_WVT_UTP_TYPES_H
#define UTP_WVT_UTP_TYPES_H

/*!
 * Структура, описыващая сегмент памяти
 */
typedef struct {
    uint16_t uid;                  //Уникальный номер данных
    uint32_t start_address;        //Адрес хранения выделяет приёмная сторона
    uint32_t size;                 //Вычисляется по длинне данных
    uint16_t crc;                  //CRC данных вычисляет передающая стороная
    uint16_t crc_of_this_struct;   //Адрес хранения выделяет приёмная сторона
//#ifdef __cplusplus
    }                           WVT_UTP_Data_Header_t;
//#else
//    }   __attribute__((packed)) WVT_UTP_Data_Header_t;
//#endif

/*!
 * Результат выполнения операции
 */
typedef enum
{
    WVT_UTP_OK      = 0x00U,
    WVT_UTP_ERROR   = 0x01U,
} WVT_UTP_Status_t;

/*!
 * Результат выполнения операции выделения памяти
 */
typedef enum
{
    WVT_UTP_ALLOC_OK                        = 0x00U,
    WVT_UTP_ALLOC_NOT_ENOUGH_PARAMETERS     = 0x01U,
    WVT_UTP_ALLOC_NOT_ENOUGH_MEMORY         = 0x02U,
    WVT_UTP_ALLOC_UID_ALREADY_EXISTS        = 0x03U,
    WVT_UTP_ALLOC_UNKNOWN_ERROR             = 0x04U,
} WVT_UTP_Allocator_Status_t;

/*!
 * Результат выполнения операции записи памяти
 */
typedef enum
{
    WVT_UTP_APPEND_OK                        = 0x00U,
    WVT_UTP_APPEND_WRONG_PARAMETERS          = 0x01U,
    WVT_UTP_APPEND_NOT_ENOUGH_MEMORY         = 0x02U,
} WVT_UTP_Append_Status_t;

/*!
 * Результат выполнения операции записи памяти
 */
typedef enum
{
    WVT_UTP_GH_OK                        = 0x00U,
    WVT_UTP_GH_WRONG_PARAMETERS          = 0x01U,
    WVT_UTP_GH_NOT_FOUND                 = 0x02U,
    WVT_UTP_GH_DATA_BROKEN               = 0x03U,
} WVT_UTP_GetHeader_Status_t;

/*!
 * Результат выполнения операции фиксации памяти
 */
typedef enum
{
    WVT_UTP_FREEZE_OK                        = 0x00U,
    WVT_UTP_FREEZE_CRC_ERROR                 = 0x01U,
} WVT_UTP_Freeze_Status_t;

/*!
 * Результат выполнения операции фиксации памяти
 */
typedef enum
{
    WVT_UTP_READ_OK                        = 0x00U,
    WVT_UTP_READ_WRONG_PARAMETERS          = 0x01U,
    WVT_UTP_READ_SEGMENT_NOT_FOUND         = 0x02U,
    WVT_UTP_READ_SIZE_OUT_OF_RANGE         = 0x03U,
    WVT_UTP_READ_HAL_ERROR                 = 0x04U,
} WVT_UTP_Read_Status_t;

/*!
 * Результат выполнения операции очистки памяти
 */
typedef enum
{
    WVT_UTP_ERASE_OK                        = 0x00U,
    WVT_UTP_ERASE_WRONG_PARAMETERS          = 0x01U,
    WVT_UTP_ERASE_NOT_FOUND                 = 0x02U,
} WVT_UTP_Erase_Status_t;

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
