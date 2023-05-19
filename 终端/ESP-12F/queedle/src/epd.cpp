#include "epd.h"
#define xDot 400
#define yDot 300

// 静态函数
static void EPD_Init(void)
{
  EPD_W21_RST_0; // Module reset
  delay(1);      // At least 10ms delay
  EPD_W21_RST_1;
  delay(1); // At least 10ms delay

  Epaper_Write_Command(0x74); //
  Epaper_Write_Data(0x54);    //
  Epaper_Write_Command(0x7E); //
  Epaper_Write_Data(0x3B);    //
  Epaper_Write_Command(0x01); //
  Epaper_Write_Data(0x2B);    //
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00); //

  Epaper_Write_Command(0x0C); //
  Epaper_Write_Data(0x8B);    //
  Epaper_Write_Data(0x9C);    //
  Epaper_Write_Data(0xD6);    //
  Epaper_Write_Data(0x0F);    //

  Epaper_Write_Command(0x3A); //
  Epaper_Write_Data(0x21);    //
  Epaper_Write_Command(0x3B); //
  Epaper_Write_Data(0x06);    //
  Epaper_Write_Command(0x3C); //
  Epaper_Write_Data(0x03);    //

  Epaper_Write_Command(0x11); // data enter mode
  Epaper_Write_Data(0x01);    // 01 –Y decrement, X increment,

  Epaper_Write_Command(0x2C); //
  Epaper_Write_Data(0x00);    // fff

  Epaper_Write_Command(0x37); //
  Epaper_Write_Data(0x00);    //
  Epaper_Write_Data(0x00);    //
  Epaper_Write_Data(0x00);    //
  Epaper_Write_Data(0x00);    //
  Epaper_Write_Data(0x80);    //

  Epaper_Write_Command(0x21); //
  Epaper_Write_Data(0x40);    //
  Epaper_Write_Command(0x22);
  Epaper_Write_Data(0xc7); // c5forgraymode//
}

static void EPD_Write(uint8_t *value, uint8_t Datalen)
{

  uint8_t i = 0;
  uint8_t *ptemp;
  ptemp = value;

  Epaper_READBUSY();

  EPD_W21_CS_0;
  EPD_W21_DC_0;      // When DC is 0, write command
  SPI_Write(*ptemp); // The first uint8_t is written with the command value
  ptemp++;
  EPD_W21_DC_1; // When DC is 1, write Data
  for (i = 0; i < Datalen - 1; i++)
  { // sub the Data
    SPI_Write(*ptemp);
    ptemp++;
  }
  Epaper_READBUSY();
  EPD_W21_CS_1;
}

static void EPD_SetRamPointer(uint16_t addrX, uint8_t addrY, uint8_t addrY1)
{
  uint8_t RamPointerX[2]; // default (0,0)
  uint8_t RamPointerY[3];
  // Set RAM X address counter
  RamPointerX[0] = 0x4e;
  RamPointerX[1] = addrX;
  // RamPointerX[1] = 0xff;
  // Set RAM Y address counter
  RamPointerY[0] = 0x4f;
  RamPointerY[1] = addrY;
  RamPointerY[2] = addrY1;
  // RamPointerY[1] = 0x2b;
  // RamPointerY[2] = 0x02;

  EPD_Write(RamPointerX, sizeof(RamPointerX));
  EPD_Write(RamPointerY, sizeof(RamPointerY));

  // Serial.printf("0x4e: %d , 0x4f: %d %d \n",RamPointerX[1],RamPointerY[1],RamPointerY[2]);
}

static void EPD_SetRamArea(uint16_t Xstart, uint16_t Xend,
                           uint8_t Ystart, uint8_t Ystart1, uint8_t Yend, uint8_t Yend1)
{

  uint8_t RamAreaX[3]; // X start and end
  uint8_t RamAreaY[5]; // Y start and end
  RamAreaX[0] = 0x44;  // command
  RamAreaX[1] = Xstart / 8;
  RamAreaX[2] = Xend / 8;
  RamAreaY[0] = 0x45; // command
  RamAreaY[1] = Ystart;
  RamAreaY[2] = Ystart1;
  RamAreaY[3] = Yend;
  RamAreaY[4] = Yend1;
  EPD_Write(RamAreaX, sizeof(RamAreaX));
  EPD_Write(RamAreaY, sizeof(RamAreaY));
}

