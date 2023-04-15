#include "time_utils.h"

TimeClient::TimeClient()
{
	timeClient = new NTPClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
	timeClient->begin();
}

void TimeClient::UpdateTime(TimeData &data)
{
	Serial.println("\nTime is updating......");
	if (millis() - t > interval)
	{
		timeClient->update();
		unsigned long epochTime = timeClient->getEpochTime();
		data.weekday = timeClient->getDay();
		data.hours = timeClient->getHours();
		data.minutes = timeClient->getMinutes();
		data.seconds = timeClient->getSeconds();

		struct tm *ptm = gmtime((time_t *)&epochTime);
		data.monthDay = ptm->tm_mday;
		data.currentMonth = ptm->tm_mon + 1;
		data.currentYear = ptm->tm_year + 1;

		t = millis();
		last_sec = millis();

		Serial.println(data.currentYear);
		Serial.println(data.monthDay);
		Serial.println(data.hours);
		Serial.println(data.minutes);
	}
	else
	{
		data.seconds += ((millis() - last_sec) / 1000);
		last_sec = millis();

		if (data.seconds > 59)
		{
			data.minutes++;
			data.seconds = 0;
		}

		if (data.minutes > 59)
		{
			data.hours++;
			data.minutes = 0;
		}

		if (data.hours > 23)
		{
			data.hours = 0;
		}
	}
}
