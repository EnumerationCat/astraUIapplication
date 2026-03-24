#ifndef __RTCtimer_H__
#define __RTCtimer_H__

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
    // 声明全局hrtc句柄（确保和你的工程中定义一致）
    extern RTC_HandleTypeDef hrtc;
    HAL_StatusTypeDef My_RTC_SetTime(struct tm *time);
    struct tm *My_RTC_GetTime();
    void My_RTC_Init();
    // 以下是新增的闹钟功能声明
    // 闹钟掩码定义
    #define ALARM_MASK_NONE          0x00    // 无掩码，精确匹配
    #define ALARM_MASK_SECONDS       0x01    // 忽略秒
    #define ALARM_MASK_MINUTES       0x02    // 忽略分
    #define ALARM_MASK_HOURS         0x04    // 忽略时
    #define ALARM_MASK_DATE          0x08    // 忽略日期
    #define ALARM_MASK_WEEKDAY       0x10    // 忽略星期
    extern uint8_t Alarm_Flag;
    // 闹钟函数声明
    HAL_StatusTypeDef My_RTC_SetAlarm(struct tm *alarmTime, uint8_t mask);
    HAL_StatusTypeDef My_RTC_DisableAlarm(void);
    uint8_t My_RTC_IsAlarmEnabled(void);
    HAL_StatusTypeDef My_RTC_SetDailyAlarm(struct tm *alarmTime);
    HAL_StatusTypeDef My_RTC_SetOneTimeAlarm(struct tm *alarmTime);
#ifdef __cplusplus
}
#endif

#endif