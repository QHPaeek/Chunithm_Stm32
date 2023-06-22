/*
 * mpr121.c
 * Created on: 2023年6月21日
 * Author: Qinh
 * 用于MPR121的LL库函数
 * 请注意，osDelay()函数只能在FreeRtos中使用，如果你不需要FreeRtos,必须将其删除。
 */
#include "mpr121.h"
#include "stm32f4xx_ll_i2c.h"
#include "main.h"
#include "i2c.h"
#include "stdint.h"
#include "cmsis_os.h"

	// MPR121 寄存器定义
	#define MHD_R    0x2B
	#define NHD_R    0x2C
	#define    NCL_R    0x2D
	#define    FDL_R        0x2E

	#define    MHD_F    0x2F
	#define    NHD_F    0x30
	#define    NCL_F        0x31
	#define    FDL_F        0x32

	#define    ELE0_T    0x41
	#define    ELE0_R    0x42
	#define    ELE1_T    0x43
	#define    ELE1_R    0x44
	#define    ELE2_T    0x45
	#define    ELE2_R    0x46
	#define    ELE3_T    0x47
	#define    ELE3_R    0x48
	#define    ELE4_T    0x49
	#define    ELE4_R    0x4A
	#define    ELE5_T    0x4B
	#define    ELE5_R    0x4C
	#define    ELE6_T    0x4D
	#define    ELE6_R    0x4E
	#define    ELE7_T    0x4F
	#define    ELE7_R    0x50
	#define    ELE8_T    0x51
	#define    ELE8_R    0x52
	#define    ELE9_T    0x53
	#define    ELE9_R    0x54
	#define    ELE10_T    0x55
	#define    ELE10_R    0x56
	#define    ELE11_T    0x57
	#define    ELE11_R    0x58

	#define    FIL_CFG    0x5D
	#define    ELE_CFG    0x5E
	#define GPIO_CTRL0    0x73
	#define    GPIO_CTRL1    0x74
	#define GPIO_DATA    0x75
	#define    GPIO_DIR    0x76
	#define    GPIO_EN        0x77
	#define    GPIO_SET    0x78
	#define    GPIO_CLEAR    0x79
	#define    GPIO_TOGGLE    0x7A
	#define    ATO_CFG0    0x7B
	#define    ATO_CFGU    0x7D
	#define    ATO_CFGL    0x7E
	#define    ATO_CFGT    0x7F

	// 全局常数（实测“3/2”参数适用3mm有机玻璃板）
	#define TOU_THRESH    3        //触摸阈值 0x0F     10//15//30//10//3
	#define    REL_THRESH    2     //释放阈值 0x0A     08//08//25//08//2


void I2C1_Write(int devaddr,int regaddr,int data)
{
	while(LL_I2C_IsActiveFlag_BUSY(I2C1)){osDelay(1);}//等待I2C总线空闲
	LL_I2C_GenerateStartCondition(I2C1);//发送开始信号
	while(!LL_I2C_IsActiveFlag_SB(I2C1)){osDelay(1);}//等待开始信号发送完成
	LL_I2C_TransmitData8(I2C1,devaddr << 1);//发送地址
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){osDelay(1);}//当发送完成后，ADDR位自动被置1.等待地址发送完成
	LL_I2C_ClearFlag_ADDR(I2C1);//清除ADDR寄存器的数值，才能进行下一步操作
	while(!LL_I2C_IsActiveFlag_TXE(I2C1)){osDelay(1);}	  //地址位置1之后TXE也会完成
	LL_I2C_TransmitData8(I2C1,regaddr);//写入寄存器地址
	while(!LL_I2C_IsActiveFlag_TXE(I2C1)){osDelay(1);}//等待从机发送应答信号
	LL_I2C_TransmitData8(I2C1,data);//发送写入的数据
	while(!LL_I2C_IsActiveFlag_BTF(I2C1)){osDelay(1);}//等待写入完成从机发送应答
	LL_I2C_GenerateStopCondition(I2C1);
}
int I2C1_Read(int devaddr,int regaddr)
{
	uint8_t data = 0;
	while(LL_I2C_IsActiveFlag_BUSY(I2C1)){}//等待I2C总线空闲
	LL_I2C_GenerateStartCondition(I2C1);//发送开始信号
	while(!LL_I2C_IsActiveFlag_SB(I2C1)){}//等待开始信号发送完成
	LL_I2C_TransmitData8(I2C1,devaddr << 1);//发送地址
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){}//当发送完成后，ADDR位自动被置1.等待地址发送完成
	LL_I2C_ClearFlag_ADDR(I2C1);//清除ADDR寄存器的数值，才能进行下一步操作
	while(!LL_I2C_IsActiveFlag_TXE(I2C1)){}	  //地址位置1之后TXE也会完成
	LL_I2C_TransmitData8(I2C1,regaddr);//写入寄存器地址
	while(!LL_I2C_IsActiveFlag_TXE(I2C1)){}//等待从机发送应答信号
