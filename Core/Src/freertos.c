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
#include "usb_otg.h"
#include "usb_device.h"
#include "cy8cmbr3116.h"
#include "serial.h"
#include "slider.h"
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

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task02 */
osThreadId_t Task02Handle;
const osThreadAttr_t Task02_attributes = {
  .name = "Task02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

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
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of Task02 */
  Task02Handle = osThreadNew(StartTask02, NULL, &Task02_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	osDelay(5);
	Sensor_Cfg();
	uint8_t slider_key_tx_buf[36] = {0xff,0x01,0x24};
	uint8_t slider_key_tx_buf2[36] = {0xFF,0x01,0x20,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xE0};
  //while(slider_scan_flag)
  while(1)
  {
	osDelay(3);
	uint8_t check = 0;
	if(slider_scan_flag == 1)
	{
	int chksum = 0xff+0x01+0x20;
    key_scan();
    for(uint8_t i = 0 ;i<32;i++)
    {
    	if(key_status[i] == 0xff)
    	{
    		key_status[i] = 0xfe;
    	}
    	else if(key_status[i] == 0xfd)
    	{
    	    key_status[i] = 0xfe;
    	}
    	chksum += key_status[i];
    }
    for(uint8_t i = 0 ;i<32;i++)
        {
    		slider_key_tx_buf2[i+3] = key_status[key_sheet[i]];
        }
    slider_key_tx_buf2[35] = 0-chksum;
    //for (uint8_t i = 0;i<32;i++)
    //{
    //	check = check -slider_key_tx_buf2[i];
    //}
    //if(check == 0)
    //{
    CDC_Transmit(0, slider_key_tx_buf2, 36);
    //}
    //osDelay(5);
    //slider_get_board_info();
    //slider_scan_flag = 0;
    //HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
	}
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(5);
    switch (serial_task_status)
    {
        case SLIDER_CMD_SET_LED:
          slider_set_led();
          break;
        case SLIDER_CMD_AUTO_SCAN_START:
          slider_scan_start();
          break;
        case SLIDER_CMD_AUTO_SCAN_STOP:
          slider_scan_stop();
          break;
        case SLIDER_CMD_RESET:
          slider_reset();
          break;
        case SLIDER_CMD_GET_BOARD_INFO:
          slider_get_board_info();
          break;
        default:
        	break;
    }
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