static void EPD_WriteDispRam(unsigned int XSize, unsigned int YSize, uint8_t *Dispbuff, unsigned int offset)
{

  int i = 0, j = 0;
  Epaper_READBUSY();

  EPD_W21_DC_0; // command write
  EPD_W21_CS_0;
  SPI_Write(0x24);

  EPD_W21_DC_1; // Data write

  Dispbuff += offset;
  for (i = 0; i < YSize; i++)
  {
    for (j = 0; j < XSize; j++)
    {
      SPI_Write(pgm_read_byte(Dispbuff));
      Dispbuff++;
    }
    Dispbuff += xDot / 8 - XSize;
  }

  EPD_W21_CS_1;
}

static void EPD_Update_Part_And_Full(void)
{
  Epaper_Write_Command(0x20);
}

//////////////////////////////////////////////////////////

void Epaper_Write_Command(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_0; // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char command)
{
  SPI_Delay(1);
  EPD_W21_CS_0;
  EPD_W21_DC_1; // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////
void EPD_HW_Init(void)
{
  EPD_W21_RST_0; // Module reset
  delay(1);      // At least 10ms delay
  EPD_W21_RST_1;
  delay(1); // At least 10ms delay

  Epaper_READBUSY();          // waiting for the electronic paper IC to release the idle signal
  Epaper_Write_Command(0x12); // SWRESET
  Epaper_READBUSY();          // waiting for the electronic paper IC to release the idle signal

  Epaper_Write_Command(0x74);
  Epaper_Write_Data(0x54);
  Epaper_Write_Command(0x7E);
  Epaper_Write_Data(0x3B);

  Epaper_Write_Command(0x2B); // Reduce glitch under ACVCOM
  Epaper_Write_Data(0x04);
  Epaper_Write_Data(0x63);

  Epaper_Write_Command(0x0C); // Soft start setting
  Epaper_Write_Data(0x8B);
  Epaper_Write_Data(0x9C);
  Epaper_Write_Data(0x96);
  Epaper_Write_Data(0x0F);

  Epaper_Write_Command(0x01); // Set MUX as 300
  Epaper_Write_Data(0x2B);
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); // Data entry mode
  Epaper_Write_Data(0x01);    // 00-y降x降 01-y降x升 10-y升x降 11-y升x升

  Epaper_Write_Command(0x44);
  Epaper_Write_Data(0x00); // RAM x address start at 0
  Epaper_Write_Data(0x31); // RAM x address end at 31h(49+1)*8->400

  Epaper_Write_Command(0x45);
  Epaper_Write_Data(0x2B); // RAM y address start at 12Bh
  Epaper_Write_Data(0x01);
  Epaper_Write_Data(0x00); // RAM y address end at 00h
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x3C); // board
  Epaper_Write_Data(0x01);    // HIZ

  // 改
  Epaper_Write_Command(0x2C); // VCOM Voltage
  Epaper_Write_Data(0x70);    //

  Epaper_Write_Command(0x18);
  Epaper_Write_Data(0X80);
  Epaper_Write_Command(0x22);
  // 更改 B1全刷 B9局刷
  Epaper_Write_Data(0XB1); // Load Temperature and waveform setting.
  Epaper_Write_Command(0x20);
  Epaper_READBUSY(); // waiting for the electronic paper IC to release the idle signal

  Epaper_Write_Command(0x4E);
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);
  Epaper_Write_Data(0x2B);
  Epaper_Write_Data(0x01);
}

void EPD_init_Full(void)
{
  EPD_Init();
  EPD_Write((uint8_t *)LUTDefault_full_opm42, sizeof(LUTDefault_full_opm42));
}

void EPD_init_Part(void)
{
  EPD_Init();
  Epaper_Write_Command(0x21);
  Epaper_Write_Data(0x00);
  EPD_Write((uint8_t *)LUTDefault_part_opm42, sizeof(LUTDefault_part_opm42));
}

//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas)
{
  unsigned int i;

  Epaper_Write_Command(0x24); // write RAM for black(0)/white (1)
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    Epaper_Write_Data(pgm_read_byte(&BW_datas[i]));
  }

  /*
 Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
  for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
  {
    Epaper_Write_Data(~pgm_read_byte(&R_datas[i]));
  }
  */
  EPD_Update();
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
  Epaper_Write_Command(0x22); // Display Update Control
  Epaper_Write_Data(0xC7);
  Epaper_Write_Command(0x20); // Activate Display Update Sequence
  Epaper_READBUSY();
}

void EPD_DeepSleep(void)
{
  // Epaper_READBUSY();
  Epaper_Write_Command(0x10); // enter deep sleep
  Epaper_Write_Data(0x01);
  delay(100);
}
void Epaper_READBUSY(void)
{
  while (1)
  {                //=1 BUSY
    ESP.wdtFeed(); // 这个函数占用cpu时间长一定要及时喂狗
    if (isEPD_W21_BUSY == 0)
      break;
  }
}

