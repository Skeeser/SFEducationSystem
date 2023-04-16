#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

struct TimeData
{
	int weekday;
	int hours;
	int minutes;
	int seconds;
	int monthDay;
	int currentMonth;
	int currentYear;
};

class TimeClient
{
public:
	TimeClient();

	void UpdateTime(TimeData &data);

private:
	long t = -1200000;
	long last_sec = 0;

	long interval = 1200000; // 20min

	const long utcOffsetInSeconds = 28800;

	WiFiUDP ntpUDP;
	NTPClient *timeClient;
};