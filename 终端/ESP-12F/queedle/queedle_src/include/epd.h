#ifndef _EPD_
#define _EPD_

#include <Arduino.h>
#include "delay.h"
#include "spi.h"
#include "all_config.h"
#include "part.h"

void Epaper_Write_Command(unsigned char command);
void Epaper_Write_Data(unsigned char command);
// EPD
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);

void EPD_HW_Init(void); // Electronic paper initialization
void EPD_Update(void);
void EPD_DeepSleep(void);
// Display
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas);
void EPD_partial_display_Full(int x_start, int y_start, const unsigned char *BW_datas, int part_column, int part_line, char mode);
void EPD_WhiteScreen_ALL_Clean(void);

// 基本功能
void EpdClean();
void EpdDisplay(const unsigned char *buff);

//局刷
void EPD_Part_Init(void);
void EPD_SetRAMValue_BaseMap(const unsigned char *datas);
void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE);
void EPD_Dis_Part_myself(unsigned int x_startA, unsigned int y_startA, const unsigned char *datasA,
                         unsigned int x_startB, unsigned int y_startB, const unsigned char *datasB,
                         unsigned int x_startC, unsigned int y_startC, const unsigned char *datasC,
                         unsigned int x_startD, unsigned int y_startD, const unsigned char *datasD,
                         unsigned int x_startE, unsigned int y_startE, const unsigned char *datasE,
                         unsigned int PART_COLUMN, unsigned int PART_LINE);
#endif