#pragma once

// #include "spline.h"

#include <Arduino.h>
#include <SPI.h>
#include "WAVEFORM_SETTING_LUT.h"

extern uint8_t UNICODEbuffer[200];
extern String fontname;

#define EPD_CS_0 digitalWrite(_CS, LOW)
#define EPD_CS_1 digitalWrite(_CS, HIGH)
#define isEPD_CS digitalRead(_CS)

#define EPD_RST_0 digitalWrite(_RST, LOW)
#define EPD_RST_1 digitalWrite(_RST, HIGH)
#define isEPD_RST digitalRead(_RST)

#define EPD_DC_0 digitalWrite(_DC, LOW)
#define EPD_DC_1 digitalWrite(_DC, HIGH)

#define READ_EPD_BUSY digitalRead(_BUSY)

#define EPD_CLK_0 digitalWrite(_CLK, LOW)
#define EPD_CLK_1 digitalWrite(_CLK, HIGH)

#define EPD_DIN_0 digitalWrite(_DIN, LOW)
#define EPD_DIN_1 digitalWrite(_DIN, HIGH)

enum epd_type
{
  WX29 = 0, // SSD1608
  WF29 = 1,
  OPM42 = 2, // SSD1619 《=== this
  WF58 = 3,
  WF29BZ03 = 4,
  C154 = 5,
  DKE42_3COLOR = 6, // SSD1619
  DKE29_3COLOR = 7, // SSD1680
  WF42 = 8,
  WF32 = 9,
  WFT0290CZ10 = 10,    // UC8151C
  GDEY042Z98 = 11,     // SSD1683
  HINKE0266A15A0 = 12, // SSD1675
};

/*
 * 典型使用流程
 * 1.EPD_init_Full/EPD_init_Part初始化
 * 2.clearbuffer清除缓存图像
 * 3.SetFont设置字体
 * 4.fontscale=1设置字体缩放
 * 5.DrawUTF 绘制文字。图像等
 * 6.EPD_DisFull/EPD_Dis_Part更新屏幕图像
 * 7.deepsleep睡眠
 *
 * 屏幕有圆点的角为（0，0）点，x方向为垂直方向，y方向为水平方向
 */
class EPaperDrive
{
public:
  /**
   * @brief 构造函数，初始化软硬SPI及引脚定义
   *
   * @param SPIMode 选择是硬件SPI还是软件SPI，0为软件，1为硬件（不采用硬件CS）
   * @param CS CS脚
   * @param RST RST脚
   * @param DC DC脚
   * @param BUSY BUSY脚
   * @param CLK 时钟线，如果是硬件SPI可以不填
   * @param DIN 数据线，如果是硬件SPI可以不填
   */
  EPaperDrive(uint8_t CS, uint8_t RST, uint8_t DC, uint8_t BUSY, uint8_t CLK = 127, uint8_t DIN = 127);

  /**
   * @brief 构析函数，暂无作用
   *
   */
  ~EPaperDrive();

  /**
   * @brief 让墨水屏进入睡眠模式，节省功耗
   *
   */
  void deepsleep(void);

  uint8_t fontscale;                ///> 字体缩放，仅支持1，2
  uint8_t frame;                    // 用于4灰度屏，fram=0,在2bit中的第一个bit中存储图像，frame=1在第二bit种存储图像
  uint8_t EPDbuffer[400 * 300 / 8]; ///>屏幕图像
  epd_type EPD_Type;

  /**
   * @brief 设置墨水屏的型号
   *
   * @param model 墨水屏的型号，目前支持的在epd_type这个枚举里面
   */
  void EPD_Set_Model(uint8_t model);

  /**
   * @brief 墨水屏全刷的初始化
   *
   */
  void EPD_init_Full(void);

  /**
   * @brief 墨水屏局刷的初始化
   *
   */
  void EPD_init_Part(void);

  /**
   * @brief 全刷传入的图像
   *
   * @param DisBuffer 传入的图像缓存指针
   * @param Label 应该刷的图像，1为传入的缓存；2为全白；3为全黑
   */
  void EPD_Dis_Full(uint8_t *DisBuffer, uint8_t Label); // 1正常2全白3全黑

  /**
   * @brief 局刷传入的图像
   *
   * @param xStart 局刷区域x开始的坐标
   * @param xEnd 局刷区域x结束的坐标
   * @param yStart 局刷区域y开始的坐标
   * @param yEnd 局刷区域y结束的坐标
   * @param DisBuffer 传入的图像缓存指针
   * @param Label 应该刷的图像，1为传入的缓存；2为全白；3为全黑
   */
  void EPD_Dis_Part(int xStart, int xEnd, int yStart, int yEnd, uint8_t *DisBuffer, uint8_t Label);

