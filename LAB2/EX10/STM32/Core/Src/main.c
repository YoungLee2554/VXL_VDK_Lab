/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
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
#define MAX_LED               4
#define MAX_LED_MATRIX        8
#define SCROLL_WIDTH          16

/* Chu ky ngat TIM2: 8MHz / (7999+1) = 1000Hz, 1000Hz / (9+1) = 100Hz => 10ms.
 * Bien TIMER_CYCLE (khai bao o USER CODE BEGIN PV) PHAI khop gia tri nay.
 * Neu doi Prescaler/Period trong file .ioc thi phai doi ca TIMER_CYCLE. */
#define TIMER_PERIOD_MS       10

/* --- 7-segment (Bai 4/5) ---
 * Phat update7SEG() moi 250ms => 1 vong 4 digit = 1000ms => f quet = 1Hz
 * (dung yeu cau Bai 4/Bai 5, Proteus thay ro lan luot tung so).
 * Khi chay mach that: doi thanh 20 (= 20ms/digit) => f = 50Hz, nguong
 * mat thay sang dong thoi la >30Hz. */
#define SCAN_7SEG_MS          250
#define DOT_TOGGLE_MS         1000      /* DOT doi trang thai moi 1s.
                                           Neu hieu chu ky 1s thi doi 500. */
#define CLOCK_TICK_MS         1000      /* buoc dem 1s cua dong ho */

/* --- LED matrix quet theo COT -> HANG (Bai 9/10) ---
 * Moi buoc quet chon 1 cot (ENM0..ENM7) va xuat 8 bit du lieu hang (ROW0..ROW7).
 * MATRIX_SCAN_MS = thoi luong 1 cot => 1 frame = 8 * 10 = 80ms (~12.5Hz).
 * Tren Proteus gia tri nay OK (de noi ro). Khi chay that nen dat 2 => 62.5Hz. */
#define MATRIX_SCAN_MS        10
#define SCROLL_FRAME_DIV      2         /* dich 1 cot sau moi 2 frame */

/* Huong chay cua chu A (xem giai thich trong updateMatrixBuffer):
 *   +1 : scroll_pos tang => chu chay ve phia chi so cot GIAM (ENM7 -> ENM0, sang trai)
 *   -1 : scroll_pos giam => chu chay ve phia chi so cot TANG (ENM0 -> ENM7, sang phai)
 * Chay thu tren Proteus, neu huong nguoc yeu cau "shifted to the left"
 * thi dao gia tri nay - ca hai deu wrap quanh SCROLL_WIDTH. */
#define SCROLL_STEP           (+1)

#define ALL_ENABLE_PINS       (EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin)
#define ALL_SEGMENT_PINS      (SEG0_Pin | SEG1_Pin | SEG2_Pin | SEG3_Pin | \
                               SEG4_Pin | SEG5_Pin | SEG6_Pin)
#define ALL_ENM_PINS          (ENM0_Pin | ENM1_Pin | ENM2_Pin | ENM3_Pin | \
                               ENM4_Pin | ENM5_Pin | ENM6_Pin | ENM7_Pin)
#define ALL_ROW_PINS          (ROW0_Pin | ROW1_Pin | ROW2_Pin | ROW3_Pin | \
                               ROW4_Pin | ROW5_Pin | ROW6_Pin | ROW7_Pin)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* ===== Soft-timer (Bai 6-8) =====
 * Cac bien counter/flag duoc ISR (timer_run) ghi va main() DOC, nen bat
 * buoc volatile. Thieu volatile thi voi -O1/-O2 compiler duoc phep cache
 * gia tri vao thanh ghi => main() khong bao gio nhan thay flag = 1. */
volatile int timer0_counter = 0;   /* 1s   : dong ho + DOT */
volatile int timer0_flag    = 0;
volatile int timer1_counter = 0;   /* 250ms: quet 7-segment */
volatile int timer1_flag    = 0;
volatile int timer2_counter = 0;   /* 10ms : quet LED matrix */
volatile int timer2_flag    = 0;
volatile int TIMER_CYCLE    = TIMER_PERIOD_MS;

/* ===== 7-segment ===== */
volatile int index_led = 0;
volatile int led_buffer[4] = {1, 5, 0, 8};   /* thi du 15:08 -> 1 5 0 8 */
volatile int hour = 15, minute = 8, second = 50;
static int dot_state = 0;

