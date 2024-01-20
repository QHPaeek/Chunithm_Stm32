#ifndef SLIDER_H
#define SLIDER_H
#include "stdio.h"

void slider_set_led();
void slider_scan_start();
void slider_scan_stop();
void slider_reset();
void slider_get_board_info();
void slider_scan();

extern uint8_t slider_scan_flag;

#endif
