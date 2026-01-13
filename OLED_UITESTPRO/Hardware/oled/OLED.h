#ifndef __OLED_H__
#define __OLED_H__
#include "main.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "u8g2.h"

void OLED_WriteCommand(uint8_t cmd);
void OLED_WriteData(uint8_t data, uint8_t Mode);
void OLED_Init(void);
void OLED_Fill(uint8_t DAT);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_SetBrightness(uint8_t contrast);

void draw(u8g2_t *u8g2);
uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
void u8g2Init(u8g2_t *u8g2);
#endif
