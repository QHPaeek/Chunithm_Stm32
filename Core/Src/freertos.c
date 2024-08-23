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
	//volatile unsigned long  ulHighFrequencyTimerTicks = 0ul;
	extern USBD_HandleTypeDef hUsbDevice;
	uint8_t rxData[255] = {0} ;
	uint32_t rxLen  = 0 ;
	uint8_t rxData2[254];
	uint32_t rxLen2;
	uint8_t Air_key_buffer = 0;
	uint8_t Air_scan_flag = 0;
	uint8_t Air_CMD[5] = {0xff,0x05,0x01,0x00,0x00};
	uint8_t led_count = 255;
	//LED状�?�计数，每个刷灯周期-1
	uint8_t system_status = 0;
	//system_status:
	//0:ground-keyboard air-none
	//1:ground-usbcdc air-keyboard
	//2:ground-usbcdc air-usbcdc
	uint8_t groundslider_keycode[32] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35};
	uint8_t ir_keycode[6] = {56,55,49,51,48,47};
	uint8_t key_buffer[42] = {0};
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
void  delay_us(uint16_t us)
{
	__HAL_TIM_SetCounter(&htim4,0);
	__HAL_TIM_ENABLE(&htim4);

	while(__HAL_TIM_GetCounter(&htim4)<us);

	__HAL_TIM_DISABLE(&htim4);
}

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
	osDelay(8);
	Sensor_Cfg(&hi2c1);
	Sensor_Cfg(&hi2c3);
	uint8_t slider_key_tx_buf2[38] = {0xFF,0x01,0x21,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
			,0x00,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xE0 ,0x00};
	uint8_t Last_raw_key_value[32] = {0};
  while(1)
  {
	osDelay(5);
	uint8_t chksum = 0xff+0x01+0x20;
	key_scan();
	for(uint8_t i = 0 ;i<32;i++)
	{
		//对中央接缝处4个按键做防误触发滤波。误触发的表现是突然�??0跳变�??255且不持续，只能读到一次�??
		if(((i == 0) || (i == 1) || (i == 20) || (i == 21)))
		{
			if((Last_raw_key_value[i] == 0) && (key_status[i] == 0xff)){
				Last_raw_key_value[i] = 0xff;
				key_status[i] = 0;
			}else{
				Last_raw_key_value[i] = key_status[i];
			}
		}
		if(key_status[i] < 64){
			key_status[i] = 0;
		}else if(key_status[i] >192){
			key_status[i] = 0xfe;
		}else{
			key_status[i] = (key_status[i] - 64) * 2;
		}
		if(key_status[i] == 0xfd && key_status[i] == 0xff)
		{
			key_status[i] = 0xfe;
		}
		chksum += key_status[i];
	}
	slider_key_tx_buf2[35] = Air_key_buffer;
	chksum += Air_key_buffer;
	for(uint8_t i = 0 ;i<32;i++)
	{
		slider_key_tx_buf2[i+3] = key_status[key_sheet[i]];
	}
	slider_key_tx_buf2[36] = 0-chksum;
	if(slider_key_tx_buf2[36] == 0xff)
	{
		slider_key_tx_buf2[36] = 0xfd;
		slider_key_tx_buf2[37] = 0xfe;
	}
	uint8_t CDC_Trans_Buffer[42];
	if(system_status && slider_scan_flag){
		memcpy(CDC_Trans_Buffer,slider_key_tx_buf2, 38);
		//memcpy(CDC_Trans_Buffer+37,Air_CMD, 5);
		CDC_Transmit(0, CDC_Trans_Buffer, 38);
	}
//	else if(system_status == 1 && slider_scan_flag == 1){
//		memcpy(CDC_Trans_Buffer,slider_key_tx_buf2, 37);
//		memset(key_buffer,0,42);
//		for(uint8_t i = 0 ;i<6;i++){
//			key_buffer[i+2] = (Air_key_buffer & (1<<i)) ? ir_keycode[i] : 0;
//		}
//		CDC_Transmit(0, CDC_Trans_Buffer, 37);
//		USBD_HID_Keybaord_SendReport(&hUsbDevice, key_buffer, 42);
//  	  }
	else if(system_status == 0){
		memset(key_buffer,0,42);
		for(uint8_t i = 0 ;i<32;i++){
			key_buffer[i+2] = key_status[key_sheet[i]] > 128 ? groundslider_keycode[i] : 0;
		}
		USBD_HID_Keybaord_SendReport(&hUsbDevice, key_buffer, 42);
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
	osDelay(2000);
	uint16_t adcValue[6] = {0};
	uint16_t last_adcValue[6] = {0};
	//uint32_t slope_threshold = 150;
	uint16_t slope_slide_threshold = 200;
	int slope[6] = {0};
	int Last_slope[6] = {0};
	uint8_t slope_status[6] = {0};
	int slope_accumulator[6] = {0};
	for (uint8_t i = 0; i < 6; i++){
		//�???????74hc238写入3bit选择点亮哪一颗灯
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,(i&0b00000001)? 1 : 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,(i&0b00000010)? 1 : 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,(i&0b00000100)? 1 : 0);
		//stm32的ADC会自动循�??????
		osDelay(1);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1);
		adcValue[i] = HAL_ADC_GetValue(&hadc1);
	}
  while(1)
  {
	for (uint8_t i = 0; i < 6; i++){
		//�???????74hc238写入3bit选择点亮哪一颗灯
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,(i&0b00000001)? 1 : 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,(i&0b00000010)? 1 : 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,(i&0b00000100)? 1 : 0);
		//stm32的ADC会自动循�??????
		last_adcValue[i] = adcValue[i];
		Last_slope[i] = slope[i];
		delay_us(100);
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 1);
		adcValue[i] = HAL_ADC_GetValue(&hadc1);
