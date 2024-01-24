#ifndef _LED_H
#define _LED_H

#include "stdio.h"
#include "dma.h"
#include "tim.h"

extern uint8_t RGB_data_raw[200];
extern uint8_t RGB_data_temp;
extern uint16_t RGB_data_DMA_buffer[769];
extern uint16_t RST_buffer[240];

#endif
