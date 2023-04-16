#pragma once

/*
 *  函数功能:根据具体日期得到星期
 *  吉姆拉尔森公式  week=(date+2*month+3*(month+1)/5+year+year/4-y/100+y/400)%7
 *  注 : 把一月 二月看成是上一年的13 14月    ,    得到结果 0 -- 6
 * @ CSDN Tyrion.Mon
 */
// void getWEEK(struct RTC_DateTimeTypeDef *time)
// {
//     u16 YY = 0;
//     u8 MM = 0;
//     if (time->month == 1 || time->month == 2)
//     {
//         MM = time->month + 12;
//         YY = time->year - 1;
//     }
//     else
//     {
//         MM = time->month;
//         YY = time->year;
//     }
//     time->week = ((time->date + 2 * MM + 3 * (MM + 1) / 5 + YY + YY / 4 - YY / 100 + YY / 400) % 7) + 1;
// }
//(29 + 16 + 5 + 2018 +2018/4 - 2018/100 + 2018/400)%7
//(29 + 16 + 5 + 18 +18/4 - 18/100 + 18/400)%7

/**
 * @brief  UTC、GMT时间转换为北京时间
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @date   CSDN Tyrion.Mon
 */
// void GMTtoBeijingTime(struct RTC_DateTimeTypeDef *GMTtime, struct RTC_DateTimeTypeDef *Beijingtime)
// {
//     Beijingtime->year = GMTtime->year;
//     Beijingtime->month = GMTtime->month;
//     Beijingtime->date = GMTtime->date;
//     Beijingtime->hour = GMTtime->hour;
//     Beijingtime->minute = GMTtime->minute;
//     Beijingtime->second = GMTtime->second;

//     if (GMTtime->hour + 8 > 23) // 东八区已是第二天
//     {
//         Beijingtime->hour = GMTtime->hour + 8 - 24; // 东八区真实时间(小时)
//         // 大月
//         if ((GMTtime->month == 1) || (GMTtime->month == 3) || (GMTtime->month == 5) || (GMTtime->month == 7) || (GMTtime->month == 8) || (GMTtime->month == 10) || (GMTtime->month == 12))
//         {
//             if (GMTtime->date == 31) // 如果此时UTC时是大月最后一天,则东八区当前时为下一月1号
//             {
//                 Beijingtime->date = 1;
//                 Beijingtime->month++;
//                 if (Beijingtime->month > 12)
//                 // 如果此时UTC时是12月最后一天, 则东八区当前时则为下一年1月1日
//                 {
//                     Beijingtime->month = 1;
//                     Beijingtime->year++;
//                 }
//             }
//             else // 如果此时UTC时不是大月最后一天
//             {
//                 Beijingtime->date++;
//             }
//         }
//         // 二月
//         else if (GMTtime->month == 2)
//         {
//             if ((IsLeapYear(GMTtime->year) == 1 && GMTtime->date == 28) || (GMTtime->date < 28)) // 如果UTC时是闰年,且UTC时为28号,则东八区现在为29号 或者 小于28号,则加一
//             {
//                 Beijingtime->date++;
//             }
//             else if ((IsLeapYear(GMTtime->year) == 0 && Beijingtime->date == 28) || (Beijingtime->date == 29)) // 如果UTC时是平年且UTC时为28号 或者 为29号,则东八区现在为3月1日
//             {
//                 Beijingtime->date = 1;
//                 Beijingtime->month++;
//             }
//         }
//         // 小月
//         else
//         {
//             if (GMTtime->date == 30) // 如果此时UTC时是小月最后一天,则东八区当前时则为下一月1号
//             {
//                 Beijingtime->date = 1;
//                 Beijingtime->month++;
//             }
//             else // 如果此时UTC时不是小月最后一天
//             {
//                 Beijingtime->date++;
//             }
//         }
//     }
//     else // 如果此时UTC时与东八区时同一天
//     {
//         Beijingtime->hour = GMTtime->hour + 8;
//     }

//     getWEEK(Beijingtime);
// }
