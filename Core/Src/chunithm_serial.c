//由Qinh改编自ilufang,2023.6.22
#include "main.h"
#include "usart.h"
#include "stdbool.h"
slider_packet_t slider_req, slider_resp;
int slider_tx_pending = 0;
int checkserial = 0;


void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)//USB CDC回调函数，在接收完成时执行
{
		// 发送任务通知，解除任务阻塞状态
	    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	    vTaskNotifyGiveFromISR(myTaskHandle, &xHigherPriorityTaskWoken);
	    // 如果任务通知引起了任务切换，则进行任务切换
	    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


//void USER_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口回调函数，当串口空闲中断触发时被中断处理函数调用
//{
//    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE)) //判断是否是空闲中断
 //   {
 //       __HAL_UART_CLEAR_IDLEFLAG(&huart2); //清楚空闲中断标志位
 //       uint8_t Len = 255 - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);	 //判断接收到的长度
 //       switch (sliderserial_readreq()) {
 //           case SLIDER_CMD_SET_LED:
  //            slider_set_led();
  //            break;
  //          case SLIDER_CMD_AUTO_SCAN_START:
 //             slider_scan_start();
 //             break;
 //           case SLIDER_CMD_AUTO_SCAN_STOP:
 //             slider_scan_stop();
 //             break;
 //           case SLIDER_CMD_RESET:
 //             slider_reset();
 //             break;
  //          case SLIDER_CMD_GET_BOARD_INFO:
 //             slider_get_board_info();
 //             break;
 //           default:
 //             slider_scan();
 //         }
 //   }
//}


uint8_t sliderserial_readreq() //接收数据
{
  short ptr = 0;
  uint8_t checksum;
  bool esc = false;
  uint8_t c;
  for(uint8_t i = 0 ; i < receivedLength ;i++ )
  {//如果串口有数据
    checkserial = SLIDER_CMD_AUTO_SCAN;
    c = rx_buffer[i];//读取
    // SYN 新包标记
    if (c == 0xff)
      {
      ptr = 0;
      slider_req.syn = c;
      slider_req.cmd = 0; // Unset cmd
      slider_req.size = 0xff; // Unset size
      checksum = 0xff;
      esc = false;
      continue;
      }
    // Escape
    //FD用于转义，其之后的字节加一。FF -> FD FE；FD -> FD FC，其他字节不得转义。
    if (esc)
      {
        esc = false;
        c++;
      }
    else if (c == 0xfd)
       {
          esc = true;
          continue;
       }
        checksum += c;
    if (!slider_req.cmd)
        {
          slider_req.cmd = c;
          continue;
        }
    if (slider_req.size == 0xff)
        {
          slider_req.size = c;
          ptr = 3;
          continue;
        }
    if (ptr >= 128)
        {
        // 缓冲区已满。 无效。 等待下一个SYN
        continue;
        }
        slider_req.data[ptr++] = c;
    if (ptr - 3 == slider_req.size + 1)
        {
          if (checksum == 0)
          	  {
        	  	  return slider_req.cmd;
          	  }
        }
    }
  return 0;
}
void sliderserial_writeresp() {
  short ptr = 0;
  uint8_t checksum;
  if (!slider_tx_pending)
  {
    if (slider_resp.cmd)
    {
      slider_tx_pending = slider_resp.size + 4;
      ptr = 0;
      checksum = 0;
    }
    else
    {
      return;
    }
  }
  while (slider_tx_pending)
  {
    uint8_t c;
    if (slider_tx_pending == 1)
    {
      c = -checksum;
    }
    else
    {
      c = slider_resp.data[ptr];
    }
    checksum += c;
    if ((ptr != 0 && c == 0xff) || c == 0xfd)
    {
      if (SerialUSB.availableForWrite() < 2)
        return;
      SerialUSB.write(0xfd);
      c--;
    }
    else
    {
      if (SerialUSB.availableForWrite() < 1)
        return;
    }
    SerialUSB.write(c);
    ptr++;
    slider_tx_pending--;
  }
  slider_resp.cmd = 0;
}

