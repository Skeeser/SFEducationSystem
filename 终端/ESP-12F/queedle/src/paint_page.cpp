#include "paint_page.h"
#include "single_finger.h"
#include "two_finger.h"
#include "three_finger.h"

#if IFTIME
PAINT_TIME sPaint_time;
TimeClient timeClient;
TimeData timeData;
size_t last_time = -60000;
#endif

static void Page_Time_Update()
{
    timeClient.UpdateTime(timeData);
    sPaint_time.Hour = timeData.hours;
    sPaint_time.Min = timeData.minutes;
    sPaint_time.Sec = timeData.seconds;
    sPaint_time.Year = timeData.currentYear;
    sPaint_time.Month = timeData.currentMonth;
    sPaint_time.Day = timeData.monthDay;
    sPaint_time.WeekDay = timeData.weekday;
}

bool Is_Trans_Time()
{
    if (millis() - last_time > LIMIT_TIME)
    {
        last_time = millis();
        return true;
    }
    return false;
}

void Page_Paint_Test(unsigned char *BlackImage)
{

    Paint_NewImage(BlackImage, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 2.Drawing on the image 示例
    Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
    Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
    Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
    Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
    // Paint_DrawString_CN(130, 0, " 你好abc", &Font12CN, BLACK, WHITE);
    // Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);

    // Paint_DrawString_EN(10, 20, "This is Queendle!!!114514", &Font24, WHITE, BLACK);
    sPaint_time.Hour = 3;
    sPaint_time.Min = 23;
    sPaint_time.Sec = 34;
    Paint_DrawTime(200, 300, &sPaint_time, &Font16, WHITE, BLACK);

    Serial.println("\nEPD_Display");
    EpdDisplay((const unsigned char *)BlackImage);
    delay(1000);
}

// 绘画第一页
void Page_Paint_Menu(unsigned char *BlackImage)
{
    Paint_NewImage(BlackImage, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Serial.println("\nPage Menu start painting");
#if IFTIME
    // 更新时间
    Page_Time_Update();
#endif

    // 开始绘制

    Paint_DrawRectangle(1, 1, 300, 400, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(4, 101, 297, 397, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(3, 1, 105, 101, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(4, 101, 105, 397, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawRectangle(38, 35, 71, 45, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(5, 51, 104, 51, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(4, 101, 104, 101, WHITE, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    Paint_DrawLine(4, 200, 297, 200, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(4, 299, 297, 299, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawCircle(25, 18, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(25, 3, 25, 33, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(10, 18, 40, 18, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawCircle(83, 18, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(83, 3, 83, 33, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(68, 18, 98, 18, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawString_EN(20, 64, "Mode", &Font24, BLACK, WHITE);
    Paint_DrawCircle(54, 151, 49, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(126, 140, "Daily News", &Font20, WHITE, BLACK);
    Paint_DrawImage(gImage_single_finger, 20, 119, 68, 68);
    Paint_DrawCircle(54, 250, 49, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(111, 239, "Click to Read", &Font20, WHITE, BLACK);
    Paint_DrawImage(gImage_two_fingers, 20, 218, 68, 68);
    Paint_DrawCircle(54, 349, 49, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_EN(126, 338, "Communicate", &Font20, WHITE, BLACK);
    Paint_DrawImage(gImage_three_fingers, 20, 317, 68, 68);

#if IFTIME

    Serial.println("Page Menu start painting year");
    Paint_DrawYearTime(120, 16, &sPaint_time, &Font20, WHITE, BLACK);
    Serial.println("Page Menu start painting time");
    Paint_DrawTime(120, 43, &sPaint_time, &Font24, WHITE, BLACK);
    Serial.println("Page Menu start painting weekday");
    Paint_DrawWeekDay(120, 70, &sPaint_time, &Font20, WHITE, BLACK);
#endif

    Serial.println("Page Menu has painted...");
    EpdDisplay((const unsigned char *)BlackImage);
    delay(1000);
}

// 绘画新闻
void Page_Paint_DailyNews(unsigned char *BlackImage, String news_data)
{
    Paint_NewImage(BlackImage, MAX_LINE_BYTES * 8, MAX_COLUMN_BYTES, ROTATE_90, MIRROR_HORIZONTAL, WHITE);
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Serial.println("\nPage DailyNews start painting");

    Paint_DrawString_EN(2, 10, news_data.c_str(), &Font16, WHITE, BLACK);

    Serial.println("Page DailyNews has painted...");
    EpdDisplay((const unsigned char *)BlackImage);
    delay(1000);
}
