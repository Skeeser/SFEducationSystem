#include "lcddrv.h"
const uint8_t LUT_DATA_Full[] = {
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, // LUT0: BB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, // LUT1: BW:     VS 0 ~7
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, // LUT2: WB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, // LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT4: VCOM:   VS 0 ~7

    0x03, 0x03, 0x00, 0x00, 0x02, // TP0 A~D RP0
    0x09, 0x09, 0x00, 0x00, 0x02, // TP1 A~D RP1
    0x03, 0x03, 0x00, 0x00, 0x02, // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00, // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00, // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00, // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00, // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

uint8_t LUT_DATA_Gs[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT0: BB:     VS 0 ~7
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT1: BW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT2: WB:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // LUT4: VCOM:   VS 0 ~7

    0x00, 0x01, 0x00, 0x00, 0x01, // TP0 A~D RP0
    0x00, 0x00, 0x00, 0x00, 0x00, // TP1 A~D RP1
    0x00, 0x00, 0x00, 0x00, 0x00, // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00, // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00, // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00, // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00, // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

uint8_t framebuff[15000] = {0};
uint8_t level[18] = {0,32,70,110,150,185,210,220,225,230,235,240,243,248,251,253,255,255};

void EPD_W21_WriteCMD(uint8_t cmd)
{
	HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, DC_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, &cmd, 1, 0xff);
	HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
}

void EPD_W21_WriteDATA(uint8_t dat)
{
	HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, DC_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi2, &dat, 1, 0xff);
	HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
}

void updata_LUT_Full()
{
	EPD_W21_WriteCMD(0x32);
	for(uint8_t i=0; i<70; i++)
	{
		EPD_W21_WriteDATA(LUT_DATA_Full[i]);
	}
}

void updata_LUT_Gs(uint8_t gs)
{
	EPD_W21_WriteCMD(0x32);
	for(uint8_t i=0; i<70; i++)
	{
		EPD_W21_WriteDATA(LUT_DATA_Gs[i]);
	}
}

void lcd_chkstatus(void)
{
	while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) != RESET);  //1:BUSY,0:Free 
	HAL_Delay(200);                       
}

void EPD_init(void)
{
	  HAL_GPIO_WritePin(GPIOA, RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOA, RES_Pin, GPIO_PIN_SET);
	  
  	EPD_W21_WriteCMD(0x12);			//SWRESET
		lcd_chkstatus(); //waiting for the electronic paper IC to release the idle signal

	  EPD_W21_WriteCMD(0x74);
		EPD_W21_WriteDATA(0x54);
		EPD_W21_WriteCMD(0x7E);
		EPD_W21_WriteDATA(0x3B);
		EPD_W21_WriteCMD(0x2B);  // Reduce glitch under ACVCOM	
		EPD_W21_WriteDATA(0x04);           
		EPD_W21_WriteDATA(0x63);

		EPD_W21_WriteCMD(0x0C);  // Soft start setting
		EPD_W21_WriteDATA(0x8B);           
		EPD_W21_WriteDATA(0x9C);
		EPD_W21_WriteDATA(0x96);
		EPD_W21_WriteDATA(0x0F);

		EPD_W21_WriteCMD(0x01);  // Set MUX as 300
		EPD_W21_WriteDATA(0x2B);           
		EPD_W21_WriteDATA(0x01);
		EPD_W21_WriteDATA(0x01);     

		EPD_W21_WriteCMD(0x11);  // Data entry mode
		EPD_W21_WriteDATA(0x01);         
		EPD_W21_WriteCMD(0x44); 
		EPD_W21_WriteDATA(0x00); // RAM x address start at 0
		EPD_W21_WriteDATA(0x31); // RAM x address end at 31h(49+1)*8->400
		EPD_W21_WriteCMD(0x45); 
		EPD_W21_WriteDATA(0x2B);   // RAM y address start at 12Bh     
		EPD_W21_WriteDATA(0x01);
		EPD_W21_WriteDATA(0x00); // RAM y address end at 00h     
		EPD_W21_WriteDATA(0x00);
		EPD_W21_WriteCMD(0x3C); // board
		EPD_W21_WriteDATA(0x01); // HIZ

    EPD_W21_WriteCMD(SSD1675_GATE_VOLTAGE);
		EPD_W21_WriteDATA(LUT_DATA_Full[70]);
		
		EPD_W21_WriteCMD(SSD1675_SOURCE_VOLTAGE);
		EPD_W21_WriteDATA(LUT_DATA_Full[71]);
		EPD_W21_WriteDATA(LUT_DATA_Full[72]);
		EPD_W21_WriteDATA(LUT_DATA_Full[73]);
		
		EPD_W21_WriteCMD(SSD1675_WRITE_DUMMY);
		EPD_W21_WriteDATA(LUT_DATA_Full[74]);
		
		EPD_W21_WriteCMD(SSD1675_WRITE_GATELINE);
		EPD_W21_WriteDATA(LUT_DATA_Full[75]);
		
		EPD_W21_WriteCMD(0x18);
		EPD_W21_WriteDATA(0X80);
		EPD_W21_WriteCMD(0x22);
		EPD_W21_WriteDATA(0XB1);	//Load Temperature and waveform setting.
		EPD_W21_WriteCMD(0x20);
		lcd_chkstatus(); //waiting for the electronic paper IC to release the idle signal

		EPD_W21_WriteCMD(0x4E); 
		EPD_W21_WriteDATA(0x00);
		EPD_W21_WriteCMD(0x4F); 
		EPD_W21_WriteDATA(0x2B);
		EPD_W21_WriteDATA(0x01);
}
void EPD_refresh(void)
{
	  updata_LUT_Full();
		EPD_W21_WriteCMD(0x22);			//DISPLAY REFRESH 	
		EPD_W21_WriteDATA(0xc7);
		EPD_W21_WriteCMD(0x20);
		lcd_chkstatus(); //waiting for the electronic paper IC to release the idle signal
}	