//	LL_I2C_GenerateStopCondition(I2C1);
	LL_I2C_GenerateStartCondition(I2C1);//发送重启信号
	while(!LL_I2C_IsActiveFlag_SB(I2C1)){}//等待重启信号发送完成
	LL_I2C_TransmitData8(I2C1,devaddr << 1 | 0x01);//重新发送地址，并且读写位置1
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){}
	LL_I2C_AcknowledgeNextData(I2C1,LL_I2C_NACK);//发送NACK信号
	LL_I2C_ClearFlag_ADDR(I2C1);//清除ADDR位
	LL_I2C_GenerateStopCondition(I2C1);//发送停止条件
	while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){}//当RXNE位置1之后，表示数据接收完成并且放置到DR寄存器中
	data = LL_I2C_ReceiveData8(I2C1);//这里实际上是读取DR寄存器中接收到的数据
	return data;
}
int MPR121_Init(void)
{
	I2C1_Write(0x5A,0x80,0x63);
	I2C1_Write(0x5A,0x5E,0x00);
	uint8_t status = I2C1_Read(0x5A,0x5D);
	if(status != 0x24)
	{
		return(0);
	}
	  // Section A  // 该组控制过滤。 data is > baseline.
	I2C1_Write(0x5A,MHD_R, 0x01);
	I2C1_Write(0x5A,NHD_R, 0x01);
	I2C1_Write(0x5A,NCL_R, 0x00);
	I2C1_Write(0x5A,FDL_R, 0x00);

	    // Section B  // 该组控制过滤。data is < baseline.
	I2C1_Write(0x5A,MHD_F, 0x01);
	I2C1_Write(0x5A,NHD_F, 0x01);
	I2C1_Write(0x5A,NCL_F, 0xFF);
	I2C1_Write(0x5A,FDL_F, 0x00);

	    //基线过滤控制寄存器
	I2C1_Write(0x5A,0x33, 0x00);    //noise half delta touched
	I2C1_Write(0x5A,0x34, 0x00);    //noise counts touched
	I2C1_Write(0x5A,0x35, 0x00);    //filter delay touched

	I2C1_Write(0x5A,0x36, 0x0F);
	I2C1_Write(0x5A,0x37, 0x0F);
	I2C1_Write(0x5A,0x38, 0x00);
	I2C1_Write(0x5A,0x39, 0x00);
	I2C1_Write(0x5A,0x3A, 0x01);
	I2C1_Write(0x5A,0x3B, 0x01);
	I2C1_Write(0x5A,0x3C, 0xFF);
	I2C1_Write(0x5A,0x3D, 0xFF);
	I2C1_Write(0x5A,0x3E, 0x00);
	I2C1_Write(0x5A,0x3F, 0x00);
	I2C1_Write(0x5A,0x40, 0x00);

	    // Section C  // 该组为每个电极设置触摸和释放阈值
	I2C1_Write(0x5A,ELE0_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE0_R, REL_THRESH);
	I2C1_Write(0x5A,ELE1_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE1_R, REL_THRESH);
	I2C1_Write(0x5A,ELE2_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE2_R, REL_THRESH);
	I2C1_Write(0x5A,ELE3_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE3_R, REL_THRESH);
	I2C1_Write(0x5A,ELE4_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE4_R, REL_THRESH);
	I2C1_Write(0x5A,ELE5_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE5_R, REL_THRESH);
	I2C1_Write(0x5A,ELE6_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE6_R, REL_THRESH);
	I2C1_Write(0x5A,ELE7_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE7_R, REL_THRESH);
	I2C1_Write(0x5A,ELE8_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE8_R, REL_THRESH);
	I2C1_Write(0x5A,ELE9_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE9_R, REL_THRESH);
	I2C1_Write(0x5A,ELE10_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE10_R, REL_THRESH);
	I2C1_Write(0x5A,ELE11_T, TOU_THRESH);
	I2C1_Write(0x5A,ELE11_R, REL_THRESH);

	I2C1_Write(0x5A,0x59, 0x02);
	I2C1_Write(0x5A,0x5A, 0x01);
	I2C1_Write(0x5A,0x5B, 0x00);
	I2C1_Write(0x5A,0x5C, 0x3F);
	I2C1_Write(0x5A,0x5D, 0x00);

	    // 启用自动配置和自动重新配置
	I2C1_Write(0x5A,0x7B, 0x0B);
	I2C1_Write(0x5A,0x7D, 0xC8);
	I2C1_Write(0x5A,0x7E, 0x82);
	I2C1_Write(0x5A,0x7F, 0xB4);

	    //电极配置（重要）
	I2C1_Write(0x5A,0x5E, 0xBC);//1011,1100。启用基线跟踪，启用接近检测，启用所有12个电极
	return(1);
	}

