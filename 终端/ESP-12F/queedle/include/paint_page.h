#ifndef __PAINT_PAGE__
#define __PAINT_PAGE__

#include "all_config.h"
#include "GUI_Paint.h"
#include "epd.h"
#include "time_utils.h"

bool Is_Trans_Time();
void Page_Paint_Test(unsigned char *BlackImage);
void Page_Paint_Menu(unsigned char *BlackImage);
void Page_Paint_DailyNews(unsigned char *BlackImage, String news_data);
void Page_Paint_OCR(unsigned char *BlackImage, String news_data);
void Page_Paint_Chat(unsigned char *BlackImage, String User_data, String Robot_data);
#endif