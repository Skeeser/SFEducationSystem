#ifndef _CONFIG_
#define _CONFIG_

/**
 * data
 **/
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

// IO settings
#define BUSY_Pin 5
#define RES_Pin 2
#define DC_Pin 4
#define CS_Pin 15
#define SCK_Pin 14
#define SDI_Pin 13
#define EPD_W21_MOSI_0 digitalWrite(SDI_Pin, LOW)
#define EPD_W21_MOSI_1 digitalWrite(SDI_Pin, HIGH)

#define EPD_W21_CLK_0 digitalWrite(SCK_Pin, LOW)
#define EPD_W21_CLK_1 digitalWrite(SCK_Pin, HIGH)

#define EPD_W21_CS_0 digitalWrite(CS_Pin, LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin, HIGH)

#define EPD_W21_DC_0 digitalWrite(DC_Pin, LOW)
#define EPD_W21_DC_1 digitalWrite(DC_Pin, HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin, LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin, HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)

// 400*300///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2
#define RED_MODE 3

#define MAX_LINE_BYTES 50 //=400/8
#define MAX_COLUMN_BYTES 300

#define ALLSCREEN_GRAGHBYTES 15000

// wifi开关
#define IFWIFI 0
#define IFTCP 0
#define IFTIME 0

#define LIMIT_TIME 60000 // 1min

#endif