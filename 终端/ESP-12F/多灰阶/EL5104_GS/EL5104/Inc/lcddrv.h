#ifndef __LCDDRV_H
#define __LCDDRV_H
#include "gpio.h"
#include "spi.h"
#include "pic.h"

#define SSD1675_DRIVER_CONTROL 0x01
#define SSD1675_GATE_VOLTAGE 0x03
#define SSD1675_SOURCE_VOLTAGE 0x04
#define SSD1675_DEEP_SLEEP 0x10
#define SSD1675_DATA_MODE 0x11
#define SSD1675_SW_RESET 0x12
#define SSD1675_HV_READY 0x14
#define SSD1675_VCI_READY 0x15
#define SSD1675_TEMP_WRITE 0x1A
#define SSD1675_MASTER_ACTIVATE 0x20
#define SSD1675_DISP_CTRL1 0x21
#define SSD1675_DISP_CTRL2 0x22
#define SSD1675_WRITE_RAM1 0x24
#define SSD1675_WRITE_RAM2 0x26
#define SSD1675_WRITE_VCOM 0x2C
#define SSD1675_READ_OTP 0x2D
#define SSD1675_READ_STATUS 0x2F
#define SSD1675_WRITE_LUT 0x32
#define SSD1675_WRITE_DUMMY 0x3A
#define SSD1675_WRITE_GATELINE 0x3B
#define SSD1675_WRITE_BORDER 0x3C
#define SSD1675_SET_RAMXPOS 0x44
#define SSD1675_SET_RAMYPOS 0x45
#define SSD1675_SET_RAMXCOUNT 0x4E
#define SSD1675_SET_RAMYCOUNT 0x4F
#define SSD1675_SET_ANALOGBLOCK 0x74
#define SSD1675_SET_DIGITALBLOCK 0x7E

void EPD_init(void);
void EPD_refresh(void);
void EPD_refresh_Gs(void);
void EPD_sleep(void);
void PIC_display(const unsigned char* picData_BW,const unsigned char* picData_R);
void PIC_display_Clean(void);
void updata_LUT_Gs(uint8_t gs);
void updata_LUT_Full(void);
void PIC_clean(void);
void PIC_display_Gs(void);
#endif
