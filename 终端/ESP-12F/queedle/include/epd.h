#ifndef _EPD_
#define _EPD_

#include <Arduino.h>
#include "delay.h"
#include "spi.h"
#include "all_config.h"
// #include "part.h"
#include "WAVEFORM_SETTING_LUT.h"

void Epaper_Write_Command(unsigned char command);
void Epaper_Write_Data(unsigned char command);
// EPD
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); // Electronic paper initialization

// 全刷初始化---0
void EPD_init_Full(void);
// 速刷和局刷初始化---1
void EPD_init_Part(void);

void EPD_Update(void);
void EPD_DeepSleep(void);
// Display
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas);
void EPD_WhiteScreen_ALL_Clean(void);
void EPD_Dis_Full(const uint8_t *DisBuffer);
void EPD_Dis_Part(int xStart, int xEnd, int yStart, int yEnd, const uint8_t *DisBuffer);
// 向ram中写值 全刷--00 速刷--10 局刷--11
void EPD_Transfer_Full_BW(const uint8_t *DisBuffer);                                          // 0
void EPD_Transfer_Part(int xStart, int xEnd, int yStart, int yEnd, const uint8_t *DisBuffer); // 1
// 基本功能
void EpdClean();
void EpdDisplay(const unsigned char *buff);

// 移植

#endif