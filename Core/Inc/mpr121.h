/*
 * mpr121.h
 *
 *  Created on: 2023年6月21日
 *      Author: XM
 */
#include "i2c.h"
#ifndef INC_MPR121_H_
#define INC_MPR121_H_
void I2C_Write(I2C_TypeDef *I2CR,int devaddr,int regaddr,int data);
uint8_t I2C_Read(I2C_TypeDef *I2CR,int devaddr,int regaddr);
int MPR121_Init(I2C_TypeDef *I2CR);


#endif /* INC_MPR121_H_ */
