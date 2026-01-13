#include "Myhal.h"

#include "astra_rocket.h"


void Myhal::_stm32_hal_init() {
    HAL_Init();
}
void Myhal::_sys_clock_init() {
    SystemClock_Config();
}

void Myhal::_gpio_init(){
    MX_GPIO_Init();

}
void Myhal::_dma_init() {
    MX_DMA_Init();
}
void Myhal::_timer_init() {
    MX_TIM3_Init();
    MX_TIM6_Init();


}
void Myhal::_spi_init() {
    MX_SPI1_Init();
}



void Myhal::_oled_init() {
    OLED_Init();
    u8g2Init(&canvasBuffer);
    u8g2_SetFont(&canvasBuffer, u8g2_font_myfont); /*字库选择*/
    u8g2_FirstPage(&canvasBuffer);
    //u8g2Init(&canvasBuffer);
    u8g2_FirstPage(&canvasBuffer);
    do
    {
        //draw(&canvasBuffer);
    }while (u8g2_NextPage(&canvasBuffer));

    //延迟一段时间显示初始图样，然后清除
    //HAL_Delay(1000); // 显示1秒后清除
    //u8g2_ClearBuffer(&canvasBuffer);
    //u8g2_SendBuffer(&canvasBuffer);
}

void Myhal::_key_init() {


}


uint8_t receiveData[50];

void Myhal::_usart1_init() {
    MX_USART1_UART_Init();
    //__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,receiveData,50);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}




bool Myhal::_getKey(key::KEY_INDEX _keyIndex) {
    if (_keyIndex == key::KEY_0) return !HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
    if (_keyIndex == key::KEY_1) return !HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
}






void Myhal::_delay(unsigned long _mill) {
    HAL_Delay(_mill);
}

// 获取自系统启动以来的毫秒数
unsigned long Myhal::_millis() {
    return HAL_GetTick();  // HAL_GetTick()返回系统启动后的毫秒数
}

// 获取系统滴答计数（与millis一致，可根据需求调整）
unsigned long Myhal::_getTick() {
    return (uwTick * 1000 + (SysTick->LOAD - SysTick->VAL) / (SystemCoreClock/1000000U));
}

// 获取随机种子（使用当前时间作为种子）
unsigned long Myhal::_getRandomSeed() {
    static uint32_t seed = 0;
    HAL_ADC_Start(&hadc1);//开启ADC1
    HAL_ADC_PollForConversion(&hadc1, 50);//表示等待转换完成
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        seed = HAL_ADC_GetValue(&hadc1);//读取ADC转换数据
    }

    return seed;
}

// 返回u8g2的画布缓冲区指针
void *Myhal::_getCanvasBuffer() {
    // u8g2_GetBufferPtr函数用于获取当前缓冲区的指针
    return u8g2_GetBufferPtr(&canvasBuffer);
}

// 返回缓冲区的瓦片高度（u8g2中瓦片高度通常以8像素为单位，对应OLED的页高度）
uint8_t Myhal::_getBufferTileHeight() {
    // 对于128x64的OLED，高度64像素，瓦片高度为64/8=8（可根据实际屏幕尺寸调整）
    // 也可通过u8g2的接口获取缓冲区高度（单位：像素）后除以8
    return u8g2_GetBufferTileHeight(&canvasBuffer);
}

// 返回缓冲区的瓦片宽度（对应OLED的宽度，单位：像素）
uint8_t Myhal::_getBufferTileWidth() {
    // 直接返回u8g2缓冲区的宽度（通常等于屏幕宽度，如128）
    return u8g2_GetBufferTileWidth(&canvasBuffer);
}

void Myhal::_canvasUpdate() {
    u8g2_SendBuffer(&canvasBuffer);  // 将缓冲区内容发送到屏幕
}

void Myhal::_canvasClear() {

    u8g2_ClearBuffer(&canvasBuffer); // 清空 u8g2 缓冲区

}

void Myhal::_setFont(const unsigned char* _font) {
    u8g2_SetFontMode(&canvasBuffer, 1); /*字体模式选择*/
    u8g2_SetFontDirection(&canvasBuffer, 0); /*字体方向选择*/
    u8g2_SetFont(&canvasBuffer, _font); // 设置 u8g2 字体
}

// 获取字符串的字体宽度（仅支持英文/UTF-8字符）
uint8_t Myhal::_getFontWidth(const std::string &_text) {
    return u8g2_GetUTF8Width(&canvasBuffer, _text.c_str());
}

