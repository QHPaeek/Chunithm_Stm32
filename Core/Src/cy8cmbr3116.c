#include "cy8cmbr3116.h"
#include "FreeRTOS.h"

uint8_t key_status[32] = {0};
extern uint8_t sensor_sys_status[32] = {0} ;
//memset(key_status1 , 0 , sizeof(key_status1));
//memset(key_status2 , 0 , sizeof(key_status2));
uint8_t KEY_ADDR[16] =
{0xba,0xbc,0xbe,0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8};
const uint8_t key_sheet[32]	//针对CY8CMBR3116模块实际传感通道与游戏内按键序号的对应表。数组顺序是0-31对应游戏内32个按键，数组数值对应key_status[32]的序号。
={21,20,23,22,24,25,27,26,29,28,30,31,19,18,17,16,5,4,7,6,9,8,10,11,12,13,15,14,2,3,0,1};
uint8_t mem_temp = 0;
uint8_t CRC_data[2] = {0x39,0xF7};

const unsigned char CY8CMBR3116_configuration[128] = {
		    0xFFu, 0xFFu, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
		    0xFFu, 0xFFu, 0xFFu, 0xFFu, 0x80u, 0x80u, 0x80u, 0x80u,
		    0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u, 0x80u,
		    0x80u, 0x80u, 0x80u, 0x80u, 0x01u, 0x00u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x80u,
		    0x05u, 0x00u, 0x00u, 0x02u, 0x00u, 0x02u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x1Eu, 0x1Eu, 0x00u,
		    0x00u, 0x1Eu, 0x1Eu, 0x00u, 0x00u, 0x00u, 0x01u, 0x01u,
		    0x00u, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
		    0xFFu, 0x00u, 0x00u, 0x00u, 0x10u, 0x00u, 0x01u, 0x58u,
		    0x00u, 0x37u, 0x01u, 0x00u, 0x00u, 0x0Au, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
		    0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xD9u, 0xE2u
		};

void key_scan()
{
	for(uint8_t i = 0;i<16;i++)
	{
		if(HAL_I2C_Mem_Read(&hi2c1,SENSOR_ADDR,KEY_ADDR[i],I2C_MEMADD_SIZE_8BIT,&mem_temp,1,100)== HAL_OK)
			{
			key_status[i] = mem_temp;
			}
	}
	for(uint8_t i = 0;i<16;i++)
	{
		if(HAL_I2C_Mem_Read(&hi2c3,SENSOR_ADDR,KEY_ADDR[i],I2C_MEMADD_SIZE_8BIT,&mem_temp,1,100)==HAL_OK)
			{
			key_status[i+16] = mem_temp;
			}
	}
}

void Sensor_softRST()
{
	uint8_t rst_cmd = 0xff;
	HAL_I2C_Mem_Write(&hi2c3,SENSOR_ADDR,CTRL_CMD,I2C_MEMADD_SIZE_8BIT,&rst_cmd,1,100);
}

void Sensor_Cfg()
{
	uint8_t sensor_sys_status = 1;
	HAL_I2C_Mem_Read(&hi2c1,SENSOR_ADDR,SYSTEM_STATUS,I2C_MEMADD_SIZE_8BIT,&sensor_sys_status,1,100);
	if (sensor_sys_status == 1)
	{
	// 首先将配置数据和 CRC 值写入到 CY8CMBR3xxx 控制器寄存器内
	while(HAL_I2C_Mem_Write(&hi2c3,SENSOR_ADDR,0x00,I2C_MEMADD_SIZE_8BIT,CY8CMBR3116_configuration,128,100)!=HAL_OK);
	uint8_t sensor_cmd = 0x03;
	while(HAL_I2C_Mem_Write(&hi2c3,SENSOR_ADDR,CTRL_CMD,I2C_MEMADD_SIZE_8BIT,&sensor_cmd,1,100)!=HAL_OK);
	osDelay(500);
	while(HAL_I2C_Mem_Read(&hi2c3,SENSOR_ADDR,CALC_CRC,I2C_MEMADD_SIZE_8BIT,CRC_data,2,100)!=HAL_OK);	//器件对该寄存器映射中的配置数据进行 CRC 校验和，然后将结果存储在 CALC_CRC 寄存器中。再读取CALC_CRC值写入CFG_CRC寄存器。该指令仅用于测试和调试，并不适用在生产配置中。
	CDC_Transmit(0, CRC_data, 2);
	osDelay(100);
	while(HAL_I2C_Mem_Write(&hi2c3,SENSOR_ADDR,CONFIG_CRC,I2C_MEMADD_SIZE_8BIT,CRC_data,2,100)!=HAL_OK);
	// 将 CMD_OP_CODE 的数值 2 写入到CTRL_CMD (0x86)寄存器内后等待 220 ms，将配置数据保存到非易失性存储器内。
	uint8_t CMD_OP_CODE = 2;
	while(HAL_I2C_Mem_Write(&hi2c3,SENSOR_ADDR,CTRL_CMD,I2C_MEMADD_SIZE_8BIT,&CMD_OP_CODE,1,100)!=HAL_OK);
	osDelay(220);
	// 再 读 取CTRL_CMD_STATUS (0x88)寄存器，以便检查配置数据是否成功被存储到非易失性存储器内
	uint8_t sensor_cmd_status = 2;
	while(HAL_I2C_Mem_Read(&hi2c3,SENSOR_ADDR,CTRL_CMD_STATUS,I2C_MEMADD_SIZE_8BIT,&sensor_cmd_status,1,100)!=HAL_OK);
		if (sensor_cmd_status == 0)
		{
			Sensor_softRST();
		}
		else
		{
			while(1)
			{
				osDelay(1000);
				uint8_t error_status = 0;
				while(HAL_I2C_Mem_Read(&hi2c3,SENSOR_ADDR,0x89,I2C_MEMADD_SIZE_8BIT,&error_status,1,100)!=HAL_OK);
				CDC_Transmit(0, &error_status, 1);
			}
		}
	// 如果成功（CTRL_CMD_STATUS 寄存器的值为 0），将 CMD_OP_CODE 的数值 255 写入到 CTRL_CMD (0x86)寄存器内来发送复位指令。
	// 如果失败（CTRL_CMD_STATUS 寄存器的值为 1），表示配置数据未被保存到非易失性存储器内。这时请读取 CTRL_CMD_ERR (0x89)寄存器，以了解保存配置数据到非易失性存储器内失败的原因。
	return;
	}

	else
	{
		return;
	}

}
