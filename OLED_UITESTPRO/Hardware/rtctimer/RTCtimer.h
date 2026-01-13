#ifndef __RTCtimer_H__
#define __RTCtimer_H__

#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

    HAL_StatusTypeDef My_RTC_SetTime(struct tm *time);
    struct tm *My_RTC_GetTime();
    void My_RTC_Init();

#ifdef __cplusplus
}
#endif

#endif