void EPD_refresh_Gs(void)
{
	  updata_LUT_Gs(0);
		EPD_W21_WriteCMD(0x22);			//DISPLAY REFRESH 	
		EPD_W21_WriteDATA(0xc7);
		EPD_W21_WriteCMD(0x20);
		lcd_chkstatus(); //waiting for the electronic paper IC to release the idle signal
}	

void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X10);  	//deep sleep
		EPD_W21_WriteDATA(0x01);
}

void PIC_clean(void)
{
    unsigned int i;
		EPD_W21_WriteCMD(0x24);	       //Transfer BW data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA(0xff);
	}
		EPD_W21_WriteCMD(0x26);		     //Transfer RED data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA(0);  
	}
}


void PIC_display(const unsigned char* picData_BW,const unsigned char* picData_R)
{
    unsigned int i;
		EPD_W21_WriteCMD(0x24);	       //Transfer BW data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA((*picData_BW));
	  picData_BW++;
	}
		EPD_W21_WriteCMD(0x26);		     //Transfer RED data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA(0);  
	  picData_R++;
	}
}

void PIC_display_Clean(void)
{
    unsigned int i;
		EPD_W21_WriteCMD(0x24);	       //Transfer BW data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA(0xff);
	}
	
		EPD_W21_WriteCMD(0x26);		     //Transfer RED data
	  for(i=0;i<15000;i++)	     
	{
	  EPD_W21_WriteDATA(0x00);
	}
}

void PIC_display_Gs(void)
{
	uint8_t gs = 0;
	uint16_t frame_buff=0;
	uint8_t c1, c2, c3, c4, c5, c6, c7;
	uint8_t a1, a2, a3, a4, a5, a6, a7;
	for(gs = 0; gs < 16; gs++)
	{
		LUT_DATA_Gs[39] = gs ;
		for(frame_buff = 0; frame_buff < 15000; frame_buff++)
		{ 
			a1 = 0;
			for(c1 = 0; c1 < 8; c1++)
			{
				if((gImage_black1[frame_buff * 8 + c1] > level[gs])&&(gImage_black1[frame_buff * 8 + c1] <= level[gs+1]))
				 a1 = a1 |(0x80>>(c1));	
			}
			framebuff[frame_buff] = a1;
		}
		PIC_display(framebuff, framebuff);
		EPD_refresh_Gs();
	}
}
