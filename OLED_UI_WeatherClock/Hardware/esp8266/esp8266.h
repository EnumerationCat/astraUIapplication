#ifndef __ESP8266_H
#define __ESP8266_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "RTCtimer.h"
#include "usart.h"
#include "tim.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include "cJSON.h"
#include "MPU6050.h"

/* 串口缓冲区配置 */
#define USART3_MAX_RECV_LEN    4096    // 最大接收长度
#define USART3_MAX_SEND_LEN    1024     // 最大发送长度

/* WiFi配置参数 */
#define WIFI_SSID             "Wi-Fi"            // 路由器SSID
#define WIFI_PASSWORD         "chuang123."       // 路由器密码

/* 天气API配置 */
#define WEATHER_SERVERIP      "api.seniverse.com"  // 天气服务器域名
#define WEATHER_PORTNUM       "80"                 // 端口号
#define WEATHER_API_KEY       "SjL5ZSKDkhD7ATz17"  // API密钥钥
#define WEATHER_LOCATION      "qingdao"            // 城市
#define WEATHER_LANGUAGE      "zh-Hans"            // 语言中文//en是英文
#define WEATHER_UNIT          "c"                  // 温度单位

/*时间API配置 */
#define Time_SERVERIP         "www.beijing-time.org"
#define Time_PORTNUM          "80"                   // 端口号



#define USART3_RX_STA_COMPLETE    (1 << 15)  // 接收完成标志位
    /* 接收状态定义 */
    // [15]: 接收完成标记  [14:0]: 接收数据长度
 typedef volatile uint16_t vu16;
/* 全局变量声明 */
extern uint8_t WiFi_Connect_Flag ;
extern uint8_t USART3_RX_BUF[USART3_MAX_RECV_LEN];  // 接收缓冲区
extern uint8_t USART3_TX_BUF[USART3_MAX_SEND_LEN];  // 发送缓冲区
extern vu16 USART3_RX_STA;                          // 接收状态标记


    /* WiFi扫描结果结构体定义 */
    /* WiFi扫描结果结构体定义 */
    typedef struct {
        int ecn;            // 加密方式
        char ssid[33];      // SSID名称（最大32字符+1结束符）
        int rssi;           // 信号强度
        char mac[18];       // MAC地址（17字符+1结束符）
        int channel;        // 信道
        int freq_offset;    // 频率偏移
        int freq_cal;       // 频率校准
    } WiFiScanResult_t;
#define MAX_WIFI_SCAN_RESULTS 10  // 最大扫描结果数量

typedef struct {
        char ip[16];        // IP地址，如"192.168.171.185"
        char gateway[16];   // 网关地址，如"192.168.171.32"
        char netmask[16];   // 子网掩码，如"255.255.255.0"
} WiFiInfo_t;


/* 天气数据结构体定义 */
typedef struct {
    char id[32];            // 位置ID
    char name[32];          // 城市名称
    char country[8];        // 国家
    char path[64];          // 路径
    char timezone[32];      // 时区
    char timezone_offset[16];// 时区偏移
} Location_t;

typedef struct {
    char text[16];          // 天气描述
    char code[8];           // 天气代码
    char temperature[8];    // 温度
} NowWeather_t;

typedef struct {
    Location_t location;    // 位置信息
    NowWeather_t now;       // 实时天气
    char last_update[32];   // 最后更新时间
} Results_t;

    // 网络时间结构体（对应代码中的nwt变量）
typedef struct {
        uint16_t year;   // 年（如2024）
        uint8_t  month;  // 月（1-12）
        uint8_t  date;   // 日（1-31）
        uint8_t  hour;   // 时（0-23）
        uint8_t  min;    // 分（0-59）
        uint8_t  sec;    // 秒（0-59）
} NetWorkTime_t;
extern WiFiScanResult_t wifiScanResults[];
extern uint8_t wifiScanCount;
extern WiFiInfo_t wifiInfo;

extern NetWorkTime_t nwt;

extern  Results_t results[];

extern uint8_t time10minflage;


void USART3_Receive_Init(void);


uint8_t esp8266_send_cmd(uint8_t *cmd, uint8_t *ack, uint16_t waittime);
uint8_t* esp8266_check_cmd(uint8_t *str);


void esp8266_start_trans(const char* wifista_ssid, const char* wifista_password);
void esp8266_close_trans(void);
void esp8266_getWiFi_Information(void);

uint8_t esp8266_scan_wifi(void);  // 扫描WiFi网络
void esp8266_print_scan_results(void);  // 打印扫描结果

uint8_t get_current_weather(void);
uint8_t get_current_Time(void);

uint8_t atk_8266_quit_trans(void);
int cJSON_WeatherParse(char *JSON, Results_t *results);
uint8_t esp8266_start_trans_nonblock(const char* wifista_ssid, const char* wifista_password);
void esp8266_reset_trans_state(void);
uint8_t esp8266_Connect(const char* wifista_ssid, const char* wifista_password);
#ifdef __cplusplus
}
#endif

#endif /* __ESP8266_H */