/* ===== LED matrix 8x8 quet COT -> HANG (Bai 9/10) =====
 * Moi phan tu la 1 COT (8 bit = 8 hang ROW0..ROW7):
 *   Bit 0 (LSB) = ROW0 (hang tren cung)
 *   Bit 1       = ROW1
 *   Bit 2       = ROW2
 *   Bit 3       = ROW3 (thanh ngang chu A)
 *   Bit 4..7    = ROW4..ROW7 (hai chan chu A chay xuong day)
 *
 * Ma hoa 5 cot cua chu "A":
 *   Cot trai       (ROW2..ROW7 sang): 0b11111100 = 0xFC
 *   Cot ke trai    (ROW1, ROW3 sang): 0b00001010 = 0x0A
 *   Cot dinh giua  (ROW0, ROW3 sang): 0b00001001 = 0x09
 *   Cot ke phai    (ROW1, ROW3 sang): 0b00001010 = 0x0A
 *   Cot phai       (ROW2..ROW7 sang): 0b11111100 = 0xFC
 *
 * Hinh chieu 8 hang x 5 cot (index 5..9 tren bang cuon 16 cot):
 *   ROW0 (bit 0):  . . # . .
 *   ROW1 (bit 1):  . # . # .
 *   ROW2 (bit 2):  # . . . #
 *   ROW3 (bit 3):  # # # # #
 *   ROW4 (bit 4):  # . . . #
 *   ROW5 (bit 5):  # . . . #
 *   ROW6 (bit 6):  # . . . #
 *   ROW7 (bit 7):  # . . . #
 *
 * Quy uoc phan cung theo so do Bai 9:
 *   - ENM0..ENM7 qua ULN2803 (open-collector sink):
 *     ENM = SET (1) => ULN2803 dan => keo COL xuong GND (enable cot).
 *     ENM = RESET (0) => ULN2803 ngat (disable cot).
 *   - ROW0..ROW7 do MCU cap truc tiep (data):
 *     ROW = SET (1) => cap nguon cho hang => LED tai (ROW, COL) SANG.
 *     ROW = RESET (0) => LED TAT. */
volatile int index_led_matrix = 0;

/* Buffer 8 cot hien thi truc tiep tren ma tran (dung khung Bai 9) */
uint8_t matrix_buffer[MAX_LED_MATRIX] = {
    0x00, 0xFC, 0x0A, 0x09, 0x0A, 0xFC, 0x00, 0x00
};

