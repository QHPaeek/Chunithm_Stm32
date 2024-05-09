#include "serial.h"
#include "stdio.h"
#include "usb_otg.h"

uint8_t serial_task_status = 0 ;
uint8_t slider_rst[4] = {0xFF,0x10,0x00,0xF1};
uint8_t slider_scan_stop_cmd[5] = {0xff,0x04,0x00,0xfd,0xfc};


