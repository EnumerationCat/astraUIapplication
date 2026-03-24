#include "RTCtimer.h"



// RTC已经被初始化的值 记录在RTC_BKP_DR1中
#define RTC_INIT_FLAG 0x2333
// 闹钟启用标志存储在备份寄存器RTC_BKP_DR2中
#define RTC_ALARM_ENABLED_FLAG 0xAAAA

// RTC操作超时时间(ms)
#define RTC_TIMEOUT_VALUE 100U
/**
 * @brief  进入RTC初始化模式
 * @param  hrtc  指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  tickstart = HAL_GetTick();
  /* 等待RTC处于INIT状态，如果到达Time out 则退出 */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* 禁用RTC寄存器的写保护 */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);


  return HAL_OK;
}

/**
 * @brief  退出RTC初始化模式
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef *hrtc)
{
  uint32_t tickstart = 0U;

  /* 禁用RTC寄存器的写保护。 */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* 等到RTC处于INIT状态，如果到达Time out 则退出 */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

/**
 * @brief  写入RTC_CNT寄存器中的时间计数器。
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针。
 * @param  TimeCounter: 写入RTC_CNT寄存器的计数器
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_WriteTimeCounter(RTC_HandleTypeDef *hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* 进入RTC初始化模式 */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
	/* 设置RTC计数器高位寄存器 */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16U));
    /* 设置RTC计数器低位寄存器 */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* 退出RTC初始化模式 */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}


/**
 * @brief  读取RTC_CNT寄存器中的时间计数器。
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针。
 * @retval 时间计数器
 */
static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef *hrtc)
{
  uint16_t high1 = 0U, high2 = 0U, low = 0U;
  uint32_t timecounter = 0U;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
	/* 当读取CNTL和CNTH寄存器期间计数器溢出时, 重新读取CNTL寄存器然后返回计数器值 */
    timecounter = (((uint32_t) high2 << 16U) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
	/* 当读取CNTL和CNTH寄存器期间没有计数器溢出, 计数器值等于第一次读取的CNTL和CNTH值 */
    timecounter = (((uint32_t) high1 << 16U) | low);
  }

  return timecounter;
}

/**
 * @brief 设置RTC时间
 * @param time 时间
 * @retval HAL status
 */
HAL_StatusTypeDef My_RTC_SetTime(struct tm *time){
	uint32_t unixTime = mktime(time);
	return RTC_WriteTimeCounter(&hrtc, unixTime);
}

/**
 * @brief 获取RTC时间
 * @retval 时间
 */
struct tm *My_RTC_GetTime() {
  time_t unixTime = RTC_ReadTimeCounter(&hrtc);
  return gmtime(&unixTime);
}

/**
 * @brief  初始化RTC闹钟中断（NVIC配置）
 * @note   必须在RTC初始化后调用，确保中断能被CPU响应
 * @retval None
 */
static void RTC_AlarmIRQ_Init(void)
{
  HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);  // 设置中断优先级（可根据需求调整）
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);         // 使能RTC闹钟中断的NVIC通道
}

void My_RTC_Init(){
	uint32_t initFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
	if(initFlag == RTC_INIT_FLAG) return;
	if (HAL_RTC_Init(&hrtc) != HAL_OK){
		Error_Handler();
	}

  // 初始化RTC闹钟中断（关键：添加这行）
  RTC_AlarmIRQ_Init();

	struct tm time = {
		  .tm_year = 2025 - 1900,
		  .tm_mon = 1 - 1,
		  .tm_mday = 1,
		  .tm_hour = 23,
		  .tm_min = 59,
		  .tm_sec = 55,
	};
	My_RTC_SetTime(&time);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, RTC_INIT_FLAG);
    // 默认禁用闹钟
    My_RTC_DisableAlarm();

}




/**
 * @brief  写入RTC_ALR寄存器中的闹钟计数器。
 * @param  hrtc   指向包含RTC配置信息的RTC_HandleTypeDef结构体的指针。
 * @param  AlarmCounter: 写入RTC_ALR寄存器的计数器
 * @retval HAL status
 */
static HAL_StatusTypeDef RTC_WriteAlarmCounter(RTC_HandleTypeDef *hrtc, uint32_t AlarmCounter)
{
    HAL_StatusTypeDef status = HAL_OK;

    /* 进入RTC初始化模式 */
    if (RTC_EnterInitMode(hrtc) != HAL_OK)
    {
        status = HAL_ERROR;
    }
    else
    {
        /* 设置闹钟计数器高位寄存器 */
        WRITE_REG(hrtc->Instance->ALRH, (AlarmCounter >> 16U));
        /* 设置闹钟计数器低位寄存器 */
        WRITE_REG(hrtc->Instance->ALRL, (AlarmCounter & RTC_CNTL_RTC_CNT));

        /* 退出RTC初始化模式 */
        if (RTC_ExitInitMode(hrtc) != HAL_OK)
        {
            status = HAL_ERROR;
        }
    }

    return status;
}

/**
 * @brief 设置RTC闹钟（带掩码）
 * @param alarmTime 闹钟时间
 * @param mask 闹钟掩码（忽略指定时间字段）
 * @retval HAL status
 */
