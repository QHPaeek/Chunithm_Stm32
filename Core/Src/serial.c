#include "serial.h"
#include "stdio.h"
#include "usb_otg.h"

uint8_t serial_task_status = 0 ;
slider_packet_t slider_req, slider_resp;
int slider_tx_pending = 0;
int checkserial = 0;

uint8_t sliderserial_readreq(uint8_t rx_buffer[],uint8_t len) //接收数据/
{
	//uint8_t flag2 = 0x66;
	//CDC_Transmit(0,&flag2, 1);
  short ptr = 0;
  uint8_t checksum;
  uint8_t esc = 0;
  uint8_t c;
  for(uint8_t i = 0;i<len;i++)
  {
    checkserial = SLIDER_CMD_AUTO_SCAN;
    c = rx_buffer[i];
    // SYN 新包标记
    if (c == 0xff)
      {
      ptr = 0;
      slider_req.syn = c;
      slider_req.cmd = 0; // Unset cmd
      slider_req.size = 0xff; // Unset size
      checksum = 0xff;
      esc = 0;
      continue;
      }
    // Escape
    //FD用于转义，其之后的字节加一。FF -> FD FE；FD -> FD FC，其他字节不得转义。
    if (esc)
      {
        esc = 0;
        c++;
      }
        else if (c == 0xfd)
        {
          esc = 1;
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
