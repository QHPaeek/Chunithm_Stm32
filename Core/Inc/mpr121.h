/*
 * mpr121.h
 *
 *  Created on: 2023年6月21日
 *      Author: XM
 */

#ifndef INC_MPR121_H_
#define INC_MPR121_H_
void I2C1_Write(int devaddr,int regaddr,int data);
int I2C1_Read(int devaddr,int regaddr);
int MPR121_Init();


#endif /* INC_MPR121_H_ */
