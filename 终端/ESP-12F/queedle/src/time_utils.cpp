#include "time_utils.h"

/**
 * @brief  判断闰年平年
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @date
 */
static uint8_t IsLeapYear(uint16_t year)
{
	if (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 400 == 0)
		return 1; // 是闰年
	return 0;	  // 是平年
}

/**
 * @brief  时间戳转换为普通时间
 * @note
 * @param  None
 * @retval None
 * @author PWH
 * @ CSDN Tyrion.Mon
 */
static void TimestampToNormalTime(struct TimeData *time, unsigned long Timestamp)
{

	uint16_t year = 1970;
	uint32_t Counter = 0, CounterTemp; // 随着年份迭加，Counter记录从1970 年 1 月 1 日（00:00:00 GMT）到累加到的年份的最后一天的秒数
	uint8_t Month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	uint8_t i;

	while (Counter <= Timestamp) // 假设今天为2018年某一天，则时间戳应小于等于1970-1-1 0:0:0 到 2018-12-31 23:59:59的总秒数
	{
		CounterTemp = Counter; // CounterTemp记录完全1970-1-1 0:0:0 到 2017-12-31 23:59:59的总秒数后退出循环
		Counter += 31536000;   // 加上今年（平年）的秒数
		if (IsLeapYear(year))
		{
			Counter += 86400; // 闰年多加一天
		}
		year++;
	}
	time->currentYear = year - 1; // 跳出循环即表示到达计数值当前年
	Month[1] = (IsLeapYear(time->currentYear) ? 29 : 28);
	Counter = Timestamp - CounterTemp; // Counter = Timestamp - CounterTemp  记录2018年已走的总秒数
	CounterTemp = Counter / 86400;	   // CounterTemp = Counter/(24*3600)  记录2018年已【过去】天数
	Counter -= CounterTemp * 86400;	   // 记录今天已走的总秒数
	// time->hours = Counter / 3600;        // 时      				 得到今天的小时
	// time->minutes = Counter % 3600 / 60; // 分
	// time->seconds = Counter % 60;        // 秒
	for (i = 0; i < 12; i++)
	{
		if (CounterTemp < Month[i]) // 不能包含相等的情况，相等会导致最后一天切换到下一个月第一天时
		{
			// （即CounterTemp已走天数刚好为n个月完整天数相加时（31+28+31...）），
			time->currentMonth = i + 1;		  // 月份不加1，日期溢出（如：出现32号）
			time->monthDay = CounterTemp + 1; // 应不作处理，CounterTemp = Month[i] = 31时，会继续循环，月份加一，
			break;							  // 日期变为1，刚好符合实际日期
		}
		CounterTemp -= Month[i];
	}
	// getWEEK(time);
}

TimeClient::TimeClient()
{
	timeClient = new NTPClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
	timeClient->begin();
}

void TimeClient::UpdateTime(TimeData &data)
{
	Serial.println("\nTime is updating......");
	Serial.print("\nmillis is => ");
	Serial.println(millis());
	if (millis() - t > interval)
	{
		ESP.wdtFeed();
		delay(500);
		timeClient->update();
		unsigned long epochTime = timeClient->getEpochTime(); // todo utc时间转化
		data.weekday = timeClient->getDay();
		data.hours = timeClient->getHours();
		data.minutes = timeClient->getMinutes();
		data.seconds = timeClient->getSeconds();

		TimestampToNormalTime(&data, epochTime);

		t = millis();
		last_sec = millis();

		Serial.println(data.currentYear);
		Serial.println(data.currentMonth);
		Serial.println(data.monthDay);
	}
	else
	{
		data.seconds += ((millis() - last_sec) / 1000);
		last_sec = millis();

		if (data.seconds > 59)
		{
			// data.minutes++;

			data.minutes += data.seconds / 60;
			data.seconds %= 60;
		}

		if (data.minutes > 59)
		{
			data.hours += data.minutes / 60;
			data.minutes %= 60;
		}

		if (data.hours > 23)
		{
			data.hours %= 24;
		}
	}
}
