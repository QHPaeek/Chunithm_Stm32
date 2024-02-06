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
#include "adc.h"
#include "usb_otg.h"
#include "usb_device.h"
#include "cy8cmbr3116.h"
#include "serial.h"
#include "slider.h"
#include "dma.h"
#include "tim.h"
#include "LED.h"
#include "usbd_cdc_acm_if.h"
#include "usbd_hid_keyboard.h"
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
	extern USBD_HandleTypeDef hUsbDevice;
	uint8_t rxData2[USB_MAX_RECEIVE_LEN] = {0} ;
	uint32_t rxLen2  = 0 ;
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
/* Definitions for Task03 */
osThreadId_t Task03Handle;
const osThreadAttr_t Task03_attributes = {
  .name = "Task03",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task04 */
osThreadId_t Task04Handle;
const osThreadAttr_t Task04_attributes = {
  .name = "Task04",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);

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

  /* creation of Task03 */
  Task03Handle = osThreadNew(StartTask03, NULL, &Task03_attributes);

  /* creation of Task04 */
  Task04Handle = osThreadNew(StartTask04, NULL, &Task04_attributes);

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
	//uint8_t slider_key_tx_buf[36] = {0xff,0x01,0x24};
	uint8_t slider_key_tx_buf2[37] = {0xFF,0x01,0x20,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xE0 ,0x00};
  //while(slider_scan_flag)
  while(1)
  {
	osDelay(10);
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
    if(slider_key_tx_buf2[35] == 0xff)
    {
    	slider_key_tx_buf2[35] = 0xfd;
    	slider_key_tx_buf2[36] = 0xfe;
    }
    //for (uint8_t i = 0;i<32;i++)
    //{
    //	check = check -slider_key_tx_buf2[i];
    //}
    //if(check == 0)
    //{
    CDC_Transmit(0, slider_key_tx_buf2, 37);
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
    uint8_t esc = 0;
    uint8_t LED_byte_num = 0;
    int gbr_status = 0;
    for(uint8_t i = 3 ;i<200;i++)
    {
    	if(LED_byte_num >=96)
    	{
    		break;
    	}
    	if(RGB_data_raw[i] == 0xfd)
    	{
    		esc = 1;
    		continue;
    	}
    	if(esc == 1)
    	{
    		RGB_data_temp = RGB_data_raw[i] +1;
    		for(uint8_t j = 0 ;j <8;j++)
    		{
    			RGB_data_DMA_buffer[(LED_byte_num+gbr_status)*8+j] = (RGB_data_temp & (0b1<<j))? 82:24;
    		}
    		LED_byte_num ++;
    		esc = 0;
    		switch(gbr_status)
    		{
    		case 0:
    			gbr_status = 1;
    			break;
    		case 1:
    			gbr_status = -1;
    			break;
    		case -1:
    			gbr_status = 0;
    			break;
    		}
    	}
    	else
    	{
    		RGB_data_temp = RGB_data_raw[i];
    		for(uint8_t j = 0 ;j <8;j++)
    		{
    		    RGB_data_DMA_buffer[(LED_byte_num+gbr_status)*8+j] = (RGB_data_temp & (0b1<<j)) ? 82:24;
    		}
    		LED_byte_num ++;
    		switch(gbr_status){
    		case 0:
    			gbr_status = 1;
    			break;
    		case 1:
    			gbr_status = -1;
    			break;
    		case -1:
    			gbr_status = 0;
    			break;
    		}
    	}
     }
    while (LED_byte_num < 96)
    {
    	for(uint8_t j = 0 ;j <8;j++)
    	{
    	   RGB_data_DMA_buffer[LED_byte_num*8+j] = 24;
    	}
    	LED_byte_num ++;
    }
	  HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t *)RGB_data_DMA_buffer, 769);
	  HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_1, (uint32_t *)RST_buffer, 240);
	  osDelay(50);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
	uint8_t led_flag = 0;
	uint8_t led_flag_len = 0;
  for(;;)
  {
	  osDelay(1);
	  if (rxLen2 != 0)
	  {
		  if(rxData2[0] == 0xFF)
		  {
    		//for (uint8_t i = 0;i<*Len;i++)
    		//{
    		//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
    			switch (rxData2[1]){
    				case 0xf0:
    					slider_get_board_info();
    					break;
    				case 0x03:
    					slider_scan_flag = 1;
    					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
    					break;
    				case 0x04:
    					slider_scan_flag = 0;
    					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
    					break;
    				case 0x10:
    					CDC_Transmit(0, slider_rst, 4);
    					break;
    				case 0x01:
    					slider_scan_flag = 1;
    					//HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
    					break;
    				case 0x02:
    				//memcpy(RGB_data_raw,Buf,Len);
    				//CDC_Transmit(0, RGB_data, Len);
    				for(uint8_t i = 0;i< rxLen2 ;i++)
    				{
    					RGB_data_raw[i] = rxData2[i];
    					led_flag = 1;
    					led_flag_len = rxLen2;
    				}

    					//HAL_DMA_Start (&hdma_memtomem_dma2_channel0 , (uint32_t) Buf , (uint32_t) RGB_data_raw , (uint32_t) Len );
    					break;
    				default:
    					break;
    			}
    	}
    	else if (led_flag == 1)
    	{
    		for(uint8_t i = 0;i< rxLen2 ;i++)
    		{
    			RGB_data_raw[i+led_flag_len] = rxData2[i];
    			led_flag = 0;
    		}
    	}
		rxLen2 = 0;
	  }

	 // if( rxLen2 > 0 ){
	 // 			osDelay(10);
	 // 			CDC_Transmit(0, rxData2, rxLen2 );//通过usb虚拟串口发�?�回�??????
	 // 			rxLen2 = 0;
	 // 		}
  }
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
//uint32_t ADC_CHANNELS_LIST[5] = {ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7, ADC_CHANNEL_8, ADC_CHANNEL_9};
uint8_t key_list[5] = {4,5,6,7,8};
uint8_t air_data[2] = {0};
//第一位为当前要点亮的灯，第二位表示灯的状态（用于点亮ws2812）
uint8_t key_buffer[8] = {0,0,0,0,0,0,0,0};
uint32_t adc_threshold_buffer[5] = {0};
uint32_t adc_threshold[5] = {0};
  /* Infinite loop */
	for (uint8_t j = 0 ; j < 50 ; j++)
	{
		for (uint8_t k = 0; k < 5; k++)
		{
			air_data[0] = k + 1;
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,k&1);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(k>>1)&1);
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(k>>2)&1);
			osDelay(1);
			HAL_ADC_Start(&hadc1);
			osDelay(1);
			adc_threshold_buffer[k] = HAL_ADC_GetValue(&hadc1);
		}
		for(uint8_t m = 0 ; m < 5 ; m++)
		{
			if(adc_threshold[m] < adc_threshold_buffer[m])
			{
				adc_threshold[m] = adc_threshold_buffer[m];
			}
		}
	}
  while(1)
  {
	  //if(slider_scan_flag == 1){
		  for (uint8_t i = 0; i < 5; i++)
		  {
			  air_data[0] = i + 1;
			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,i&1);
			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(i>>1)&1);
			  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,(i>>2)&1);
			  osDelay(1);
			  HAL_ADC_Start(&hadc1);
			  osDelay(1);
			  uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
			  //uint8_t arr[4] = {0};
			  //    arr[0] = (adcValue >> 24) & 0xFF; // Extract the highest 8 bits
			  //    arr[1] = (adcValue >> 16) & 0xFF; // Extract the next 8 bits
			  //    arr[2] = (adcValue >> 8) & 0xFF;  // Extract the next 8 bits
			  //    arr[3] = adcValue & 0xFF;         // Extract the lowest 8 bits
			  //CDC_Transmit(0, &i, 1);

			  //CDC_Transmit(0, arr, 4);
			  key_buffer[i + 2] = adcValue <= adc_threshold[i] ? 0 : key_list[i] ;
			  air_data[1] = adcValue <= adc_threshold[i] ? air_data[1] & ~(1 << i) : air_data[1]| (1 << i) ;
		  }
		  key_buffer[0] = 0;
		  key_buffer[1] = 0;
		  USBD_HID_Keybaord_SendReport(&hUsbDevice,key_buffer,8);
	  }
	  //osDelay(50);
  //}
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