HAL_StatusTypeDef My_RTC_SetAlarm(struct tm *alarmTime, uint8_t mask)
{
  if (alarmTime == NULL) return HAL_ERROR;

  // 1. 转换闹钟时间为Unix时间戳
  time_t alarmUnixTime = mktime(alarmTime);
  if (alarmUnixTime == (time_t)-1) return HAL_ERROR;

  // 2. 根据掩码调整闹钟时间（忽略指定字段）
  struct tm adjustedTime = *localtime(&alarmUnixTime);
  if (mask & ALARM_MASK_SECONDS) adjustedTime.tm_sec = 0;
  if (mask & ALARM_MASK_MINUTES) adjustedTime.tm_min = 0;
  if (mask & ALARM_MASK_HOURS)   adjustedTime.tm_hour = 0;
  if (mask & ALARM_MASK_DATE)    adjustedTime.tm_mday = 1;
  if (mask & ALARM_MASK_WEEKDAY) adjustedTime.tm_wday = 0;

  // 重新计算调整后的Unix时间戳
  alarmUnixTime = mktime(&adjustedTime);
  if (alarmUnixTime == (time_t)-1) return HAL_ERROR;

  // 3. 写入闹钟计数器
  HAL_StatusTypeDef status = RTC_WriteAlarmCounter(&hrtc, (uint32_t)alarmUnixTime);
  if (status != HAL_OK) return status;

  // 4. 启用闹钟中断
  if (RTC_EnterInitMode(&hrtc) == HAL_OK)
  {
    // 清除闹钟标志位
    SET_BIT(hrtc.Instance->CRL, RTC_CRL_ALRF);
    // 启用闹钟中断
    SET_BIT(hrtc.Instance->CRH, RTC_CRH_ALRIE);
    RTC_ExitInitMode(&hrtc);

    // 5. 标记闹钟启用
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, RTC_ALARM_ENABLED_FLAG);


    return HAL_OK;
  }

  return HAL_ERROR;
}

/**
 * @brief 禁用RTC闹钟
 * @retval HAL status
 */
HAL_StatusTypeDef My_RTC_DisableAlarm(void)
{
  // 1. 禁用闹钟中断
  if (RTC_EnterInitMode(&hrtc) == HAL_OK)
  {
    // 禁用闹钟中断
    CLEAR_BIT(hrtc.Instance->CRH, RTC_CRH_ALRIE);
    // 清除闹钟标志位
    SET_BIT(hrtc.Instance->CRL, RTC_CRL_ALRF);
    RTC_ExitInitMode(&hrtc);

    // 2. 清除闹钟启用标志
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, 0x0000);
    return HAL_OK;
  }

  return HAL_ERROR;
}

/**
 * @brief 检查闹钟是否启用
 * @retval 1-启用 0-禁用
 */
uint8_t My_RTC_IsAlarmEnabled(void)
{
  uint32_t alarmFlag = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
  return (alarmFlag == RTC_ALARM_ENABLED_FLAG) ? 1 : 0;
}

/**
 * @brief 设置每日重复闹钟
 * @param alarmTime 闹钟时间（仅小时/分钟/秒生效）
 * @retval HAL status
 */
HAL_StatusTypeDef My_RTC_SetDailyAlarm(struct tm *alarmTime)
{
  if (alarmTime == NULL) return HAL_ERROR;

  // 基于当前时间，替换小时/分钟/秒，保持日期不变（实现每日重复）
  time_t now = RTC_ReadTimeCounter(&hrtc);
  struct tm currentTime = *localtime(&now);
  currentTime.tm_hour = alarmTime->tm_hour;
  currentTime.tm_min = alarmTime->tm_min;
  currentTime.tm_sec = alarmTime->tm_sec;

  // 设置闹钟（忽略日期和星期）
  return My_RTC_SetAlarm(&currentTime, ALARM_MASK_DATE | ALARM_MASK_WEEKDAY);
}

/**
 * @brief 设置一次性闹钟（精确匹配）
 * @param alarmTime 闹钟时间
 * @retval HAL status
 */
HAL_StatusTypeDef My_RTC_SetOneTimeAlarm(struct tm *alarmTime)
{
  // 无掩码，精确匹配所有时间字段
  return My_RTC_SetAlarm(alarmTime, ALARM_MASK_NONE);
}


uint8_t Alarm_Flag=0;
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) {
  Alarm_Flag=1;
}
/*
// 1. 设置每日8:00闹钟
struct tm dailyAlarm = {0};
dailyAlarm.tm_hour = 8;
dailyAlarm.tm_min = 0;
dailyAlarm.tm_sec = 0;
My_RTC_SetDailyAlarm(8, 0, 0);

// 2. 设置一次性闹钟（2025-01-02 12:30:00）
struct tm oneTimeAlarm = {
  .tm_year = 2025 - 1900,
  .tm_mon = 1 - 1,
  .tm_mday = 2,
  .tm_hour = 12,
  .tm_min = 30,
  .tm_sec = 0
};
My_RTC_SetOneTimeAlarm(&oneTimeAlarm);

// 3. 检查闹钟状态
if (My_RTC_IsAlarmEnabled())
{
  // 闹钟已启用
}

// 4. 禁用闹钟
My_RTC_DisableAlarm();*/