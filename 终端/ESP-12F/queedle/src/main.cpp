#include Arduino.h>
#include "pic.h"
#include "all_config.h"
#include "delay.h"
#include "spi.h"
#include "epd.h"
#include "wifi.h"
#include "GUI_Paint.h"
// #include "part.h"
#include "paint_page.h"

// 定义wifi用户名和密码
const char *SSID = "OnePlus 8";     // 要连接的WiFi的名称
const char *PASSWORD = "zgx441427"; // 要连接的WiFi的密码
const char *HOST = "192.168.140.132";
const int TCPPORT = 8266;
const char *sendbuff = "this is queedle!";

// 全局变量定义
myWifi wifi;
String news_buff[5];

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
  static String now_page = "Menu";
  static String last_page = "";
  static String construct = "";
  static String last_construct = "";
  static int news_flag = 0;

  unsigned char *readbuff = new unsigned char[ALLSCREEN_GRAGHBYTES];
#if IFTCP // wifi功能区
  if (!wifi.client.connected())
  {
    wifi.WifiInit(SSID, PASSWORD, HOST, TCPPORT);
  }
  // TcpReadTest(readbuff);
  // 接收数据
  String receive_data = wifi.WifiTcpRead();
  // 数据分割
  int index_head = receive_data.indexOf(" ");

  if (receive_data.substring(0, index_head) == "head")
  {
    Serial.print("receive head => ");
    int index_now_page = receive_data.indexOf(" ", index_head + 1);
    now_page = receive_data.substring(index_head + 1, index_now_page);
    construct = receive_data.substring(index_now_page + 1, receive_data.length());
    Serial.println("[" + now_page + "]");
    if (now_page == "Menu")
    {
      if (now_page != last_page)
      {
        last_page = now_page;
        Page_Paint_Menu(readbuff);
      }
    }
    else if (now_page == "News")
    {
      if (now_page != last_page)
      {
        last_page = now_page;
        int i = 0;
        news_flag = 0;
        while (i != 5)
        {
          auto temp_new = wifi.WifiTcpRead();
          if (temp_new != "")
          {
            news_buff[i] = temp_new;
            i++;
          }
        }
      }
      if (last_construct != construct)
      {
        last_construct = construct;
        if (construct == "Next" || news_flag == 0)
        {
          if (news_flag == 5)
            news_flag = 0;
          Page_Paint_DailyNews(readbuff, news_buff[news_flag]);
          news_flag++;
        }
      }
    }
    else if (now_page == "Ocr")
    {
      if (now_page != last_page)
      {
        last_page = now_page;
        Page_Paint_OCR(readbuff, "Please Click what you want to Read!");
      }
      if (last_construct != construct)
      {
        last_construct = construct;
        if (construct == "Retry")
        {
          String temp_ocr = "";
          while (temp_ocr == "") // todo 超时控制
          {
            temp_ocr = wifi.WifiTcpRead();
          }

          Page_Paint_OCR(readbuff, temp_ocr); // temp_ocr
        }
      }
    }
    else if (now_page == "Chat")
    {
      if (now_page != last_page)
      {
        last_page = now_page;
        Page_Paint_Chat(readbuff, " ", " ");
      }
      if (last_construct != construct)
      {
        last_construct = construct;
        if (construct == "Start")
        {
          String temp_user = "";
          String temp_robot = "";
          while (temp_user == "") // todo 超时控制
          {
            temp_user = wifi.WifiTcpRead();
          }
          while (temp_robot == "") // todo 超时控制
          {
            temp_robot = wifi.WifiTcpRead();
          }

          Page_Paint_Chat(readbuff, temp_user, temp_robot); // temp_ocr
        }
      }
    }
  }
  else
  {
    if (now_page == "Menu" && Is_Trans_Time())
    {
      Page_Paint_Menu(readbuff);
    }
  }

#endif
  // #if IFTIME
  //   if (Is_Trans_Time())
  // #else
  //   if (IfPushTest())
  // #endif
  //   {
  //     Serial.println("\npaint txt.");
  //     // Page_Paint_Menu(readbuff);
  //     // Page_Paint_DailyNews(readbuff, "ssda");
  //   }

  delete[] readbuff;
}

// 按键测试

bool IfPushTest()
{

  if (!digitalRead(0))
  {
    ESP.wdtFeed();
    delay(5);
    return true;
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
    // EpdDisplay((const unsigned char *)readbuff);
  }
}

// void EpdPartialTest()
// {
//   unsigned char fen_L, fen_H, miao_L, miao_H;
//   //////////////////////Partial refresh time demo/////////////////////////////////////
//   EPD_HW_Init(); // Electronic paper initialization
//   // EPD_SetRAMValue_BaseMap(gImage_BW); // Partial refresh background color
//   for (fen_H = 0; fen_H < 6; fen_H++)
//   {
//     for (fen_L = 0; fen_L < 10; fen_L++)
//     {
//       for (miao_H = 0; miao_H < 6; miao_H++)
//       {
//         for (miao_L = 0; miao_L < 10; miao_L++)
//         {
//           EPD_Dis_Part_myself(16, 60, (unsigned char *)pgm_read_byte(&Num[miao_L]),          // x-A,y-A,DATA-A
//                               16, 92, (unsigned char *)pgm_read_byte(&Num[miao_H]),          // x-B,y-B,DATA-B
//                               16, 132, (unsigned char *)pgm_read_byte(gImage_numdot),        // x-C,y-C,DATA-C
//                               16, 174, (unsigned char *)pgm_read_byte(&Num[fen_L]),          // x-D,y-D,DATA-D
//                               16, 206, (unsigned char *)pgm_read_byte(&Num[fen_H]), 32, 64); // x-E,y-E,DATA-E,Resolution 32*64

//           if ((fen_L == 0) && (miao_H == 0) && (miao_L == 9))
//             EpdClean();
//         }
//       }
//     }
//   }
// }
