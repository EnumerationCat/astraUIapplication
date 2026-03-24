#include "esp8266.h"

uint8_t WiFi_Connect_Flag = 0;
 uint8_t USART3_RX_BUF[USART3_MAX_RECV_LEN];  // 接收缓冲区
 uint8_t USART3_TX_BUF[USART3_MAX_SEND_LEN];  // 发送缓冲区
 vu16 USART3_RX_STA;                          // 接收状态标记
/**
 * @brief  重定向标准输出到串口1（适用于GCC）
 */
int _write(int file, char *ptr, int len)
{
    if (file == STDOUT_FILENO || file == STDERR_FILENO)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
        return len;
    }

    errno = EBADF;
    return -1;
}

/**
 * @brief  重定向标准输入（可选）
 */
int _read(int file, char *ptr, int len)
{
    if (file == STDIN_FILENO)
    {
        // 这里可以实现从串口接收数据
        // 暂时返回0，表示没有数据
        return 0;
    }

    errno = EBADF;
    return -1;
}
/**
 * @brief  定时器更新中断回调函数
 * @param  htim: 定时器句柄指针
 * @retval None
 */
int time10min=0;
uint8_t time10minflage=0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {  // 判断是否是TIM2触发的中断//定时10ms


        __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);

        // 如果已经接收到数据但还没标记完成，则标记接收完成
        if(USART3_RX_STA > 0 && (USART3_RX_STA & (1 << 15)) == 0)
        {
            USART3_RX_STA |= (1 << 15);  // 标记接收完成
        }

        // 停止定时器
        HAL_TIM_Base_Stop_IT(&htim2);
    }
	//定时1s
	if (htim->Instance == TIM7) {
		time10min++;
		if (time10min>=600) {
			time10min=0;
			time10minflage=1;
		}
	}

}
// 接收数据的临时变量
static uint8_t RxByte;

//这个放MyHal开机自动执行，串口错误中断里重新加一下
void USART3_Receive_Init(void)
{
    // 启动串口接收中断
    HAL_UART_Receive_IT(&huart3, &RxByte, 1);
    HAL_TIM_Base_Stop_IT(&htim2);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

    if (huart->Instance == USART3)
    {
        // 如果之前接收的一批数据还没有被处理，则不再接收新数据
        if((USART3_RX_STA & (1 << 15)) == 0)
        {
            // 检查缓冲区是否还有空间
            if(USART3_RX_STA < USART3_MAX_RECV_LEN)
            {
                // 如果是第一个字符，启动定时器用于超时判断
                if(USART3_RX_STA == 0)
                {
                    // 启动定时器
                    __HAL_TIM_SET_COUNTER(&htim2, 0);  // 计数器清零
                    HAL_TIM_Base_Start_IT(&htim2);     // 启动定时器中断
                }

                // 存储接收到的数据
                USART3_RX_BUF[USART3_RX_STA++] = RxByte;
            }
            else
            {
                // 缓冲区已满，强制标记接收完成
                USART3_RX_STA |= (1 << 15);
            }
        }

        // 重新启动接收中断，等待下一个字节
        HAL_UART_Receive_IT(&huart3, &RxByte, 1);
    }
}


//const uint8_t* wifista_ssid="Wi-Fi";			//路由器SSID号
//const uint8_t* wifista_password="chuang123."; 	//连接密码




Results_t results[] = {{0}};

