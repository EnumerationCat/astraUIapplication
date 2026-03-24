#include "OLED.h"

void OLED_WriteCommand(uint8_t cmd)
{
    uint8_t rxdata = 0;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI1_DC_GPIO_Port, SPI1_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1, &cmd, &rxdata, 1, 0xffff);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}

void OLED_WriteData(uint8_t data, uint8_t Mode)
{
    if (!Mode)data = ~data;
    uint8_t rxdata = 0;
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI1_DC_GPIO_Port, SPI1_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_TransmitReceive(&hspi1, &data, &rxdata, 1, 0xffff);
    HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
}


/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
    OLED_WriteCommand(0x00 | (X & 0x0F));
}

/**
  * @brief  OLEDȫ����������
  * @param  �������ݣ�0x00Ϊ����
  * @retval ��
  */
void OLED_Fill(uint8_t DAT)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            OLED_WriteData(DAT, 1);
        }
    }
}

/**
  * @brief  OLED��ʼ��
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);
    uint32_t i, j;

    for (i = 0; i < 1000; i++)
    {
        for (j = 0; j < 1000; j++);
    }
    HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);

    OLED_WriteCommand(0xAE);

    OLED_WriteCommand(0xD5);
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8);
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3);
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40);

    OLED_WriteCommand(0xA1);

    OLED_WriteCommand(0xC8);

    OLED_WriteCommand(0xDA);
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81);
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9);
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB);
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4);

    OLED_WriteCommand(0xA6);

    OLED_WriteCommand(0x8D);
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF);
    OLED_Fill(0x00);
}








/*官方提供的Logo绘制demo*/
void draw(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1);
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21, 8, "8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51, 30, "g");
    u8g2_DrawStr(u8g2, 67, 30, "\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_myfont);
    u8g2_DrawStr(u8g2, 1, 54, "github.com/olikraus/u8g2");
    u8g2_SetFont(u8g2,u8g2_font_myfont); // 设置英文大字体
}


uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND: /*通过SPI发送arg_int个字节数据*/
        HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)arg_ptr, arg_int);while(hspi1.TxXferCount);
        //HAL_SPI_Transmit(&hspi2, (uint8_t *)arg_ptr, arg_int, 200);
        break;
    case U8X8_MSG_BYTE_INIT: /*初始化函数*/
        break;
    case U8X8_MSG_BYTE_SET_DC: /*设置DC引脚,表明发送的是数据还是命令*/
        HAL_GPIO_WritePin(SPI1_DC_GPIO_Port, SPI1_DC_Pin, arg_int);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
        u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
        break;
    default:
        return 0;
    }
    return 1;
}


uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: /*delay和GPIO的初始化，在main中已经初始化完成了*/
        break;
    case U8X8_MSG_DELAY_MILLI: /*延时函数*/
        HAL_Delay(arg_int);
        break;
    case U8X8_MSG_GPIO_CS: /*片选信号*/
        HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, arg_int);
        break;
    case U8X8_MSG_GPIO_DC:
        break;
    case U8X8_MSG_GPIO_RESET:
        break;
    }
    return 1;
}

void u8g2Init(u8g2_t *u8g2)
{
    /*
    U8G2_R0： 顺时针旋转0度
    U8G2_R1： 顺时针旋转90度
    U8G2_R2： 顺时针旋转180度
    U8G2_R3： 顺时针旋转270度
    U8G2_MIRROR：U8G2_R0的镜像
    */
    u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
    u8g2_InitDisplay(u8g2); // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
    u8g2_SetPowerSave(u8g2, 0); // 打开显示器
    u8g2_ClearBuffer(u8g2);
    u8g2_SetFontMode(u8g2, 1); /*字体模式选择*/
    u8g2_SetFontDirection(u8g2, 0); /*字体方向选择*/
    u8g2_SetFont(u8g2, u8g2_font_myfont); /*字库选择*/

    //HAL_Delay(1000);
    //u8g2_DrawLine(u8g2, 0, 0, 127, 63);
    //u8g2_DrawLine(u8g2, 127, 0, 0, 63);
    //u8g2_SendBuffer(u8g2);
    //HAL_Delay(1000);
    //
    u8g2_FirstPage(u8g2);
    do
    {
        draw(u8g2);
    } while (u8g2_NextPage(u8g2));
    HAL_Delay(1000);

}

/**
 * @brief  设置OLED亮度（通过调节对比度）
 * @param  contrast 对比度值，范围0x00~0xFF（值越大亮度越高）
 * @retval 无
 */
void OLED_SetBrightness(uint8_t contrast) {
    OLED_WriteCommand(0x81);        // 发送设置对比度指令
    OLED_WriteCommand(contrast);    // 发送对比度值
}