  /**
   * @brief 直接向RAM中写全刷的黑白图片
   *
   * @param DisBuffer 应当写入的图像缓存指针
   * @param Label 应该刷的图像，1为传入的缓存；2为全白；3为全黑；4为向“OLD”RAM中写全黑
   */
  void EPD_Transfer_Full_BW(uint8_t *DisBuffer, uint8_t Label);

  /**
   * @brief 直接向RAM中写局刷的图像
   *
   * @param xStart 局刷区域x开始的坐标
   * @param xEnd 局刷区域x结束的坐标
   * @param yStart 局刷区域y开始的坐标
   * @param yEnd 局刷区域y结束的坐标
   * @param DisBuffer 传入的图像缓存指针
   * @param Label 应该刷的图像，1为传入的缓存；2为全白；3为全黑
   */
  void EPD_Transfer_Part(int xStart, int xEnd, int yStart, int yEnd, uint8_t *DisBuffer, uint8_t Label);

  /**
   * @brief 清除之前所绘的图像缓存
   *
   */
  void clearbuffer();

  /**
   * @brief 设置vcom（仅有OPM42这块屏需要在意，别的屏幕不用管）
   *
   * @param vcom vcom的等级
   */
  void EPD_Set_Contrast(uint8_t vcom);

  /**
   * @brief 墨水屏全刷更新
   *
   */
  void EPD_Update(void);

  /**
   * @brief 墨水屏局刷更新
   *
   */
  void EPD_Update_Part(void);

  /**
   * @brief 在图像缓存中画以PROGMEM类型储存图片
   *
   * @param xMove 开始的x坐标
   * @param yMove 开始的y坐标
   * @param width 图像的宽度
   * @param height 图像的高度
   * @param xbm 图像的指针
   */
  void DrawXbm_P(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const uint8_t *xbm);

  /**
   * @brief 等待BUSY信号变为空闲（最大200ms超时）
   *
   * @return bool 1代表成功，0代表超时
   */
  bool ReadBusy(void);

  /**
   * @brief 等待BUSY信号变为空闲（最大2000ms超时）
   *
   * @return bool 1代表成功，0代表超时
   */
  bool ReadBusy_long(void);

  /**
   * @brief 向墨水屏驱动芯片写入命令
   *
   * @param command 命令的值
   */
  void EPD_WriteCMD(uint8_t command);

  /**
   * @brief 向墨水屏驱动芯片写入数据
   *
   * @param data 数据的值
   */
  void EPD_WriteData(uint8_t data);

  /**
   * @brief 向墨水屏驱动芯片中写命令和数据（第一个字节为命令，剩下的均为数据）
   *
   * @param value 要写的所有字节的指针
   * @param datalen 要写的所有字节的长度
   */
  void EPD_Write(uint8_t *value, uint8_t datalen);

  void EPD_WriteDispRam_Old(unsigned int XSize, unsigned int YSize, uint8_t *Dispbuff, unsigned int offset, uint8_t label);
  void EPD_WriteDispRam(unsigned int XSize, unsigned int YSize, uint8_t *Dispbuff, unsigned int offset, uint8_t label);
  void EPD_SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1);

private:
  uint8_t _CS;
  uint8_t _RST;
  uint8_t _DC;
  uint8_t _BUSY;
  uint8_t _CLK;
  uint8_t _DIN;

  int xDot;
  int yDot;
  int16_t CurrentCursor;

  void SPI_Write(uint8_t value);
  void driver_delay_xms(unsigned long xms);

  void EPD_WriteDispRam_RED(unsigned int XSize, unsigned int YSize, uint8_t *Dispbuff, unsigned int offset, uint8_t label);
  // void EPD_SetRamArea(uint16_t Xstart,uint16_t Xend,uint8_t Ystart,uint8_t Ystart1,uint8_t Yend,uint8_t Yend1);

  void EPD_Init(void);
  void EPD_WriteCMD_p1(uint8_t command, uint8_t para);

  void EPD_SetRamArea(uint16_t Xstart, uint16_t Xend, uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1);
  void SetPixel(int16_t x, int16_t y);
  int getIcon(int weathercodeindex);
  void DrawUTF(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t *code);
  int UTFtoUNICODE(uint8_t *code);
};
