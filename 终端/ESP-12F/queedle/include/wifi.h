#ifndef _WIFI_
#define _WIFI_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "all_config.h"
#include <WString.h>
#include <paint_page.h>

class myWifi
{
public:
    myWifi();
    void WifiInit(const char *ssid, const char *password, const char *host, int tcpport);
    // void WifiHttpSetting();
    void WifiTcpSend(const char *sendbuff);
    bool WifiTcpRead(unsigned char *readbuff);
    String WifiTcpRead();

    WiFiClient client; // tcpclient实例
private:
    // struct ip_info info;
    // ESP8266WebServer server(int port = 80); //这里修改端口http
};

String get_host(String ip);

#endif