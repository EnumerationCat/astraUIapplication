//
// Created by Fir on 2024/3/7 007.
// 此文件的作用是引导STM32进入astra UI 基于C++
// this file is used to guide STM32 into astra UI based on C++
//

#ifndef ASTRA_CORE_SRC_ASTRA_ASTRA_ROCKET_H_
#define ASTRA_CORE_SRC_ASTRA_ASTRA_ROCKET_H_

#include "astra_rocket.h"
#ifdef __cplusplus
extern "C" {
#endif

/*---- C ----*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "OLED.h"
#include "ws2812.h"
#include "u8g2.h"
void CppMain(void);
void astraCoreInit(void);
void astraCoreStart(void);
void astraCoreTest(void);
void astraCoreDestroy(void);

/*---- C ----*/

#ifdef __cplusplus
}

/*---- Cpp ----*/


#include "hal.h"
#include "Myhal.h"
#include "config.h"
#include "OLED.h"
#include "item.h"
#include "launcher.h"

#include "astra_logo.h"
#include "Clock_app.h"

extern bool checkBox_Value;
extern astra::CheckBox* checkBox;
extern unsigned char ScreenPopUp_Value;
extern astra::PopUp*ScreenPopUp;
extern unsigned char Slider_Value;
extern astra::Slider*slider;


//extern astra::Launcher* astraLauncher;
//extern astra::Menu* rootPage;
/*---- Cpp ----*/


#endif
#endif //ASTRA_CORE_SRC_ASTRA_ASTRA_ROCKET_H_
