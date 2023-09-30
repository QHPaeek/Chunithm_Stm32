/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "mpr121.h"
#include "usb_device.h"
#include "usbd_hid.h"
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
/* USER CODE BEGIN Variables */
extern USBD_HandleTypeDef hUsbDeviceFS;

//各个传感器的状态
uint16_t touchA_status = 0;
uint16_t touchB_status = 0;
uint16_t touchC_status = 0;

//掩码
uint16_t maskcode[12] = {0b1,0b10,0b100,0b1000,0b10000,0b100000,0b1000000,0b10000000,
0b100000000,0b1000000000,0b10000000000,0b100000000000};

//HID对应键值
uint8_t keycodeA[10] = {5,4,7,6,9,8,11,10,13,12};
uint8_t keycodeB[12] = {15,14,17,16,19,18,21,20,23,22,25,24};
uint8_t keycodeC[10] = {27,26,29,28,31,30,33,32,35,34};
uint8_t ir_keycode[6] = {36,37,38,39,86,87};
uint8_t other_keycode[2] = {};
//对应按键顺序BADCFEHGJI

uint8_t key_buffer[42] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for task01 */
osThreadId_t task01Handle;
const osThreadAttr_t task01_attributes = {
  .name = "task01",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task02 */
osThreadId_t task02Handle;
const osThreadAttr_t task02_attributes = {
  .name = "task02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task03 */
osThreadId_t task03Handle;
const osThreadAttr_t task03_attributes = {
  .name = "task03",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task04 */
osThreadId_t task04Handle;
const osThreadAttr_t task04_attributes = {
  .name = "task04",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task05 */
osThreadId_t task05Handle;
const osThreadAttr_t task05_attributes = {
  .name = "task05",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for task06 */
osThreadId_t task06Handle;
const osThreadAttr_t task06_attributes = {
  .name = "task06",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void USB_task(void *argument);
void MPR121_1(void *argument);
void MPR121_2(void *argument);
void MPR121_3(void *argument);
void WS2812_task(void *argument);
void IR_task(void *argument);
void CardReader(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	//TaskHandle_t myTaskHandle;
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(USB_task, NULL, &defaultTask_attributes);

  /* creation of task01 */
  task01Handle = osThreadNew(MPR121_1, NULL, &task01_attributes);

  /* creation of task02 */
  task02Handle = osThreadNew(MPR121_2, NULL, &task02_attributes);

  /* creation of task03 */
  task03Handle = osThreadNew(MPR121_3, NULL, &task03_attributes);

  /* creation of task04 */
  task04Handle = osThreadNew(WS2812_task, NULL, &task04_attributes);

  /* creation of task05 */
  task05Handle = osThreadNew(IR_task, NULL, &task05_attributes);

  /* creation of task06 */
  task06Handle = osThreadNew(CardReader, NULL, &task06_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_USB_task */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_USB_task */
void USB_task(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN USB_task */
  /* Infinite loop */
  for(;;)
  {
	  	  	for(uint8_t i = 0 ; i < 11 ; i++)
	  		{
	  			key_buffer[i+2] = (touchA_status & maskcode[i]) ? keycodeA[i] : 0 ;
	  		}
	  		for(uint8_t j = 0 ; j < 13 ; j++)
	  		{
	  			key_buffer[j+12] = (touchB_status & maskcode[j]) ? keycodeB[j] : 0 ;
	  		}
	  		for(uint8_t k = 0 ; k < 11 ; k++)
	  		{
	  			key_buffer[k+24] = (touchC_status & maskcode[k]) ? keycodeC[k] : 0 ;
	  		}
	  	  USBD_HID_SendReport(&hUsbDeviceFS, key_buffer, 42);
	  	  osDelay(10);
  }
  /* USER CODE END USB_task */
}

/* USER CODE BEGIN Header_MPR121_1 */
/**
* @brief Function implementing the task01 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MPR121_1 */
void MPR121_1(void *argument)
{
  /* USER CODE BEGIN MPR121_1 */
  /* Infinite loop */
  for(;;)
  {
	  touchA_status = (I2C_Read(I2C1,0X5A,1) << 8) + I2C_Read(I2C1,0X5A,0);
	  osDelay(1);
  }
  /* USER CODE END MPR121_1 */
}

/* USER CODE BEGIN Header_MPR121_2 */
/**
* @brief Function implementing the task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MPR121_2 */
void MPR121_2(void *argument)
{
  /* USER CODE BEGIN MPR121_2 */
  /* Infinite loop */
  for(;;)
  {
	  touchB_status = (I2C_Read(I2C2,0X5A,1) << 8) + I2C_Read(I2C2,0X5A,0);
	  osDelay(1);
  }
  /* USER CODE END MPR121_2 */
}

/* USER CODE BEGIN Header_MPR121_3 */
/**
* @brief Function implementing the task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MPR121_3 */
void MPR121_3(void *argument)
{
  /* USER CODE BEGIN MPR121_3 */
  /* Infinite loop */
  for(;;)
  {
	  touchC_status = (I2C_Read(I2C3,0X5A,1) << 8) + I2C_Read(I2C3,0X5A,0);
	  osDelay(1);
  }
  /* USER CODE END MPR121_3 */
}

/* USER CODE BEGIN Header_WS2812_task */
/**
* @brief Function implementing the task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_WS2812_task */
void WS2812_task(void *argument)
{
  /* USER CODE BEGIN WS2812_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END WS2812_task */
}

/* USER CODE BEGIN Header_IR_task */
/**
* @brief Function implementing the task05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_IR_task */
void IR_task(void *argument)
{
  /* USER CODE BEGIN IR_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END IR_task */
}

/* USER CODE BEGIN Header_CardReader */
/**
* @brief Function implementing the task06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CardReader */
void CardReader(void *argument)
{
  /* USER CODE BEGIN CardReader */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END CardReader */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

