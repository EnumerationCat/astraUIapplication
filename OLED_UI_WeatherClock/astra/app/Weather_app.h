#pragma once
#ifndef _Weather_app_H_
#define _Weather_app_H_

#include "item.h"
#include "menu.h"
#include <vector>
#include <string>
#include "WeatherBMP.h"
#include "esp8266.h"
namespace astra {
class Weather: public Menu {
public:
    // 动画参数
    bool isEntering = true;
    bool isActive = false;
    float posY = 0;
    float posYTrg = 0;
    float enterStartY = -systemConfig.screenHeight;

    // 图标展开动画变量
    float currentIconWidth;
    float currentIconHeight;
    float targetIconWidth;
    float targetIconHeight;
    float iconX;
    float iconY;
    float iconXTrg;
    float iconYTrg;

    // 文本右侧插入动画变量
    float locationXPos;
    float locationXTrg;
    float textXPos;
    float textXTrg;
    float temperatureXPos;
    float temperatureXTrg;

    // 动画状态
    bool isIconAnimating;
    bool isTextAnimating;

    // 文本动画参数
    std::vector<float> dataXPos;
    std::vector<float> dataXTrg;
    std::vector<float> dataYPos;
    std::vector<float> dataYTrg;
    const float DATA_ANIM_START_X = -50;
    const float DATA_ANIM_START_Y = 38;


    // 天气代码枚举
    typedef enum {
        WEATHER_SUNNY_DAY = 0,           // 晴（国内城市白天晴）
        WEATHER_SUNNY_NIGHT = 1,         // 晴（国内城市夜晚晴）
        WEATHER_FAIR_DAY = 2,            // 晴（国外城市白天晴）
        WEATHER_FAIR_NIGHT = 3,          // 晴（国外城市夜晚晴）
        WEATHER_CLOUDY = 4,              // 多云
        WEATHER_PARTLY_CLOUDY_1 = 5,     // 晴间多云
        WEATHER_PARTLY_CLOUDY_2 = 6,     // 晴间多云
        WEATHER_MOSTLY_CLOUDY_1 = 7,     // 大部多云
        WEATHER_MOSTLY_CLOUDY_2 = 8,     // 大部多云
        WEATHER_OVERCAST = 9,            // 阴
        WEATHER_SHOWER = 10,             // 阵雨
        WEATHER_THUNDERSHOWER = 11,      // 雷阵雨
        WEATHER_THUNDERSHOWER_HAIL = 12, // 雷阵雨伴有冰雹
        WEATHER_LIGHT_RAIN = 13,         // 小雨
        WEATHER_MODERATE_RAIN = 14,      // 中雨
        WEATHER_HEAVY_RAIN = 15,         // 大雨
        WEATHER_STORM = 16,              // 暴雨
        WEATHER_HEAVY_STORM = 17,        // 大暴雨
        WEATHER_SEVERE_STORM = 18,       // 特大暴雨
        WEATHER_ICE_RAIN = 19,           // 冻雨
        WEATHER_SLEET = 20,              // 雨夹雪
        WEATHER_SNOW_FLURRY = 21,        // 阵雪
        WEATHER_LIGHT_SNOW = 22,         // 小雪
        WEATHER_MODERATE_SNOW = 23,      // 中雪
        WEATHER_HEAVY_SNOW = 24,         // 大雪
        WEATHER_SNOWSTORM = 25,          // 暴雪
        WEATHER_DUST = 26,               // 浮尘
        WEATHER_SAND = 27,               // 扬沙
        WEATHER_DUSTSTORM = 28,          // 沙尘暴
        WEATHER_SANDSTORM = 29,          // 强沙尘暴
        WEATHER_FOGGY = 30,              // 雾
        WEATHER_HAZE = 31,               // 霾
        WEATHER_WINDY = 32,              // 风
        WEATHER_BLUSTERY = 33,           // 大风
        WEATHER_HURRICANE = 34,          // 飓风
        WEATHER_TROPICAL_STORM = 35,     // 热带风暴
        WEATHER_TORNADO = 36,            // 龙卷风
        WEATHER_COLD = 37,               // 冷
        WEATHER_HOT = 38,                // 热
        WEATHER_UNKNOWN = 99             // 未知
    } WeatherCode;
    WeatherCode weathercode = WEATHER_UNKNOWN;
public:
    std::string getType() const override;

    // 构造函数声明
    Weather();
    explicit Weather(const std::string& _title);
    Weather(const std::string& _title, const std::vector<unsigned char>& _pic);

    //前景元素初始化（模仿List和Tile）
    void forePosInit() override;

    std::vector<unsigned char> generateDefaultPic();


    void render(const std::vector<float>& _camera) override;

public:

    void resetEnterAnimation();
public:
    // 重写父类的deInit方法
    void deInit() override;  // 添加此声明


};
}  // namespace astra

#endif