void EPD_WhiteScreen_ALL_Clean(void)
{
  unsigned int i;
  Epaper_Write_Command(0x24); // write RAM for black(0)/white (1)
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    Epaper_Write_Data(0xff);
  }
  Epaper_Write_Command(0x26); // write RAM for black(0)/white (1)
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    Epaper_Write_Data(0x00);
  }
  EPD_Update();
}

void EPD_Dis_Full(const uint8_t *DisBuffer)
{

  unsigned int yStart = 0;
  unsigned int yEnd = 300 - 1;
  unsigned int xStart = 0;
  unsigned int xEnd = 400 - 1;

  EPD_SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
  EPD_SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
  EPD_WriteDispRam(xDot / 8, yDot, (uint8_t *)DisBuffer, 0);
  EPD_Update_Part_And_Full();

  Epaper_READBUSY();
  Epaper_READBUSY();
  Epaper_READBUSY();
  Epaper_READBUSY();
  EPD_SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
  EPD_WriteDispRam(xDot / 8, yDot, (uint8_t *)DisBuffer, 0);
}

void EPD_Dis_Part(int xStart, int xEnd, int yStart, int yEnd, const uint8_t *DisBuffer)
{

  int temp1 = xStart, temp2 = xEnd;
  xStart = yStart;
  xEnd = yEnd;
  yEnd = yDot - temp1 - 2;
  yStart = yDot - temp2 - 3;

  unsigned int Xsize = xEnd - xStart;
  unsigned int Ysize = yEnd - yStart + 1;
  if (Xsize % 8 != 0)
  {
    Xsize = Xsize + (8 - Xsize % 8);
  }
  Xsize = Xsize / 8;
  unsigned int offset = yStart * xDot / 8 + xStart / 8;

  unsigned long temp = yStart;
  yStart = yDot - 1 - yEnd;
  yEnd = yDot - 1 - temp;

  EPD_SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
  EPD_SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);

  EPD_WriteDispRam(Xsize, Ysize, (uint8_t *)DisBuffer, offset);

  EPD_Update_Part_And_Full();
  Epaper_READBUSY();
  Epaper_READBUSY();

  EPD_WriteDispRam(Xsize, Ysize, (uint8_t *)DisBuffer, offset);
}

void EPD_Transfer_Full_BW(const uint8_t *DisBuffer)
{

  unsigned int yStart = 0;
  unsigned int yEnd = yDot - 1;
  unsigned int xStart = 0;
  unsigned int xEnd = xDot - 1;
  unsigned long temp = yStart;

  yStart = yDot - 1 - yEnd;
  yEnd = yDot - 1 - temp;
  EPD_SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
  EPD_SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);

  EPD_WriteDispRam(xDot / 8, yDot, (uint8_t *)DisBuffer, 0);
  EPD_Update_Part_And_Full();
}

void EPD_Transfer_Part(int xStart, int xEnd, int yStart, int yEnd, const uint8_t *DisBuffer)
{

  int temp1 = xStart, temp2 = xEnd;
  xStart = yStart;
  xEnd = yEnd;
  yEnd = yDot - temp1 - 2;
  yStart = yDot - temp2 - 3;

  unsigned int Xsize = xEnd - xStart;
  unsigned int Ysize = yEnd - yStart + 1;
  if (Xsize % 8 != 0)
  {
    Xsize = Xsize + (8 - Xsize % 8);
  }
  Xsize = Xsize / 8;
  unsigned int offset = yStart * xDot / 8 + xStart / 8;

  unsigned long temp = yStart;
  yStart = yDot - 1 - yEnd;
  yEnd = yDot - 1 - temp;

  EPD_SetRamArea(xStart, xEnd, yEnd % 256, yEnd / 256, yStart % 256, yStart / 256);
  EPD_SetRamPointer(xStart / 8, yEnd % 256, yEnd / 256);
  EPD_WriteDispRam(Xsize, Ysize, (uint8_t *)DisBuffer, offset);
  EPD_Update_Part_And_Full();
  Epaper_READBUSY();
}

void EpdClean()
{
  // 清理屏幕
  EPD_HW_Init(); // Electronic paper initialization
  EPD_WhiteScreen_ALL_Clean();
  EPD_DeepSleep(); // Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
}

void EpdDisplay(const unsigned char *buff)
{
  EPD_HW_Init();
  EPD_WhiteScreen_ALL(buff); // Refresh the picture in full screen
  EPD_DeepSleep();           // Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
  delay(30);
}
