
#include "ws2812.h"

#define Code0       30
#define Code1       60
#define CodeReset   0

void ws2812_Updata() {
    static uint16_t data[] = {
        Code1,Code1,Code1,Code1,Code1,Code1,Code1,Code1,
        Code1,Code1,Code1,Code1,Code1,Code1,Code1,Code1,
        Code1,Code1,Code1,Code1,Code1,Code1,Code1,Code1,
        CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,
        CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,
        CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,
        CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,
        CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset,CodeReset
    };
    HAL_TIM_PWM_Start_DMA(&htim3,TIM_CHANNEL_1,(uint32_t*)data,sizeof(data)/sizeof(uint16_t));


}
// 在tim.c的USER CODE区域添加DMA完成回调
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim3) {
        HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);  // 传输完成后停止PWM
    }
}


