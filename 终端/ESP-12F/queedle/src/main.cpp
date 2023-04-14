#include <Arduino.h>
#include "pic.h"
#include "all_config.h"
#include "delay.h"
#include "spi.h"
#include "epd.h"
#include "wifi.h"
#include "GUI_Paint.h"
#include "part.h"

// 定义wifi用户名和密码
const char *SSID = "OnePlus 8";     // 要连接的WiFi的名称
const char *PASSWORD = "zgx441427"; // 要连接的WiFi的密码
const char *HOST = "192.168.140.132";
const int TCPPORT = 8266;
const char *sendbuff = "this is queedle!";

// wifi开关
#define IFWIFI 0
myWifi wifi;

// test
bool IfPushTest();
void TcpReadTest(unsigned char *sendbuff);
void TcpReadTest(unsigned char *readbuff);
void PaintTest(unsigned char *BlackImage);
void EpdPartialTest();

void setup()
{
  pinMode(BUSY_Pin, INPUT);
  pinMode(RES_Pin, OUTPUT);
  pinMode(DC_Pin, OUTPUT);
  pinMode(CS_Pin, OUTPUT);
  pinMode(SCK_Pin, OUTPUT);
  pinMode(SDI_Pin, OUTPUT);

  // 初始化串口
  Serial.begin(9600);
  Serial.println("\nqueedle is starting........");
#if IFWIFI
  wifi.WifiInit(SSID, PASSWORD, HOST, TCPPORT);
#endif

  EpdClean();
}

// Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/

void loop()
{
  unsigned char *readbuff = new unsigned char[ALLSCREEN_GRAGHBYTES];

#if IFWIFI // wifi功能区
  if (!wifi.client.connected())
  {
    wifi.WifiInit(SSID, PASSWORD, HOST, TCPPORT);
  }

  TcpReadTest(readbuff);

#endif

  if (IfPushTest())
  {
    Serial.println("\npaint txt.");
    PaintTest(readbuff);
    // EpdDisplay(gImage_BW);
    // void EpdPartialTest();
    // EPD_Part_Init();
    // EPD_Dis_Part(20, 40, gImage_BW, 100, 100);
  }

  delete[] readbuff;
}

// 按键测试
bool IfPushTest()
{
  static int ButtonState = 0;
  static int ButtonLastState = 0;

  ButtonState = !digitalRead(16);
  if (ButtonState != ButtonLastState)
  {
    ButtonLastState = ButtonState;
    if (ButtonState)
    {
      return true;
    }
  }

  return false;
}

// 发送测试
void TcpSendTest(const char *sendbuff)
{
  wifi.WifiTcpSend(sendbuff);
  Serial.println(sendbuff);
  delay(1500);
}

// 接收测试
void TcpReadTest(unsigned char *readbuff)
{
  if (wifi.WifiTcpRead(readbuff))
  {
    Serial.println("\nstart display");
    EpdDisplay((const unsigned char *)readbuff);
  }
}

PAINT_TIME sPaint_time;
// 绘图测试
void PaintTest(unsigned char *BlackImage)
{
  Paint_NewImage(BlackImage, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);

  // 2.Drawing on the image 示例
  // Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
  // Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
  // Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  // Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  // Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  // Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  // Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  // Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  // Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  // Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  // Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  // Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
  // Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
  // Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
  // Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
  // Paint_DrawString_CN(130, 0, " 你好abc", &Font12CN, BLACK, WHITE);
  // Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

  Paint_DrawString_EN(10, 20, "This is Queendle!!!114514", &Font24, WHITE, BLACK);
  // sPaint_time.Hour = 3;
  // sPaint_time.Min = 23;
  // sPaint_time.Sec = 34;
  // Paint_DrawTime(50, 50, &sPaint_time, &Font16, WHITE, BLACK);
  // Paint_DrawImage(gImage_BW, 10, 10, 60, 40);

  Serial.println("\nEPD_Display");
  EpdDisplay((const unsigned char *)BlackImage);
  delay(1000);
}

void EpdPartialTest()
{
  unsigned char fen_L, fen_H, miao_L, miao_H;
  //////////////////////Partial refresh time demo/////////////////////////////////////
  EPD_HW_Init();                      // Electronic paper initialization
  EPD_SetRAMValue_BaseMap(gImage_BW); // Partial refresh background color
  for (fen_H = 0; fen_H < 6; fen_H++)
  {
    for (fen_L = 0; fen_L < 10; fen_L++)
    {
      for (miao_H = 0; miao_H < 6; miao_H++)
      {
        for (miao_L = 0; miao_L < 10; miao_L++)
        {
          EPD_Dis_Part_myself(16, 60, (unsigned char *)pgm_read_byte(&Num[miao_L]),          // x-A,y-A,DATA-A
                              16, 92, (unsigned char *)pgm_read_byte(&Num[miao_H]),          // x-B,y-B,DATA-B
                              16, 132, (unsigned char *)pgm_read_byte(gImage_numdot),        // x-C,y-C,DATA-C
                              16, 174, (unsigned char *)pgm_read_byte(&Num[fen_L]),          // x-D,y-D,DATA-D
                              16, 206, (unsigned char *)pgm_read_byte(&Num[fen_H]), 32, 64); // x-E,y-E,DATA-E,Resolution 32*64

          if ((fen_L == 0) && (miao_H == 0) && (miao_L == 9))
            EpdClean();
        }
      }
    }
  }
}
