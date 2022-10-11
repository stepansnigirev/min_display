/**
  * Minimal display example
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32469i_discovery_lcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BLACK           0xFF000000
#define BLUE            0xFF5267FF
#define PROGRESS_WIDTH  380
#define PROGRESS_Y      250
#define LCD_WIDTH       480
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/
static void lcd_init(){
  BSP_LCD_Init();
  BSP_LCD_InitEx(LCD_ORIENTATION_PORTRAIT);
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_BACKGROUND, LCD_FB_START_ADDRESS);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);
  BSP_LCD_Clear(BLACK);
  BSP_LCD_SetBackColor(BLACK);
  BSP_LCD_SetTextColor(BLUE);
}

static void show_progress(int progress){
  uint16_t x0 = (LCD_WIDTH-PROGRESS_WIDTH)/2;
  uint16_t active_width = progress*PROGRESS_WIDTH/100;
  uint16_t inactive_width = PROGRESS_WIDTH-active_width;

  BSP_LCD_SetTextColor(BLUE);
  BSP_LCD_FillRect(x0, PROGRESS_Y, progress*PROGRESS_WIDTH/100, 20);
  BSP_LCD_SetTextColor(BLACK);
  BSP_LCD_FillRect(x0+active_width, PROGRESS_Y, inactive_width, 20);
  BSP_LCD_SetTextColor(BLUE);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* STM32F469xx HAL library initialization */
  HAL_Init();
  
  /* Configure the System clock to have a frequency of 180 MHz */
  SystemClock_Config();
  LED1_GPIO_CLK_ENABLE();
  LED2_GPIO_CLK_ENABLE();
  LED3_GPIO_CLK_ENABLE();
  LED4_GPIO_CLK_ENABLE();
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);
  for (int i = 0; i < 5; ++i)
  {
    HAL_Delay(100);
    BSP_LED_Toggle(LED1);
    HAL_Delay(100);
    BSP_LED_Toggle(LED2);
    HAL_Delay(100);
    BSP_LED_Toggle(LED3);
    HAL_Delay(100);
    BSP_LED_Toggle(LED4);
  }
  
  lcd_init();
  BSP_LCD_DisplayStringAt(0, 200, (uint8_t *)"Testing display...", CENTER_MODE);

  int progress = 0;
  show_progress(progress);

  while(1)
  {
    HAL_Delay(30);
    
    progress = (progress+1) % 100;
    show_progress(progress);

  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  *         The USB clock configuration from PLLSAI:
  *            PLLSAIM                        = 8
  *            PLLSAIN                        = 384
  *            PLLSAIP                        = 8
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
#if defined(USE_STM32469I_DISCO_REVA)
  RCC_OscInitStruct.PLL.PLLM = 25;
#else
  RCC_OscInitStruct.PLL.PLLM = 8;
#endif /* USE_STM32469I_DISCO_REVA */
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 2;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Activate the OverDrive to reach the 180 MHz Frequency */  
  HAL_PWREx_EnableOverDrive();
  
  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 7; 
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

/**
  * @brief This function provides accurate delay (in milliseconds) based 
  *        on SysTick counter flag.
  * @note This function is declared as __weak to be overwritten in case of other
  *       implementations in user file.
  * @param Delay: specifies the delay time length, in milliseconds.
  * @retval None
  */

void HAL_Delay(__IO uint32_t Delay)
{
  while(Delay) 
  {
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) 
    {
      Delay--;
    }
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
