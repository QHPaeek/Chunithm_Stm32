#ifndef _SERIAL_H
#define _SERIAL_H
#include <stdio.h>
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

extern int slider_tx_pending;
extern int checkserial;
extern uint8_t slider_key_tx_buf[36];
extern uint8_t serial_task_status;
extern uint8_t slider_rst[4];
extern uint8_t Buffer[256];
#endif
