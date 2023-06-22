//由Qinh改编自ilufang,2023.6.22
#include "main.h"
#include "usart.h"
#include "stdbool.h"
slider_packet_t slider_req, slider_resp;
int slider_tx_pending = 0;
int checkserial = 0;
uint8_t sliderserial_readreq() //接收数据
{
  short ptr = 0;
  uint8_t checksum;
  bool esc = false;
  uint8_t c;
  while (SerialUSB.available())
  {//如果串口有数据
    checkserial = SLIDER_CMD_AUTO_SCAN;
    c = SerialUSB.read();//读取
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
      }   else if (c == 0xfd)
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

