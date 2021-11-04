# Waviot UTP (Update Transfer Protocol)
## Описание работы библиотеки


## Инициализация UTP Обработчика
```c
#include "WVT_UTP.h"
int main(void){
    WVT_UTP_Init_t settings;
    settings.memory_size        = 1024 * 4; 
    settings.memory_page_size   = 8;      
    settings.init               = &memory_init;
    settings.memory_erase       = &memory_erase;
    settings.memory_read        = &memory_read;
    settings.memory_write       = &memory_write;
    WVT_UTP_init(settings);
}
```

WVT_UTP_Init_t содержит в себе параметры выделенной памяти, а также HAL-коллбэки
| Параметр | Описание |
| ------ | ----------- |
| memory_size   | Полный размер памяти, отведенной для хранилища данных |
| memory_page_size | Размер страницы флеш-памяти |
## Описание коллбэков

| Коллбэк | Описание |
| ------ | ----------- |
| init   | Вызывается перед началом работы библитеки, в качестве аргумента передается полный размер памяти memory_size|
| memory_erase | Стереть участок памяти. Библиотека гарантирует, что относительный адрес начала стирания будет равен началу страницы флеш памяти, а запрашиваемый размер области очистки будет кратен размеру страницы  |
| memory_read    | Прочитать участок памяти указанного размера по относительному адресу |
| memory_write   | Стереть участок памяти указанного размера по относительному адресу |
