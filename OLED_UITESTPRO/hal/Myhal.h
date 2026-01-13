//
// Created by 20127 on 2025/9/24.
//

#ifndef OLED_UI_TEST_MYHAL_H
#define OLED_UI_TEST_MYHAL_H

#ifdef __cplusplus
extern "C" {
#endif

    /*---c---*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dma.h"
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"
#include "spi.h"
#include "adc.h"
#include "usart.h"
#include "OLED.h"
#include <string.h>

    extern uint8_t receiveData[];
    //此处添加其它C头文件

    void CppMain();  //主程序函数


#ifdef __cplusplus
}

/*---c++---*/
#include "hal.h"
#include "cmath"



//此处添加其他Cpp头文件
class Myhal : public HAL {
private:
    void _stm32_hal_init();
    void _sys_clock_init();
    void _gpio_init();
    void _dma_init();
    void _timer_init();
    void _spi_init();
    void _oled_init();
    void _key_init();
    void _usart1_init();

public:
    Myhal() = default;





protected:
    u8g2_t canvasBuffer {};

public:
    inline void init() override {

        _stm32_hal_init();
        _sys_clock_init();
        _gpio_init();
        _dma_init();
        _timer_init();
        _spi_init();
        _oled_init();
        _key_init();
        //_ws2812_Update();
        _usart1_init();



    }


public:
    bool _getKey(key::KEY_INDEX _keyIndex) override;


public:
    void _screenOn() override;
    void _screenOff() override;
public:
    void *_getCanvasBuffer() override;
    uint8_t _getBufferTileHeight() override;
    uint8_t _getBufferTileWidth() override;
    void _canvasClear() override;
    void _canvasUpdate() override;
    uint8_t _getFontWidth(const std::string &_text) override;
    unsigned char _getFontHeight() override;
    void _setDrawType(unsigned char _type) override;
    void _setFont(const unsigned char* _font) override;
    void _drawPixel(float _x, float _y) override;
    void _drawEnglish(float _x, float _y, const std::string& _text) override;
    void _drawChinese(float _x, float _y, const std::string& _text) override;
    void _drawVDottedLine(float _x, float _y, float _h) override;
    void _drawHDottedLine(float _x, float _y, float _l) override;
    void _drawVLine(float _x, float _y, float _h) override;
    void _drawHLine(float _x, float _y, float _l) override;
    void _drawBMP(float _x, float _y, float _w, float _h, const uint8_t *_bitMap) override;
    void _drawBox(float _x, float _y, float _w, float _h) override;
    void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
    void _drawFrame(float _x, float _y, float _w, float _h) override;
    void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;
    void _setClipRect(int _x, int _y, int _w, int _h)override;
    void _resetClipRect() override;
public:
    // 重写时间相关方法
    void _delay(unsigned long _mill) override;  // 已存在，补充override
    unsigned long _millis() override;
    unsigned long _getTick() override;
    unsigned long _getRandomSeed() override;
public:
    //void _updateConfig() override;

};



#endif
#endif




