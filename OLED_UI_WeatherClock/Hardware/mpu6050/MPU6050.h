#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "main.h"
#include "stm32f1xx_hal.h"
/*
bilibili 澪智菌
@time时间: 2025.5.5
@version版本:V1_4
@Encoding :UTF-8
@attention : 注意杜邦线别用劣质的，还有如果零飘严重可以进入函数MPU6050_Get_Angle()里进行修改，里面会告诉你怎么做
1.增加了使用四元素得到欧拉角的函数
2.引入了软件矫正零点漂移函数，用于修正z轴零点漂移(MPU6050_Get_Angle)
3.积分法

if you can't display Chinese correctly,please check your encoding mode(please set encoding mode to UTF-8)
*/

#ifdef __cplusplus
extern "C" {
#endif
    //陀螺仪原生数据结构体
    typedef struct MPU6050_raw
    {
        int16_t AccX;
        int16_t AccY;
        int16_t AccZ;
        int16_t GyroX;
        int16_t GyroY;
        int16_t GyroZ;
        uint16_t Temp;
    }MPU6050_raw;
    //陀螺仪角度结构体
    typedef struct MPU6050
    {
        float yaw;
        float roll;
        float pitch;
    }MPU6050;

    extern MPU6050 MM;
    extern uint8_t MPU6050_flag;
    /////////////////////////常用函数//////////////////////////
    void MPU6050_Init(void);
    //void MPU6050_init(I2C_HandleTypeDef *hi2c);   												//初始化姿态传感器,使用硬件SPI
    void MPU6050_Get_Angle_Plus(MPU6050* mpu6050data) ;                        //得到角度(四元素法+互补滤波)
    void MPU6050_Get_Angle(MPU6050* mpu6050data);                              //得到角度(正常方法+卡尔曼滤波+互补滤波)
    float MPU6050_GetTemp(void);                                        //得到温度

    /////////////////////////扩展函数//////////////////////////
    uint8_t MPU6050_ID(void);                                           //读取ID
    void MPU6050_Get_Raw(MPU6050_raw* mpu6050data);                            //得到原生数据

#ifdef __cplusplus
}
#endif

#endif