/* Bang cuon 16 cot cho Bai 10: chu A nam o index 5..9, hai ben la cot trong */
static const uint8_t scroll_tape[SCROLL_WIDTH] = {
    0x00, 0x00, 0x00, 0x00, 0x00,
    0xFC, 0x0A, 0x09, 0x0A, 0xFC,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

volatile int scroll_pos = 4;   /* khoi dau tai 4 de chu A (5..9) nam giua khung 8 cot */
static int scroll_div = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void timer_set(volatile int *counter, volatile int *flag, int duration);
void setTimer0(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void timer_run(void);

void display7SEG(int num);
void updateClockBuffer(void);
void update7SEG(int index);

static void displayRowData(uint8_t data);
static void updateMatrixBuffer(void);
void updateLEDMatrix(int index);
void scanLEDMatrix(void);
static void advance_scroll(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief Nap mot software timer: quy duration (ms) ra so nhip ngat.
  *        Lam tron LEN va bao dam toi thieu 1 nhip, de duration < TIMER_CYCLE
  *        khong bi ep ve 0 (neu ve 0 thi ngat khong bao gio set flag =>
  *        chuc nang dung han - day la cau tra loi Report 2/3 Bai 6).
  *        Xoa flag TRUOC khi nap counter tranh race voi ISR.
  */
static void timer_set(volatile int *counter, volatile int *flag, int duration)
{
  int ticks;

  if (duration <= 0)
  {
    ticks = 1;
  }
  else
  {
    ticks = duration / TIMER_CYCLE;
    if ((duration % TIMER_CYCLE) != 0)
    {
      ticks++;                    /* lam tron len */
    }
    if (ticks < 1)
    {
      ticks = 1;
    }
  }

  *flag = 0;
  *counter = ticks;
}

void setTimer0(int duration) { timer_set(&timer0_counter, &timer0_flag, duration); }
void setTimer1(int duration) { timer_set(&timer1_counter, &timer1_flag, duration); }
void setTimer2(int duration) { timer_set(&timer2_counter, &timer2_flag, duration); }

/**
  * @brief Duoc goi trong ngat TIM2 moi 10ms. Chi dem va set flag => ISR ngan
  *        nhat co the, toan bo xu ly phuc tap nam o main() (Bai 6-8).
  */
void timer_run(void)
{
  if (timer0_counter > 0)
  {
    timer0_counter--;
    if (timer0_counter == 0) timer0_flag = 1;
  }
  if (timer1_counter > 0)
  {
    timer1_counter--;
    if (timer1_counter == 0) timer1_flag = 1;
  }
  if (timer2_counter > 0)
  {
    timer2_counter--;
    if (timer2_counter == 0) timer2_flag = 1;
  }
}
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

  /* Reset of all peripherals, Initializes the Flash interface, and Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_TIM2_Init();
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  /* Blank chu dong truoc khi bat ngat de tranh nhay loe luc boot:
   * - 7-seg anode chung: EN = SET (tat PNP), SEG = SET (tat doan).
   * - LED matrix: ENM = RESET (tat ULN2803, khong chon cot), ROW = RESET (tat hang). */
  HAL_GPIO_WritePin(GPIOA, ALL_ENABLE_PINS, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, ALL_ENM_PINS, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ALL_SEGMENT_PINS, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, ALL_ROW_PINS, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, DOT_Pin, GPIO_PIN_RESET);

  updateClockBuffer();
  update7SEG(0);
  index_led = 1;

  updateMatrixBuffer();

  /* Bat NVIC roi moi bat ngat. De o day (khong phai trong MX_TIM2_Init) de
   * tranh ngat fire luc MX_GPIO_Init() chua xong. */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);

  setTimer0(CLOCK_TICK_MS);   /* 1s   : dong ho + DOT     */
  setTimer1(SCAN_7SEG_MS);    /* 250ms: quet 1 digit      */
  setTimer2(MATRIX_SCAN_MS);  /* 10ms : quet 1 cot matrix */

  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* (1) Moi 1s: tang dong ho va dao trang thai DOT.
     *     Da bo HAL_Delay(1000) => non-blocking (Bai 7). */
    if (timer0_flag == 1)
    {
      setTimer0(CLOCK_TICK_MS);

      second++;
      if (second >= 60)
      {
        second = 0;
        minute++;
        if (minute >= 60)
        {
          minute = 0;
          hour++;
        }
      }
      if (hour >= 24)
      {
        hour = 0;
      }
      updateClockBuffer();

      dot_state ^= 1;
      HAL_GPIO_WritePin(GPIOA, DOT_Pin,
                        (dot_state != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    /* (2) Moi 250ms: quet 1 digit 7-segment => f quet toan bo 4 digit = 1Hz. */
    if (timer1_flag == 1)
    {
      setTimer1(SCAN_7SEG_MS);

      update7SEG(index_led);
      index_led++;
      if (index_led >= MAX_LED)
      {
        index_led = 0;
      }
    }

    /* (3) Moi 10ms: quet 1 cot LED matrix (8 cot = 1 frame = 80ms). */
    if (timer2_flag == 1)
    {
      setTimer2(MATRIX_SCAN_MS);

      scanLEDMatrix();
    }
  }
  /* USER CODE END 3 */
}

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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
  /* 8MHz / (7999+1) = 1000Hz ; 1000Hz / (9+1) = 100Hz => ngat moi 10ms.
   * NVIC duoc bat trong USER CODE BEGIN 2 (sau MX_GPIO_Init). */
  /* USER CODE END TIM2_Init 2 */
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
  HAL_GPIO_WritePin(GPIOA, DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin|ENM0_Pin|ENM1_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin|ROW0_Pin
                          |ROW1_Pin|ROW2_Pin|ROW3_Pin|ROW4_Pin
                          |ROW5_Pin|ROW6_Pin|ROW7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOT_Pin LED_RED_Pin EN0_Pin EN1_Pin
                           EN2_Pin EN3_Pin ENM0_Pin ENM1_Pin
                           ENM2_Pin ENM3_Pin ENM4_Pin ENM5_Pin
                           ENM6_Pin ENM7_Pin */
  GPIO_InitStruct.Pin = DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin|ENM0_Pin|ENM1_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin SEG3_Pin
                           SEG4_Pin SEG5_Pin SEG6_Pin ROW0_Pin
                           ROW1_Pin ROW2_Pin ROW3_Pin ROW4_Pin
                           ROW5_Pin ROW6_Pin ROW7_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin|ROW0_Pin
                          |ROW1_Pin|ROW2_Pin|ROW3_Pin|ROW4_Pin
                          |ROW5_Pin|ROW6_Pin|ROW7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/**
  * @brief Xuat mot so (0..9) ra 7-segment loai ANODE CHUNG => SEG active-low
  *        (0 = sang). Bit 0=a, 1=b, 2=c, 3=d, 4=e, 5=f, 6=g (SEG0..SEG6).
  *        Bang ma gia tri SET len chan: 0xC0='0', 0xF9='1', ..., 0x90='9'.
  */
void display7SEG(int num)
{
  static const uint8_t seg7[10] =
      {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
  uint8_t temp;

  if (num < 0 || num > 9)
  {
    return;
  }
  temp = seg7[num];

  HAL_GPIO_WritePin(GPIOB, SEG0_Pin, (temp >> 0) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG1_Pin, (temp >> 1) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG2_Pin, (temp >> 2) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG3_Pin, (temp >> 3) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG4_Pin, (temp >> 4) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG5_Pin, (temp >> 5) & 1);
  HAL_GPIO_WritePin(GPIOB, SEG6_Pin, (temp >> 6) & 1);
}

/**
  * @brief Sinh led_buffer tu hour/minute. So co 1 chu so duoc bu chu so 0 o
  *        truoc (hour=8, minute=5 => 0 8 0 5). Day la noi dung Report 1 Bai 5.
  */
void updateClockBuffer(void)
{
  led_buffer[0] = hour   / 10;
  led_buffer[1] = hour   % 10;
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute % 10;
}

/**
  * @brief Quet digit thu index theo thu tu: TAT ca EN -> cap nhat SEG ->
  *        BAT lai dung 1 digit. Thu tu nay tranh ghosting (segment bi doi
  *        trong khi digit cu van dang sang). EN active-low (PNP).
  */
void update7SEG(int index)
{
  static const uint32_t en_pin[MAX_LED] = {EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin};
  int digit;

  if (index < 0 || index >= MAX_LED)
  {
    return;
  }

  digit = led_buffer[index];      /* doc 1 ban sao, de main() khong ghi giua chung */

  HAL_GPIO_WritePin(GPIOA, ALL_ENABLE_PINS, GPIO_PIN_SET);
  display7SEG(digit);
  HAL_GPIO_WritePin(GPIOA, en_pin[index], GPIO_PIN_RESET);
}

/**
  * @brief Xuat 8 bit du lieu cua 1 cot ra 8 hang ROW0..ROW7 (PB8..PB15).
  *        Bit 0 = ROW0 (hang tren cung), ..., Bit 7 = ROW7 (hang duoi cung).
  *        Bit = 1 => GPIO_PIN_SET (cap nguon cho hang).
  */
static void displayRowData(uint8_t data)
{
  HAL_GPIO_WritePin(GPIOB, ROW0_Pin, ((data >> 0) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW1_Pin, ((data >> 1) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW2_Pin, ((data >> 2) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW3_Pin, ((data >> 3) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW4_Pin, ((data >> 4) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW5_Pin, ((data >> 5) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW6_Pin, ((data >> 6) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, ROW7_Pin, ((data >> 7) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief Cap nhat 8 cot cua matrix_buffer tu bang cuon 16 cot (scroll_tape)
  *        theo vi tri scroll_pos hien tai:
  *        matrix_buffer[col] = scroll_tape[(scroll_pos + col) % SCROLL_WIDTH]
  *        <=> cot hien thi `col` nhan du lieu tai vi tri `g` khi col = (g - scroll_pos) mod 16.
  */
static void updateMatrixBuffer(void)
{
  int col;

  for (col = 0; col < MAX_LED_MATRIX; col++)
  {
    int src = (scroll_pos + col) % SCROLL_WIDTH;
    matrix_buffer[col] = scroll_tape[src];
  }
}

/**
  * @brief Quet cot thu `index` (0..7) cua LED matrix theo dung khung Bai 9:
  *        1) Tat tat ca ENM0..ENM7 (blank) de tranh lem anh (ghosting).
  *        2) Xuat 8 bit du lieu hang cua matrix_buffer[index] ra ROW0..ROW7.
  *        3) Bat dung 1 chan ENM[index] = SET (ULN2803 keo cot do xuong GND).
  */
void updateLEDMatrix(int index)
{
  if (index < 0 || index >= MAX_LED_MATRIX)
  {
    return;
  }

  HAL_GPIO_WritePin(GPIOA, ALL_ENM_PINS, GPIO_PIN_RESET);

  switch (index)
  {
    case 0:
      displayRowData(matrix_buffer[0]);
      HAL_GPIO_WritePin(GPIOA, ENM0_Pin, GPIO_PIN_SET);
      break;
    case 1:
      displayRowData(matrix_buffer[1]);
      HAL_GPIO_WritePin(GPIOA, ENM1_Pin, GPIO_PIN_SET);
      break;
    case 2:
      displayRowData(matrix_buffer[2]);
      HAL_GPIO_WritePin(GPIOA, ENM2_Pin, GPIO_PIN_SET);
      break;
    case 3:
      displayRowData(matrix_buffer[3]);
      HAL_GPIO_WritePin(GPIOA, ENM3_Pin, GPIO_PIN_SET);
      break;
    case 4:
      displayRowData(matrix_buffer[4]);
      HAL_GPIO_WritePin(GPIOA, ENM4_Pin, GPIO_PIN_SET);
      break;
    case 5:
      displayRowData(matrix_buffer[5]);
      HAL_GPIO_WritePin(GPIOA, ENM5_Pin, GPIO_PIN_SET);
      break;
    case 6:
      displayRowData(matrix_buffer[6]);
      HAL_GPIO_WritePin(GPIOA, ENM6_Pin, GPIO_PIN_SET);
      break;
    case 7:
      displayRowData(matrix_buffer[7]);
      HAL_GPIO_WritePin(GPIOA, ENM7_Pin, GPIO_PIN_SET);
      break;
    default:
      break;
  }
}

/**
  * @brief Quet 1 cot LED matrix: goi updateLEDMatrix(index_led_matrix).
  *        Khi du 8 cot (het 1 frame) thi tinh buoc cuon.
  *        Duoc goi tu main() theo timer2 (10ms/cot), KHONG goi trong ISR.
  */
void scanLEDMatrix(void)
{
  if (index_led_matrix < 0 || index_led_matrix >= MAX_LED_MATRIX)
  {
    index_led_matrix = 0;
  }

  updateLEDMatrix(index_led_matrix);

  index_led_matrix++;
  if (index_led_matrix >= MAX_LED_MATRIX)
  {
    index_led_matrix = 0;
    scroll_div++;
    if (scroll_div >= SCROLL_FRAME_DIV)
    {
      scroll_div = 0;
      advance_scroll();
    }
  }
}

/**
  * @brief Dich bang cuon di 1 cot theo huong SCROLL_STEP, wrap trong 0..15,
  *        sau do cap nhat lai matrix_buffer[0..7].
  */
static void advance_scroll(void)
{
  if (SCROLL_STEP > 0)
  {
    scroll_pos = (scroll_pos + 1) % SCROLL_WIDTH;
  }
  else
  {
    scroll_pos = (scroll_pos + SCROLL_WIDTH - 1) % SCROLL_WIDTH;
  }
  updateMatrixBuffer();
}

/**
  * @brief ISR ngat TIM2 - chay moi TIMER_PERIOD_MS (10ms).
  *        Chi goi timer_run(): toan bo xu ly (dong ho, quet 7-seg, quet matrix)
  *        nam trong vong lap while(1) cua main(), ISR chi dem va set flag.
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    timer_run();
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param function has failed
  * @param  file: pointer to the source file name
  * @param  line: assert_param line number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report file name and line number,
     e.g. printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