//		if(i == 5){
//			uint8_t adc_char[10] = {0};
//			sprintf((char *)adc_char,"%ld\n",adcValue[i]);
//			uint8_t len_adc = strlen(adc_char);
//			CDC_Transmit(0,adc_char,len_adc);
//		}
		slope[i] = adcValue[i] - last_adcValue[i];

		if(slope[i] > 0){
			slope_accumulator[i] += slope[i];
		}else if(slope[i] < 0){
			slope_accumulator[i] += slope[i];
		}

		if(slope_accumulator[i] > 0 && slope[i] <= 50){
			slope_accumulator[i] = slope[i];
		}else if(slope_accumulator[i] < 0 && slope[i] >= 50){
			slope_accumulator[i] = slope[i];
		}

		if(slope_accumulator[i] > slope_slide_threshold){
			Air_key_buffer = Air_key_buffer | (1 << i);
			//slope_accumulator[i] = 0;
		}else if(slope_accumulator[i] < (0 - slope_slide_threshold)){
			Air_key_buffer = Air_key_buffer & ~(1 << i);
			//slope_accumulator[i] = 0;
		}
	}
	//指令打包
	Air_CMD[3] = Air_key_buffer;
	Air_CMD[4] = 0 - (Air_key_buffer + 0xff + 0x05 + 0x01);
	osDelay(2);
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
//	uint8_t led_flag = 0;
//	uint8_t led_flag_len = 0;
  for(;;)
  {
	osDelay(5);
	if (rxLen != 0){
		if(rxData[0] == 0xFF){
			switch (rxData[1]){
				case 0xf0:
					slider_get_board_info();
					break;
				case 0x03:
					slider_scan_flag = 1;
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
					break;
				case 0x04:
					slider_scan_flag = 2;
					CDC_Transmit(0, slider_scan_stop_cmd, 5);
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,1);
					break;
				case 0x10:
					CDC_Transmit(0, slider_rst, 4);
					break;
				case 0x01:
					slider_scan_flag = 1;
					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,0);
					break;
				case 0x02:
					memcpy(RGB_data_raw,rxData,rxLen);
					break;
				case 0x06:
					Air_scan_flag = 1;
					break;
//				case 0x07:
//					Air_scan_flag = 0;
//					break;
				default:
					break;
			}
    	}
		led_count = 255;
		rxLen = 0;
		if(Air_scan_flag){
			system_status = 2;
		}else if(slider_scan_flag){
			system_status = 1;
		}
	  }
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
	//pwm计数�??????120重载，设置为120即一直低电平，设置为90表示ws2812的高，设置为30表示ws2812的低
	//ws2812传输数据顺序要求是GRB,游戏下发数据为RGB,�??????要对调顺�??????
	RGB_data_DMA_buffer[969] = 120;
	RGB_Air_DMA_buffer[591] = 120;
	for(;;){
		if(led_count == 0){
			system_status = 0;
			slider_scan_flag = 0;
			Air_scan_flag = 0;
			//减到0：没有接到指令，说明游戏已经�??出，切换到模�??0
		}else{
			led_count--;
		}
		if(system_status == 0){
			for(uint8_t i = 0 ;i<31;i++)
		    {
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3)*8+j+224] = 30;
		    	}
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3+1)*8+j+224] = 30;
		    	}
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3+2)*8+j+224] = 90;
		    	}
		    }
			for(uint8_t i = 0 ;i<16;i++)
		    {
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*6+1)*8+j+224] = ((key_status[key_sheet[2*i]] > 128)|(key_status[key_sheet[2*i+1]] > 128)) ? 90:30;
		    	}
		    }

		}else{
			for(uint8_t i = 0 ;i<31;i++)
		    {
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3)*8+j+224] = (gamma8[RGB_data_raw[4+i*3+2]] & (1<<j)) ? 90:30;
		    	}
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3+1)*8+j+224] = (gamma8[RGB_data_raw[4+i*3+1]] & (1<<j)) ? 90:30;
		    	}
		    	for(uint8_t j = 0 ;j <8;j++)
		    	{
		    		RGB_data_DMA_buffer[(i*3+2)*8+j+224] = (gamma8[RGB_data_raw[4+i*3]] & (1<<j)) ? 90:30;
		    	}
		    }
		}
	    for(uint8_t i = 0 ;i<6;i++)
	    {
	    	if(Air_key_buffer & (1<<i)){
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3)*8+j+224] = 90;
	    		}
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3+1)*8+j+224] =90;
	    		}
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3+2)*8+j+224] =90;
	    		}
	    	}
	    	else{
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3)*8+j+224] = 30;
	    		}
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3+1)*8+j+224] =30;
	    		}
	    		for(uint8_t j = 0 ;j <8;j++)
	    		{
	    			RGB_Air_DMA_buffer[(i*3+2)*8+j+224] =90;
	    		}
	    	}
	    }
	    //�??????启DMA传输刷灯
		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)RGB_data_DMA_buffer, 970);
		HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t *)RGB_Air_DMA_buffer, 592);
		osDelay(10);
	  }
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

