#include "epd.h"

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

  //改
  Epaper_Write_Command(0x2C); // VCOM Voltage
  Epaper_Write_Data(0x70);    //

  Epaper_Write_Command(0x18);
  Epaper_Write_Data(0X80);
  Epaper_Write_Command(0x22);
  //更改 B1全刷 B9局刷
  Epaper_Write_Data(0XB1); // Load Temperature and waveform setting.
  Epaper_Write_Command(0x20);
  Epaper_READBUSY(); // waiting for the electronic paper IC to release the idle signal

  Epaper_Write_Command(0x4E);
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);
  Epaper_Write_Data(0x2B);
  Epaper_Write_Data(0x01);
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
  Epaper_Write_Command(0x10); // enter deep sleep
  Epaper_Write_Data(0x01);
  delay(100);
}
void Epaper_READBUSY(void)
{
  while (1)
  {                //=1 BUSY
    ESP.wdtFeed(); //这个函数占用cpu时间长一定要及时喂狗
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

void EpdClean()
{
  // 清理屏幕
  EPD_HW_Init(); // Electronic paper initialization
  EPD_WhiteScreen_ALL_Clean();
  EPD_DeepSleep(); // Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
}

void EpdDisplay(const unsigned char *buff)
{
  // wifi.WifiTcpSend((const char *)readbuff);
  //    Full screen refresh
  EPD_HW_Init();             // Electronic paper initialization
  EPD_WhiteScreen_ALL(buff); // Refresh the picture in full screen
  EPD_DeepSleep();           // Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
  delay(30);
}

/*-------------------------局刷实现---------------------------*/
void EPD_select_LUT(unsigned char *wave_data)
{
  unsigned char count;
  Epaper_Write_Command(0x32);
  for (count = 0; count < 70; count++)
    Epaper_Write_Data(wave_data[count]);
}
///////////////////////////Part update//////////////////////////////////////////////
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/
void EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22); // Display Update Control
  Epaper_Write_Data(0x0C);
  Epaper_Write_Command(0x20); // Activate Display Update Sequence
  Epaper_READBUSY();
}
// The x axis is reduced by one byte, and the y axis is reduced by one pixel.
void EPD_SetRAMValue_BaseMap(const unsigned char *datas)
{
  unsigned int i;
  const unsigned char *datas_flag;
  datas_flag = datas;

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    Epaper_Write_Data(*datas);
    datas++;
  }
  datas = datas_flag;
  Epaper_Write_Command(0x26); // Write Black and White image to RAM
  for (i = 0; i < ALLSCREEN_GRAGHBYTES; i++)
  {
    Epaper_Write_Data(*datas);
    datas++;
  }
  EPD_Update();
  EPD_Part_Init();
}

void EPD_Part_Init(void)
{

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

  Epaper_Write_Command(0x3C); // board
  Epaper_Write_Data(0x01);    // HIZ

  //改
  Epaper_Write_Command(0x2C); // VCOM Voltage
  Epaper_Write_Data(0x70);    //

  Epaper_Write_Command(0x18);
  Epaper_Write_Data(0X80);
  Epaper_Write_Command(0x22);
  //更改 B1全刷 B9局刷
  Epaper_Write_Data(0XB1); // Load Temperature and waveform setting.
  Epaper_Write_Command(0x20);
  Epaper_READBUSY(); // waiting for the electronic paper IC to release the idle signal

  Epaper_Write_Command(0x2C); // VCOM Voltage
  Epaper_Write_Data(0x26);
}
void EPD_Dis_Part(unsigned int x_start, unsigned int y_start, const unsigned char *datas, unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i;
  unsigned int x_end, y_start1, y_start2, y_end1, y_end2;
  x_start = x_start / 8;
  x_end = x_start + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_start;
  if (y_start >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_start + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);  // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   // ????=0

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_start);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datas);
    datas++;
  }
  EPD_Update();
}

/////////////////////////////////////TIME///////////////////////////////////////////////////
void EPD_Dis_Part_myself(unsigned int x_startA, unsigned int y_startA, const unsigned char *datasA,
                         unsigned int x_startB, unsigned int y_startB, const unsigned char *datasB,
                         unsigned int x_startC, unsigned int y_startC, const unsigned char *datasC,
                         unsigned int x_startD, unsigned int y_startD, const unsigned char *datasD,
                         unsigned int x_startE, unsigned int y_startE, const unsigned char *datasE,
                         unsigned int PART_COLUMN, unsigned int PART_LINE)
{
  unsigned int i;
  unsigned int x_end, y_start1, y_start2, y_end1, y_end2;

  // Data A////////////////////////////
  x_startA = x_startA / 8; // Convert to byte
  x_end = x_startA + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_startA - 1;
  if (y_startA >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_startA + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA); // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_startA);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datasA);
    datasA++;
  }
  // Data B/////////////////////////////////////
  x_startB = x_startB / 8; // Convert to byte
  x_end = x_startB + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_startB - 1;
  if (y_startB >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_startB + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB); // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_startB);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datasB);
    datasB++;
  }

  // Data C//////////////////////////////////////
  x_startC = x_startC / 8; // Convert to byte
  x_end = x_startC + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_startC - 1;
  if (y_startC >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_startC + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC); // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_startC);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datasC);
    datasC++;
  }

  // Data D//////////////////////////////////////
  x_startD = x_startD / 8; // Convert to byte
  x_end = x_startD + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_startD - 1;
  if (y_startD >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_startD + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD); // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_startD);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datasD);
    datasD++;
  }
  // Data E//////////////////////////////////////
  x_startE = x_startE / 8; // Convert to byte
  x_end = x_startE + PART_LINE / 8 - 1;

  y_start1 = 0;
  y_start2 = y_startE - 1;
  if (y_startE >= 256)
  {
    y_start1 = y_start2 / 256;
    y_start2 = y_start2 % 256;
  }
  y_end1 = 0;
  y_end2 = y_startE + PART_COLUMN - 1;
  if (y_end2 >= 256)
  {
    y_end1 = y_end2 / 256;
    y_end2 = y_end2 % 256;
  }

  Epaper_Write_Command(0x44);  // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE); // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128
  Epaper_Write_Command(0x45);  // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2); // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1); // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);   // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);

  Epaper_Write_Command(0x4E); // set RAM x address count to 0;
  Epaper_Write_Data(x_startE);
  Epaper_Write_Command(0x4F); // set RAM y address count to 0X127;
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);

  Epaper_Write_Command(0x24); // Write Black and White image to RAM
  for (i = 0; i < PART_COLUMN * PART_LINE / 8; i++)
  {
    Epaper_Write_Data(*datasE);
    datasE++;
  }
  EPD_Part_Update();
}
