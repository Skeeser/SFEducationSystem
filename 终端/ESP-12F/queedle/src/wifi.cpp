#include "wifi.h"

using namespace std;

String get_host(String ip)
{
    String ret = "";
    String str = "132"; //"164" ;

    int T = 3;
    for (unsigned int i = 0; i < ip.length(); i++)
    {
        if (ip[i] == '.')
            T--;
        ret += ip[i];

        if (!T)
        {
            ret += str;
            return ret;
        }
    }
    return ret;
}

myWifi::myWifi()
{
}

void myWifi::WifiInit(const char *ssid, const char *password, const char *host, int tcpport)
{

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // 使用静态ip
    //  wifi_station_dhcpc_stop();
    //  struct ip_info info;
    //  //这里更改ipv4地址
    //  IP4_ADDR(&info.ip, 192, 168, 41, 132);
    //  IP4_ADDR(&info.gw, 192, 168, 41, 195);
    //  IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    //  wifi_set_ip_info(STATION_IF, &info);

    while (WiFi.status() != WL_CONNECTED)
    {
        ESP.wdtFeed();
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    String mhost = get_host(WiFi.localIP().toString());
    Serial.println(WiFi.localIP().toString());
    Serial.println(mhost.c_str());

// 连接tcp服务器
// 处理未连接到服务器异常
#if IFTCP
    Serial.println("\nstart connecting to web server....");
    while (!client.connected())
    {
        ESP.wdtFeed();

        if (!client.connect(mhost.c_str(), tcpport))
        // if (!client.connect(WiFi.localIP(), tcpport))
        {
            delay(500);
            Serial.print(".");
        }
    }
    Serial.println("\ntcp connected!");
#endif
}

// void myWifi::WifiHttpSetting() //没弄完
// {
//     //使用静态ip
//     wifi_station_dhcpc_stop();
//     //这里更改ipv4地址
//     IP4_ADDR(&info.ip, 192, 168, 54, 132);
//     IP4_ADDR(&info.gw, 192, 168, 54, 250);
//     IP4_ADDR(&info.netmask, 255, 255, 255, 0);
//     wifi_set_ip_info(STATION_IF, &info);

//     // Connect to WiFi network
//     Serial.println("");
//     Serial.println("");
//     Serial.print("Connected to ");
//     Serial.println(m_ssid);

//     while (WiFi.status() != WL_CONNECTED)
//     {
//         ESP.wdtFeed();
//         delay(500);
//         Serial.print(".");
//     }

//     Serial.println("\nWiFi connected");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());

//     if (MDNS.begin("esp8266"))
//     {
//         Serial.println("MDNS responder started");
//     }

//     // server.on("/", handleRoot);
//     // server.on("/styles.css", sendCSS);
//     // server.on("/processingA.js", sendJS_A);
//     // server.on("/processingB.js", sendJS_B);
//     // server.on("/processingC.js", sendJS_C);
//     // server.on("/processingD.js", sendJS_D);
//     // server.on("/LOAD", EPD_Load);
//     // server.on("/EPD", EPD_Init);
//     // server.on("/NEXT", EPD_Next);
//     // server.on("/SHOW", EPD_Show);
//     // server.onNotFound(handleNotFound);

//     // server.begin();
//     Serial.println("HTTP server started");
// }

// 透传图片数据
bool myWifi::WifiTcpRead(unsigned char *readbuff)
{
    // 透传数据
    if (client.available())
    {
        for (int count = 0; count < ALLSCREEN_GRAGHBYTES; count++)
        {
            ESP.wdtFeed();
            auto read_ch = client.read();
            if (read_ch != -1)
                readbuff[count] = read_ch;

            if (count % 20 == 0) // 数据经过测试100合适 500出问题了
            {
                delay(1); // 必须延时一毫秒接收，要不接受不完全
            }

            // Serial.print(readbuff[count]);
            //   client.read(readbuff, ALLSCREEN_GRAGHBYTES / 10);
        }

        return true;
    }
    return false;
}

// 透传字符串数据
String myWifi::WifiTcpRead()
{
    if (client.available())
    {
        // 接收字符串
        String data = client.readStringUntil('\n');
        Serial.print("read data is =>");
        Serial.print(data);
        Serial.println("");
        return data;
    }
    return "";
}

void myWifi::WifiTcpSend(const char *sendbuff)
{
    client.write(sendbuff, strlen(sendbuff));
}