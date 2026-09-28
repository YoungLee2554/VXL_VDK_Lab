/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
  /*int state = 0;
  int timer = 5;*/
  int count=0;
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    /* USER CODE END WHILE */
  while(1)
  {
	  if(count>= 10) count = 0;
	  display7SEG(count++);
	  /*if (timer <= 0) {
	            if (state == 0) { state = 1; timer = 2; }
	            else if (state == 1) { state = 2; timer = 5; }
	            else if (state == 2) { state = 3; timer = 2; }
	            else if (state == 3) { state = 0; timer = 5; }
	        }

	        switch (state)
	        {
	            case 0:
	                HAL_GPIO_WritePin(GPIOA, LED_nRED_Pin | LED_sRED_Pin | LED_nYELLOW_Pin | LED_sYELLOW_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_eGREEN_Pin | LED_wGREEN_Pin | LED_eYELLOW_Pin | LED_wYELLOW_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_nGREEN_Pin | LED_sREDA12_Pin, GPIO_PIN_RESET);
	                HAL_GPIO_WritePin(GPIOA, LED_eRED_Pin | LED_wRED_Pin, GPIO_PIN_RESET);
	                break;

	            case 1:
	                HAL_GPIO_WritePin(GPIOA, LED_nGREEN_Pin | LED_sREDA12_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_nYELLOW_Pin | LED_sYELLOW_Pin, GPIO_PIN_RESET);
	                break;

	            case 2:
	                HAL_GPIO_WritePin(GPIOA, LED_nYELLOW_Pin | LED_sYELLOW_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_eRED_Pin | LED_wRED_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_nRED_Pin | LED_sRED_Pin, GPIO_PIN_RESET);
	                HAL_GPIO_WritePin(GPIOA, LED_eGREEN_Pin | LED_wGREEN_Pin, GPIO_PIN_RESET);
	                break;

	            case 3:
	                HAL_GPIO_WritePin(GPIOA, LED_eGREEN_Pin | LED_wGREEN_Pin, GPIO_PIN_SET);
	                HAL_GPIO_WritePin(GPIOA, LED_eYELLOW_Pin | LED_wYELLOW_Pin, GPIO_PIN_RESET);
	                break;
	        }
	        display7SEG(timer);
	        timer--;*/
	        HAL_Delay(1000);
	  }
  }
    /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_nRED_Pin|LED_nYELLOW_Pin|LED_nGREEN_Pin|LED_eRED_Pin
                          |LED_eYELLOW_Pin|LED_eGREEN_Pin|LED_sRED_Pin|LED_sYELLOW_Pin
                          |LED_sREDA12_Pin|LED_wRED_Pin|LED_wYELLOW_Pin|LED_wGREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, PIN_0_Pin|PIN_1_Pin|PIN_2_Pin|PIN_3_Pin
                          |PIN_4_Pin|PIN_5_Pin|PIN_6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_nRED_Pin LED_nYELLOW_Pin LED_nGREEN_Pin LED_eRED_Pin
                           LED_eYELLOW_Pin LED_eGREEN_Pin LED_sRED_Pin LED_sYELLOW_Pin
                           LED_sREDA12_Pin LED_wRED_Pin LED_wYELLOW_Pin LED_wGREEN_Pin */
  GPIO_InitStruct.Pin = LED_nRED_Pin|LED_nYELLOW_Pin|LED_nGREEN_Pin|LED_eRED_Pin
                          |LED_eYELLOW_Pin|LED_eGREEN_Pin|LED_sRED_Pin|LED_sYELLOW_Pin
                          |LED_sREDA12_Pin|LED_wRED_Pin|LED_wYELLOW_Pin|LED_wGREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PIN_0_Pin PIN_1_Pin PIN_2_Pin PIN_3_Pin
                           PIN_4_Pin PIN_5_Pin PIN_6_Pin */
  GPIO_InitStruct.Pin = PIN_0_Pin|PIN_1_Pin|PIN_2_Pin|PIN_3_Pin
                          |PIN_4_Pin|PIN_5_Pin|PIN_6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void display7SEG(int num){
	uint8_t seg7[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};
	if(num<0||num>9) return;
	uint8_t temp = seg7[num];
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (temp >> 0) & 1); // Đoạn a
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (temp >> 1) & 1); // Đoạn b
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (temp >> 2) & 1); // Đoạn c
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (temp >> 3) & 1); // Đoạn d
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (temp >> 4) & 1); // Đoạn e
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (temp >> 5) & 1); // Đoạn f
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (temp >> 6) & 1); // Đoạn g

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
