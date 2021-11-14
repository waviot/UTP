#include "main.h"
#include "WVT_UTP.h"

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

#define STORAGE_ADDRESS 0x08010000
#define STORAGE_SIZE    0x00010000

WVT_UTP_Status_t memory_init(uint32_t memory_size){
    return WVT_UTP_OK;
}

WVT_UTP_Status_t memory_read(uint32_t address, uint8_t * buffer, uint32_t size) {
    address += STORAGE_ADDRESS;
    uint8_t * ptr = (uint8_t *)address;
    for(uint32_t i=0; i<size;i++)
    {
      buffer[i] = ptr[i];
    }
    return WVT_UTP_OK;
}

WVT_UTP_Status_t memory_write(uint32_t address, uint8_t *data, uint32_t length) {
    uint32_t value=0;
    uint32_t counter=0;
    uint32_t word_address=0;
    WVT_UTP_Status_t result = WVT_UTP_OK;
    HAL_FLASH_Unlock(); 
    
    while (counter<length)
    { 
        uint8_t changes_vount = sizeof(uint32_t) - (address % sizeof(uint32_t));
        word_address = address  - (address % sizeof(uint32_t));
        memory_read(word_address, (uint8_t *)&value, sizeof(value));
        for(uint8_t i=address % sizeof(uint32_t); i<sizeof(uint32_t) && counter<length;i++){
            value &= ~( (uint32_t)(0xFF << (i*8)) );
            value |= ( (uint32_t)( data[counter++] << (i*8)) );
            address ++;
        }
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, word_address + STORAGE_ADDRESS, value) != HAL_OK){
            result = WVT_UTP_ERROR;
            break;
        }
    }
    return result;
}


WVT_UTP_Status_t memory_erase(uint32_t address, uint32_t size) {
    address += STORAGE_ADDRESS;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError;
    HAL_FLASH_Unlock();
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = address;
    EraseInitStruct.NbPages     = size/FLASH_PAGE_SIZE;
    HAL_StatusTypeDef result = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
    //HAL_FLASH_Lock();
    if (result != HAL_OK){
      return WVT_UTP_ERROR;
    }
    return WVT_UTP_OK;
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  
  WVT_UTP_Init_t settings;
  settings.memory_size        = STORAGE_SIZE;    
  settings.memory_page_size   = FLASH_PAGE_SIZE; 
  settings.init = &memory_init;
  settings.memory_erase = &memory_erase;
  settings.memory_read = &memory_read;
  settings.memory_write = &memory_write;
  WVT_UTP_init(settings);
  
  //memory_erase(0, STORAGE_SIZE);
  uint8_t rx_buffer[128];
  uint8_t tx_buffer[128];
  uint32_t cursor = 0;
  uint8_t last_received_byte = 0;
  while (1)
  {
      uint8_t current_byte;
      if( HAL_UART_Receive(&huart2, &current_byte, 1, 100) == HAL_OK){
        if(last_received_byte==0xAA){
            if(current_byte == 0){
                 if(cursor<255)rx_buffer[cursor++] = 0xAA;
            }
            if(current_byte == 1){
                 cursor = 0;
            }
            if(current_byte == 2){
                 uint32_t response_length = 0;
                 WVT_UTP_packet_handler(rx_buffer, cursor, tx_buffer, &response_length);
                 uint8_t b = 0xAA;
                 HAL_UART_Transmit(&huart2, &b, 1, 1000); b = 1;
                 HAL_UART_Transmit(&huart2, &b, 1, 1000);
                 for(uint32_t i=0;i<response_length;i++){
                    HAL_UART_Transmit(&huart2, &tx_buffer[i], 1, 1000);
                    if(tx_buffer[i]==0xAA){
                        b = 0x0;
                        HAL_UART_Transmit(&huart2, &b, 1, 1000);
                    }
                 }
                 b = 0xAA;
                 HAL_UART_Transmit(&huart2, &b, 1, 1000); b = 2;
                 HAL_UART_Transmit(&huart2, &b, 1, 1000);
                 HAL_UART_Transmit(&huart2, tx_buffer, response_length, 1000);
            }
        }else{           
            if(current_byte != 0xAA){
                if(cursor<255)rx_buffer[cursor++] = current_byte;
            }
        }
        last_received_byte = current_byte;
      }   
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif 