NetWorkTime_t nwt = {0};
void esp8266_start_trans(const char* wifista_ssid, const char* wifista_password)
{
	//USART3_Receive_Init();
	char Ussercode[50];
    						//申请32字节内存，用于存wifista_ssid，wifista_password
    printf("send:AT\r\n");
    while(esp8266_send_cmd("AT","OK",20))//检查WIFI模块是否在线
    {
    }
	esp8266_scan_wifi();
    //设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
    printf("send:AT+CWMODE=1\r\n");
    esp8266_send_cmd("AT+CWMODE=1","OK",50);
    //Wifi模块重启
    printf("send:AT+RST\r\n");
    esp8266_send_cmd("AT+RST","OK",50);
	HAL_Delay(1000);
	HAL_Delay(1000);
	HAL_Delay(1000);
	// uint32_t now_ms = HAL_GetTick();
	// while ( HAL_GetTick()-now_ms<=3000);

    //设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!!
    printf("send:AT+CIPMUX=0\r\n");
    esp8266_send_cmd("AT+CIPMUX=0","OK",50);   //0：单连接，1：多连接

    sprintf(Ussercode,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
    printf("send:AT+CWJAP=\"%s\",\"%s\"\r\n",wifista_ssid,wifista_password);

    while(esp8266_send_cmd(Ussercode,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
	HAL_Delay(1000);
	esp8266_getWiFi_Information();

}
void esp8266_close_trans(void) {
	printf("send:AT+CWQAP\r\n");
	esp8266_send_cmd("AT+CWQAP","OK",50);

}
// 在esp8266.c中添加全局变量
WiFiScanResult_t wifiScanResults[MAX_WIFI_SCAN_RESULTS] = {{0}};
uint8_t wifiScanCount = 0;
// WiFi扫描函数
// WiFi扫描函数
uint8_t esp8266_scan_wifi(void) {
     wifiScanCount = 0;  // 重置计数器

    // 清空接收缓冲区
    USART3_RX_STA = 0;

    // 发送扫描命令
    printf("send:AT+CWLAP\r\n");
    esp8266_send_cmd((uint8_t*)"AT+CWLAP", (uint8_t*)"OK", 5000);

    if(USART3_RX_STA & 0X8000) {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = '\0';
        char *buffer = (char*)USART3_RX_BUF;
        char *line_start = buffer;

        printf("=== WiFi扫描开始 ===\r\n");

        while((line_start = strstr(line_start, "+CWLAP:")) != NULL && wifiScanCount < MAX_WIFI_SCAN_RESULTS) {
            WiFiScanResult_t *result = &wifiScanResults[wifiScanCount];

            // 初始化结构体
            memset(result, 0, sizeof(WiFiScanResult_t));

            // 移动到参数开始位置
            char *ptr = line_start + strlen("+CWLAP:");

            if(*ptr != '(') {
                line_start++;
                continue;
            }

            ptr++;  // 跳过 '('

            // 1. 解析ecn（第一个数字）
            char *comma1 = strchr(ptr, ',');
            if(!comma1) break;
            *comma1 = '\0';  // 临时替换为字符串结束符
            result->ecn = atoi(ptr);
            *comma1 = ',';  // 恢复逗号
            ptr = comma1 + 1;

            // 2. 解析ssid（在双引号中）
            if(*ptr != '\"') break;
            ptr++;  // 跳过双引号
            char *quote1 = strchr(ptr, '\"');
            if(!quote1) break;
            *quote1 = '\0';
            strncpy(result->ssid, ptr, sizeof(result->ssid) - 1);
            *quote1 = '\"';
            ptr = quote1 + 1;

            // 跳过逗号
            if(*ptr != ',') break;
            ptr++;

            // 3. 解析rssi
            char *comma2 = strchr(ptr, ',');
            if(!comma2) break;
            *comma2 = '\0';
            result->rssi = atoi(ptr);
            *comma2 = ',';
            ptr = comma2 + 1;

            // 4. 解析mac（在双引号中）
            if(*ptr != '\"') break;
            ptr++;  // 跳过双引号
            char *quote2 = strchr(ptr, '\"');
            if(!quote2) break;
            *quote2 = '\0';
            strncpy(result->mac, ptr, sizeof(result->mac) - 1);
            *quote2 = '\"';
            ptr = quote2 + 1;

            // 跳过逗号
            if(*ptr != ',') break;
            ptr++;

            // 5. 解析channel
            char *comma3 = strchr(ptr, ',');
            if(!comma3) break;
            *comma3 = '\0';
            result->channel = atoi(ptr);
            *comma3 = ',';
            ptr = comma3 + 1;

            // 6. 解析freq_offset
            char *comma4 = strchr(ptr, ',');
            if(!comma4) break;
            *comma4 = '\0';
            result->freq_offset = atoi(ptr);
            *comma4 = ',';
            ptr = comma4 + 1;

            // 7. 解析freq_cal（最后一个数字，以')'结束）
            char *paren_end = strchr(ptr, ')');
            if(!paren_end) break;
            *paren_end = '\0';
            result->freq_cal = atoi(ptr);
            *paren_end = ')';

            // 打印结果
            printf("网络 %d:\r\n", wifiScanCount + 1);
            printf("  SSID: %s\r\n", result->ssid);
        	printf("  加密方式: ");
        	switch(result->ecn) {
        		case 0: printf("OPEN\r\n"); break;
        		case 1: printf("WEP\r\n"); break;
        		case 2: printf("WPA_PSK\r\n"); break;
        		case 3: printf("WPA2_PSK\r\n"); break;
        		case 4: printf("WPA_WPA2_PSK\r\n"); break;
        		default: printf("未知(%d)\r\n", result->ecn); break;
        	}
            printf("  信号强度: %d dBm\r\n", result->rssi);
            printf("  MAC地址: %s\r\n", result->mac);
            printf("  信道: %d\r\n", result->channel);
            printf("  频率偏移: %d kHz\r\n", result->freq_offset);
            printf("  频率校准: %d\r\n", result->freq_cal);
            printf("--------------------------------\r\n");

            wifiScanCount++;

            // 查找下一行
            line_start = strchr(line_start, '\n');
            if(line_start) line_start++;
            else break;
        }

        printf("总共找到 %d 个WiFi网络\r\n", wifiScanCount);
        USART3_RX_STA = 0;
        return wifiScanCount;
    }

    USART3_RX_STA = 0;
    return 0;
}






// 全局变量声明
WiFiInfo_t wifiInfo = {0};

// 修改函数，解析WiFi信息
void esp8266_getWiFi_Information(void) {
	USART3_RX_STA = 0;


    esp8266_send_cmd((uint8_t*)"AT+CIPSTA?", (uint8_t*)"OK", 50);

    if(USART3_RX_STA & 0X8000) {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = '\0';

        char *buffer = (char*)USART3_RX_BUF;

        // 1. 查找IP地址
        char *ip_start = strstr(buffer, "+CIPSTA:ip:\"");
        if(ip_start) {
            ip_start += strlen("+CIPSTA:ip:\"");  // 移动到IP起始位置

            char *ip_end = strchr(ip_start, '\"');  // 找到结束的双引号
            if(ip_end) {
                int ip_len = ip_end - ip_start;
                if(ip_len < sizeof(wifiInfo.ip)) {
                    strncpy(wifiInfo.ip, ip_start, ip_len);
                    wifiInfo.ip[ip_len] = '\0';
                }
            }
        }

        // 2. 查找网关地址
        char *gateway_start = strstr(buffer, "+CIPSTA:gateway:\"");
        if(gateway_start) {
            gateway_start += strlen("+CIPSTA:gateway:\"");

            char *gateway_end = strchr(gateway_start, '\"');
            if(gateway_end) {
                int gateway_len = gateway_end - gateway_start;
                if(gateway_len < sizeof(wifiInfo.gateway)) {
                    strncpy(wifiInfo.gateway, gateway_start, gateway_len);
                    wifiInfo.gateway[gateway_len] = '\0';
                }
            }
        }

        // 3. 查找子网掩码
        char *netmask_start = strstr(buffer, "+CIPSTA:netmask:\"");
        if(netmask_start) {
            netmask_start += strlen("+CIPSTA:netmask:\"");

            char *netmask_end = strchr(netmask_start, '\"');
            if(netmask_end) {
                int netmask_len = netmask_end - netmask_start;
                if(netmask_len < sizeof(wifiInfo.netmask)) {
                    strncpy(wifiInfo.netmask, netmask_start, netmask_len);
                    wifiInfo.netmask[netmask_len] = '\0';
                }
            }
        }


        printf("IP: %s\r\n", wifiInfo.ip);
        printf("Gateway: %s\r\n", wifiInfo.gateway);
        printf("Netmask: %s\r\n", wifiInfo.netmask);

        USART3_RX_STA = 0;
    }
}


void AT_Send_cmd(const char *cmd) {

    HAL_UART_Transmit(&huart3, (uint8_t*)cmd, strlen(cmd), 100);
}
//向ESP8266发送命令
//cmd:发送的命令字符串;ack:期待的应答结果,如果为空,则表示不需要等待应答;waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果);1,发送失败
uint8_t esp8266_send_cmd(uint8_t *cmd,uint8_t*ack,uint16_t waittime)
{
	char full_cmd[128];
	// 构建完整命令（添加回车换行）
	snprintf(full_cmd, sizeof(full_cmd), "%s\r\n", cmd);
    uint8_t res=0;
    USART3_RX_STA=0;
    AT_Send_cmd(full_cmd);
    if(ack&&waittime)		//需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            HAL_Delay(10);
            if(USART3_RX_STA&0X8000)//接收到期待的应答结果
            {
                if(esp8266_check_cmd(ack))
                {
                    printf("receive:%s\r\n",(uint8_t*)ack);
                    break;//得到有效数据
                }
                USART3_RX_STA=0;
            }
        }
        if(waittime==0)res=1;
    }
    return res;
}
//ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果;其他,期待应答结果的位置(str的位置)
uint8_t* esp8266_check_cmd(uint8_t *str)
{
    char *strx=0;
    if(USART3_RX_STA&0X8000)		//接收到一次数据了
    {
        USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
        strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
    }
    return (uint8_t*)strx;
}


//获取一次实时天气
//返回：0---获取成功，1---获取失败
uint8_t get_current_weather(void)
{
	char cmd[128];


	// 构建TCP连接命令
	snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%s",WEATHER_SERVERIP,WEATHER_PORTNUM);
	printf("%s\r\n",cmd);
	esp8266_send_cmd(cmd,"OK",200);//连接到目标TCP服务器
	HAL_Delay(300);

	printf("send:AT+CIPMODE=1\r\n");
	esp8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传

	USART3_RX_STA=0;
	printf("send:AT+CIPSEND\r\n");
    //这里有bug，需要再次连接TCP服务器，才连接上
	esp8266_send_cmd(cmd,"OK",200);//连接到目标TCP服务器
	esp8266_send_cmd("AT+CIPSEND",(uint8_t*)">",100);         //开始透传
	HAL_Delay(500); // 增加500ms延时，等待模式切换完成

	printf("start trans...\r\n");

	AT_Send_cmd("GET https://api.seniverse.com/v3/weather/now.json?key=SjL5ZSKDkhD7ATz17&location=qingdao&language=en&unit=c\n\n");
	HAL_Delay(20);//延时20ms返回的是指令发送成功的状态
	USART3_RX_STA=0;	//清零串口3数据
	HAL_Delay(1000);
	if(USART3_RX_STA&0X8000)		//此时再次接到一次数据，为天气的数据
	{
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
	}
	printf("USART3_RX_BUF=%s\r\n",USART3_RX_BUF);


	cJSON_WeatherParse(USART3_RX_BUF, results);		//解析天气数据
	//打印结构体内内容
	printf("获取内容如下： \r\n");

	printf("%s \r\n",results[0].location.country);
	printf("%s \r\n",results[0].location.name);
	printf("%s \r\n",results[0].location.path);

	printf("%s \r\n",results[0].now.text);
	printf("%s \r\n",results[0].now.temperature);
	printf("%s \r\n",results[0].last_update);

	atk_8266_quit_trans();//退出透传

	printf("send:AT+CIPCLOSE\r\n");
	esp8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接

	return 0;
}



//获取一次实时时间
//返回：0---获取成功，1---获取失败
//获取一次实时时间
//返回：0---获取成功，1---获取失败
uint8_t get_current_Time(void)
{
    // 替换指针：用数组替代malloc的内存，避免动态分配
    char cmd[128];          // 存储AT指令
    int gmt_pos = -1;             // GMT字符串在缓冲区的位置（替代p_end指针）
    int time_base_pos = -1;       // 时间解析基准位置（替代p指针）

    // 1. 构建TCP连接命令（无指针，直接用数组）
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%s", Time_SERVERIP, Time_PORTNUM);
    printf("%s\r\n", cmd);

    // 连接TCP服务器，失败直接返回
    esp8266_send_cmd((uint8_t*)cmd, (uint8_t*)"OK", 200);

    HAL_Delay(300);

    // 2. 设置透传模式
    printf("send:AT+CIPMODE=1\r\n");
    esp8266_send_cmd((uint8_t*)"AT+CIPMODE=1", (uint8_t*)"OK", 100);

	esp8266_send_cmd((uint8_t*)cmd, (uint8_t*)"OK", 200);
    USART3_RX_STA = 0;
    printf("send:AT+CIPSEND\r\n");
    esp8266_send_cmd((uint8_t*)"AT+CIPSEND", (uint8_t*)">", 100);

    HAL_Delay(500); // 等待模式切换完成

    // 4. 发送HTTP请求获取时间
    printf("start trans...\r\n");

	AT_Send_cmd("GET /time15.asp HTTP/1.1Host:www.beijing-time.org\n\n");

    // 5. 等待并接收数据
    HAL_Delay(20);  // 指令发送成功等待
    USART3_RX_STA = 0; // 清零接收状态
    HAL_Delay(1000);   // 等待服务器返回数据

    // 给接收缓冲区加结束符（避免乱码）
    if(USART3_RX_STA & 0X8000)
    {
        USART3_RX_BUF[USART3_RX_STA & 0X7FFF] = 0;
    }
    printf("USART3_RX_BUF=%s\r\n", USART3_RX_BUF);

    // 6. 解析时间（核心：用下标替代指针）
    if(USART3_RX_STA & 0x8000)
    {
        // 查找"Date"关键字（替代resp指针）
        char* date_ptr = strstr((char*)USART3_RX_BUF, "Date");
        if(date_ptr != NULL)
        {
            // 查找"GMT"关键字，获取位置（替代p_end指针）
            char* gmt_ptr = strstr((char*)USART3_RX_BUF, "GMT");
            if(gmt_ptr != NULL)
            {
                // 转换为下标（替代p指针：p = p_end -9）
                gmt_pos = gmt_ptr - (char*)USART3_RX_BUF;
                time_base_pos = gmt_pos - 9;

                // 安全检查：避免数组越界
                if(time_base_pos >= 0 && (time_base_pos + 8) < USART3_MAX_RECV_LEN &&
                   (time_base_pos - 12) >= 0 && (time_base_pos - 5) >= 0)
                {
                    // 解析小时（GMT+8）
                    nwt.hour = ((USART3_RX_BUF[time_base_pos] - '0') * 10 + (USART3_RX_BUF[time_base_pos + 1] - '0') + 8) % 24;
                    // 解析分钟
                    nwt.min = ((USART3_RX_BUF[time_base_pos + 3] - '0') * 10 + (USART3_RX_BUF[time_base_pos + 4] - '0')) % 60;
                    // 解析秒
                    nwt.sec = ((USART3_RX_BUF[time_base_pos + 6] - '0') * 10 + (USART3_RX_BUF[time_base_pos + 7] - '0')) % 60;
                    // 解析年
                    nwt.year = (USART3_RX_BUF[time_base_pos - 5] - '0') * 1000 +
                               (USART3_RX_BUF[time_base_pos - 4] - '0') * 100 +
                               (USART3_RX_BUF[time_base_pos - 3] - '0') * 10 +
                               (USART3_RX_BUF[time_base_pos - 2] - '0');
                    // 解析日
                    nwt.date = (USART3_RX_BUF[time_base_pos - 12] - '0') * 10 +
                               (USART3_RX_BUF[time_base_pos - 11] - '0');

                    // 解析月份（无指针，直接用strstr）
                    if(strstr((char*)USART3_RX_BUF, "Jan")) nwt.month = 1;
                    else if(strstr((char*)USART3_RX_BUF, "Feb")) nwt.month = 2;
                    else if(strstr((char*)USART3_RX_BUF, "Mar")) nwt.month = 3;
                    else if(strstr((char*)USART3_RX_BUF, "Apr")) nwt.month = 4;
                    else if(strstr((char*)USART3_RX_BUF, "May")) nwt.month = 5;
                    else if(strstr((char*)USART3_RX_BUF, "Jun")) nwt.month = 6;
                    else if(strstr((char*)USART3_RX_BUF, "Jul")) nwt.month = 7;
                    else if(strstr((char*)USART3_RX_BUF, "Aug")) nwt.month = 8;
                    else if(strstr((char*)USART3_RX_BUF, "Sep")) nwt.month = 9;
                    else if(strstr((char*)USART3_RX_BUF, "Oct")) nwt.month = 10;
                    else if(strstr((char*)USART3_RX_BUF, "Nov")) nwt.month = 11;
                    else if(strstr((char*)USART3_RX_BUF, "Dec")) nwt.month = 12;

                	printf("nwt.year = %d\r\n",nwt.year);
                	printf("nwt.month = %d\r\n",nwt.month);
                	printf("nwt.date = %d\r\n",nwt.date);

                	printf("nwt.hour = %d\r\n",nwt.hour);
                	printf("nwt.min = %d\r\n",nwt.min);
                	printf("nwt.sec = %d\r\n",nwt.sec);

                }

            }

        }

        USART3_RX_STA = 0; // 清零接收状态
    }

    //退出透传+关闭连接
    atk_8266_quit_trans();
    printf("send:AT+CIPCLOSE\r\n");
    esp8266_send_cmd((uint8_t*)"AT+CIPCLOSE", (uint8_t*)"OK", 50);
    return 0;
}



//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
uint8_t atk_8266_quit_trans(void)
{
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';
	HAL_Delay(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';
	HAL_Delay(15);					//大于串口组帧时间(10ms)
	while((USART3->SR&0X40)==0);	//等待发送空
	USART3->DR='+';
	HAL_Delay(500);					//等待500ms
	return esp8266_send_cmd("AT","OK",20);//退出透传判断.
}


/*********************************************************************************
* Function Name    ： cJSON_WeatherParse,解析天气数据
* Parameter		   ： JSON：天气数据包  results：保存解析后得到的有用的数据
* Return Value     ： 0：成功 其他:错误
* Function Explain ：
* Create Date      ： 2017.12.6 by lzn
**********************************************************************************/
int cJSON_WeatherParse(char *JSON, Results_t *results)
{
	cJSON *json,*arrayItem,*object,*subobject,*item;

	json = cJSON_Parse(JSON); //解析JSON数据包
	if(json == NULL)		  //检测JSON数据包是否存在语法上的错误，返回NULL表示数据包无效
	{
		printf("Error before: [%s] \r\n",cJSON_GetErrorPtr()); //打印数据包语法错误的位置
		return 1;
	}
	else
	{
		if((arrayItem = cJSON_GetObjectItem(json,"results")) != NULL); //匹配字符串"results",获取数组内容
		{
			int size = cJSON_GetArraySize(arrayItem);     //获取数组中对象个数
			printf("cJSON_GetArraySize: size=%d \r\n",size);

			if((object = cJSON_GetArrayItem(arrayItem,0)) != NULL)//获取父对象内容
			{
				/* 匹配子对象1 */
				if((subobject = cJSON_GetObjectItem(object,"location")) != NULL)
				{
					printf("---------------------------------subobject1-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"id")) != NULL)   //匹配子对象1成员"id"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.id,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"name")) != NULL) //匹配子对象1成员"name"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.name,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"country")) != NULL)//匹配子对象1成员"country"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.country,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"path")) != NULL)  //匹配子对象1成员"path"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.path,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone")) != NULL)//匹配子对象1成员"timezone"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"timezone_offset")) != NULL)//匹配子对象1成员"timezone_offset"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].location.timezone_offset,item->valuestring,strlen(item->valuestring));
					}
				}
				/* 匹配子对象2 */
				if((subobject = cJSON_GetObjectItem(object,"now")) != NULL)
				{
					printf("---------------------------------subobject2-------------------------------\r\n");
					if((item = cJSON_GetObjectItem(subobject,"text")) != NULL)//匹配子对象2成员"text"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.text,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"code")) != NULL)//匹配子对象2成员"code"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.code,item->valuestring,strlen(item->valuestring));
					}
					if((item = cJSON_GetObjectItem(subobject,"temperature")) != NULL) //匹配子对象2成员"temperature"
					{
						printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",item->type,item->string,item->valuestring);
						memcpy(results[0].now.temperature,item->valuestring,strlen(item->valuestring));
					}
				}
				/* 匹配子对象3 */
				if((subobject = cJSON_GetObjectItem(object,"last_update")) != NULL)
				{
					printf("---------------------------------subobject3-------------------------------\r\n");
					printf("cJSON_GetObjectItem: type=%d, string is %s,valuestring=%s \r\n",subobject->type,subobject->string,subobject->valuestring);
					memcpy(results[0].last_update, subobject->valuestring,strlen(subobject->valuestring));
				}
			}
		}
	}

	cJSON_Delete(json); //释放cJSON_Parse()分配出来的内存空间

	return 0;
}