unsigned char Myhal::_getFontHeight() {
    return u8g2_GetMaxCharHeight(&canvasBuffer);
}

void Myhal::_setDrawType(unsigned char _type) {
    if (ScreenPopUp_Value==0) {
        u8g2_SetDrawColor(&canvasBuffer, _type);
    }
    else if (ScreenPopUp_Value==1) {

        switch (_type) {
            case 0:
                u8g2_SetDrawColor(&canvasBuffer, 1);break;
            case 1:
                u8g2_SetDrawColor(&canvasBuffer, 0);break;
            case 2:
                u8g2_SetDrawColor(&canvasBuffer, 2);break;

        }
    }



}

void Myhal::_drawPixel(float _x, float _y) {
    u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y));
}

void Myhal::_drawEnglish(float _x, float _y, const std::string &_text) {
    u8g2_DrawStr(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void Myhal::_drawChinese(float _x, float _y, const std::string &_text) {
    u8g2_DrawUTF8(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void Myhal::_drawVDottedLine(float _x, float _y, float _h) {
    for (unsigned char i = 0; i < (unsigned char)std::round(_h); i++) {
        if (i % 8 == 0 | (i - 1) % 8 == 0 | (i - 2) % 8 == 0) continue;
        u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y) + i);
    }
}

// 绘制水平虚线（每隔一定像素跳过部分点）
void Myhal::_drawHDottedLine(float _x, float _y, float _l) {
    for (unsigned char i = 0; i < _l; i++) {
        if (i % 8 == 0 | (i - 1) % 8 == 0 | (i - 2) % 8 == 0) continue;
        u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x) + i, (int16_t)std::round(_y));
    }
}

// 绘制垂直线（实线）
void Myhal::_drawVLine(float _x, float _y, float _h) {
    u8g2_DrawVLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_h));
}

// 绘制水平线（实线）
void Myhal::_drawHLine(float _x, float _y, float _l) {
    u8g2_DrawHLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_l));
}

// 绘制位图（XBM格式）
void Myhal::_drawBMP(float _x, float _y, float _w, float _h, const unsigned char *_bitMap) {
    u8g2_DrawXBMP(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), _bitMap);

}

// 绘制实心矩形
void Myhal::_drawBox(float _x, float _y, float _w, float _h) {
    u8g2_DrawBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

// 绘制带圆角的实心矩形
void Myhal::_drawRBox(float _x, float _y, float _w, float _h, float _r) {
    u8g2_DrawRBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}

// 绘制矩形边框（空心）
void Myhal::_drawFrame(float _x, float _y, float _w, float _h) {
    u8g2_DrawFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

// 绘制带圆角的矩形边框（空心）
void Myhal::_drawRFrame(float _x, float _y, float _w, float _h, float _r) {
    u8g2_DrawRFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}


// 屏幕开启（根据硬件特性实现，此处假设OLED默认常亮，仅做示例）
void Myhal::_screenOn() {
    // 实际需根据OLED驱动实现（如通过GPIO控制电源或发送指令）
    u8g2_SetPowerSave(&canvasBuffer, 0); // 关闭省电模式（开启屏幕）
}

// 屏幕关闭（同理，根据硬件实现）
void Myhal::_screenOff() {
    u8g2_SetPowerSave(&canvasBuffer, 1); // 开启省电模式（关闭屏幕）
}



// 实现裁剪区域设置
void Myhal::_setClipRect(int _x, int _y, int _w, int _h) {

    // u8g2_SetClipWindow参数：x0, y0, x1, y1（右下角坐标）
    // 注意转换为u8g2要求的int16_t类型，并确保坐标有效
    int16_t x1 = static_cast<int16_t>(_x + _w - 1);  // 计算右下角X坐标
    int16_t y1 = static_cast<int16_t>(_y + _h - 1);  // 计算右下角Y坐标
    u8g2_SetClipWindow(&canvasBuffer,
                      static_cast<int16_t>(_x),
                      static_cast<int16_t>(_y),
                      x1,
                      y1);
}

// 实现裁剪区域重置（恢复为全屏）
void Myhal::_resetClipRect() {
    // 获取屏幕尺寸（基于现有接口）
    int16_t screenWidth = 128;
    int16_t screenHeight = 64;  // 瓦片高度通常以8像素为单位

    // 设置裁剪区域为全屏
    u8g2_SetClipWindow(&canvasBuffer, 0, 0, screenWidth , screenHeight );
}
