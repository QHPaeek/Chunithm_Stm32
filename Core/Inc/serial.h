/*
 * 由ilufang撰写
 * Qinh修改
 */
#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>
enum {
  SLIDER_CMD_AUTO_SCAN = 0x01,
  SLIDER_CMD_SET_LED = 0x02,
  SLIDER_CMD_AUTO_SCAN_START = 0x03,
  SLIDER_CMD_AUTO_SCAN_STOP = 0x04,
  SLIDER_CMD_DIVA_UNK_09 = 0x09,
  SLIDER_CMD_DIVA_UNK_0A = 0x0A,
  SLIDER_CMD_RESET = 0x10,
  SLIDER_CMD_GET_BOARD_INFO = 0xF0,
};
typedef union slider_packet {
  struct
  {
    uint8_t syn;
    uint8_t cmd;
    uint8_t size;
    union
    {
      struct
      {
        uint8_t led_unk;
        uint8_t leds[96];
      };
      char version[32];
      uint8_t pressure[32];
    };
  };
  uint8_t data[128];
}
slider_packet_t;
extern slider_packet_t slider_req, slider_resp;
extern int slider_tx_pending;
extern int checkserial;
extern uint8_t slider_key_tx_buf[36];
extern uint8_t serial_task_status;
uint8_t sliderserial_readreq(uint8_t *rx_buffer,uint8_t len);//接收数据
#endif
