#ifndef __PAINT_PAGE__
#define __PAINT_PAGE__

#include "all_config.h"
#include "GUI_Paint.h"
#include "epd.h"

// 绘画第一页
void Page_Paint_Menu(unsigned char *BlackImage)
{
    Paint_NewImage(BlackImage, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 开始绘制

    Serial.println("\nPage Menu has painted...");
    EpdDisplay((const unsigned char *)BlackImage);
    delay(1000);
}

#endif