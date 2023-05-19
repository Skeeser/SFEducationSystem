#include <Arduino.h>
#include "pic.h"
#include "all_config.h"
#include "delay.h"
#include "spi.h"
#include "epd.h"
#include "wifi.h"
#include "GUI_Paint.h"
// #include "part.h"
#include "paint_page.h"
#include "EPaperDrive.h"

// test

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
void new_driver();
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

  // new_driver();
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
#if SFE // wifi功能区
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

  // TcpReadTest(readbuff);
  // Serial.print(".");
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
    Serial.println("开始全刷");
    EPD_init_Full();
    // EPD_init_Part();
    EPD_Transfer_Full_BW((const unsigned char *)readbuff);
    EPD_DeepSleep();
    delay(5000);
    Serial.println("开始局刷");
    // EPD_init_Full();
    EPD_init_Part();
    EPD_Transfer_Part(10, 43, 50, 199, (const unsigned char *)readbuff);
    EPD_DeepSleep();
  }
  // if (Is_Trans_Time())
  //   EPD_DeepSleep();
}

// 新驱动测试
void new_driver()
{
  EPD_init_Full(); // 全刷初始化，使用全刷波形
  Serial.printf("缓存图像绘制完毕，准备全刷 \n");
  EPD_Transfer_Full_BW((const unsigned char *)gImage_BW); // 将缓存中的图像传给屏幕控制芯片全刷屏幕
  EPD_DeepSleep();                                        // 让屏幕进入休眠模式
  Serial.println("全刷完毕");

  delay(5000);
  unsigned char *readbuff = new unsigned char[ALLSCREEN_GRAGHBYTES];
  Paint_NewImage(readbuff, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
  Paint_SelectImage(readbuff);
  Paint_DrawString_EN(0, 0, "waveshare", &Font16, WHITE, BLACK);

  EPD_init_Part(); // 局刷的初始化
  Serial.printf("开始局刷 \n");
  EPD_Dis_Part(10, 43, 50, 199, (const unsigned char *)readbuff); // 将缓存中的图像传给屏幕控制芯片局新屏幕
  Serial.printf("局刷结束 \n");
  EPD_DeepSleep();
  delete[] readbuff;
}