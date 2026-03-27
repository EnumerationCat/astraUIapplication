#include "Weather_app.h"

namespace astra {

std::string Weather::getType() const {
    return "Weather";
}


Weather::Weather() : Weather("Weather", generateDefaultPic()) {}

Weather::Weather(const std::string& _title) : Weather(_title, generateDefaultPic()) {}

    // 在构造函数中初始化动画变量
Weather::Weather(const std::string& _title, const std::vector<unsigned char>& _pic) {
    // 初始化基类Menu的成员
    this->title = _title;
    this->pic = _pic;
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};

    // 初始化动画参数
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;

    // 初始化数据动画参数
    dataXPos.push_back(DATA_ANIM_START_X);
    dataXTrg.push_back(0);
    dataYPos.push_back(DATA_ANIM_START_Y);
    dataYTrg.push_back(0);

    // 初始化图标展开动画变量
    currentIconWidth = 0;
    currentIconHeight = 0;
    targetIconWidth = 60;
    targetIconHeight = 60;

    // 初始化图标位置（屏幕中心）
    int screenWidth = HAL::getSystemConfig().screenWeight;
    int screenHeight = HAL::getSystemConfig().screenHeight;
    iconXTrg = (screenWidth/2 - targetIconWidth);
    iconYTrg = (screenHeight - targetIconHeight)/2;
    iconX = screenWidth/2;  // 从中心点开始
    iconY = screenHeight/2;

    // 初始化文本右侧插入动画变量
    locationXPos = screenWidth + 100;  // 从屏幕右侧外开始
    textXPos = screenWidth + 100;
    temperatureXPos = screenWidth + 100;

    // 动画状态
    isIconAnimating = true;
    isTextAnimating = true;

    // 计算文本目标位置（在render中会重新计算，这里先初始化）
    locationXTrg = 0;
    textXTrg = 0;
    temperatureXTrg = 0;
}


std::vector<unsigned char> Weather::generateDefaultPic() {
    return {
        0xff, 0x07, 0xfe, 0xff, 0xff, 0x01, 0xf8, 0xff, 0xff, 0xf8, 0xf1, 0xff, 0x7f, 0xfc, 0xe3, 0xff,
        0x7f, 0xfe, 0xc7, 0xff, 0x3f, 0xff, 0x0f, 0xfe, 0x3f, 0xff, 0x0f, 0xf8, 0x3f, 0xff, 0xff, 0xf1,
        0x0f, 0xff, 0xff, 0xe3, 0x03, 0xff, 0xff, 0x67, 0xf1, 0xff, 0xff, 0x67, 0xf9, 0xff, 0xff, 0x6f,
        0xf9, 0xff, 0xff, 0x6f, 0xfd, 0xff, 0xff, 0x6f, 0xfd, 0xff, 0xff, 0x67, 0xf9, 0xff, 0xff, 0x67,
        0xf9, 0xff, 0xff, 0x73, 0xe1, 0xff, 0xff, 0xf0, 0x03, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x3f, 0xcf, 0xf3, 0xfc, 0x1f, 0xc7, 0x71, 0xfc, 0x8f, 0xe3, 0x38, 0xfe,
        0xc7, 0x71, 0x1c, 0xff, 0x6f, 0xdb, 0xb6, 0xff, 0x3f, 0xce, 0xf3, 0xff, 0x1f, 0xc7, 0xf1, 0xff,
        0x8f, 0xe3, 0xf8, 0xff, 0xcf, 0xf3, 0xfc, 0x3f
    };
}

void Weather::forePosInit() {


}






void Weather::render(const std::vector<float>& _camera) {
    Item::updateConfig();

    HAL::setDrawType(1);

    // 检查天气数据是否有效，如果无效则使用默认值
    static bool dataInitialized = false;
    if (!dataInitialized) {
        // 检查是否有有效的天气数据
        if (strlen(results[0].now.code) == 0 || strlen(results[0].location.name) == 0) {
            // 使用默认数据
            strcpy(results[0].now.code, "99"); // UNKNOWN weather code
            strcpy(results[0].location.name, "No Data");
            strcpy(results[0].now.text, "404Error");
            strcpy(results[0].now.temperature, "--");
        }
        dataInitialized = true;
    }

    // 处理进入动画
    if (isEntering) {
        Animation::move(&posY, posYTrg, astraConfig.tileAnimationSpeed);
        if (posY == posYTrg) {
            isEntering = false;
            isActive = true;
        }
    }

    // 锁的位图尺寸（40x40方形）
    const int WeatherWidth = 60;
    const int WeatherHeight = 60;

    // 计算居中坐标
    const int WeatherX = (HAL::getSystemConfig().screenWeight/2 - WeatherWidth);
    const int WeatherY = (HAL::getSystemConfig().screenHeight - WeatherHeight)/2;



    // 计算当前裁剪区域的大小和位置（从中心展开）
    int clipWidth = static_cast<int>(currentIconWidth);
    int clipHeight = static_cast<int>(currentIconHeight);
    int clipX = WeatherX + (WeatherWidth - clipWidth) / 2; // 从中心开始
    int clipY = WeatherY + (WeatherHeight - clipHeight) / 2; // 从中心开始

    // 更新图标展开动画
    if (isIconAnimating) {
        Animation::move(&currentIconWidth, targetIconWidth, 70);
        Animation::move(&currentIconHeight, targetIconHeight, 70);

        // 检查动画是否完成
        if (fabs(currentIconWidth - targetIconWidth) < 0.5f &&
            fabs(currentIconHeight - targetIconHeight) < 0.5f) {
            isIconAnimating = false;
        }
    }

    // 计算文本目标位置
    HAL::setFont(u8g2_font_12x6LED_tr);
    int screenWidth = HAL::getSystemConfig().screenWeight;

    locationXTrg = screenWidth*3/4 - HAL::getFontWidth(results[0].location.name)/2;
    textXTrg = screenWidth*3/4 - HAL::getFontWidth(results[0].now.text)/2;
    temperatureXTrg = screenWidth*3/4 - HAL::getFontWidth(results[0].now.temperature)/2 - 8;

    // 更新文本右侧插入动画
    if (isTextAnimating) {
        Animation::move(&locationXPos, locationXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&textXPos, textXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&temperatureXPos, temperatureXTrg, astraConfig.clokDataAnimationSpeed);

        // 检查动画是否完成
        if (fabs(locationXPos - locationXTrg) < 0.5f &&
            fabs(textXPos - textXTrg) < 0.5f &&
            fabs(temperatureXPos - temperatureXTrg) < 0.5f) {
            isTextAnimating = false;
        }
    }

    // 绘制天气图标（使用裁剪区域实现从中间展开的效果）
    if (currentIconWidth > 0 && currentIconHeight > 0) {
        // 设置裁剪区域，实现从中间展开的效果
        HAL::setClipRect(clipX, clipY, clipWidth, clipHeight);

        // 绘制完整的天气图标（但只会显示裁剪区域内的部分）
        switch (std::stoi(results[0].now.code)) {
            case WEATHER_SUNNY_DAY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, morningSunny);
                break;
            case WEATHER_SUNNY_NIGHT:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, nightClear);
                break;
            case WEATHER_FAIR_DAY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, morningSunny);
                break;
            case WEATHER_FAIR_NIGHT:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, nightClear);
                break;
            case WEATHER_CLOUDY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Cloudy);
                break;
            case WEATHER_PARTLY_CLOUDY_1:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, morningPartlyCloudy);
                break;
            case WEATHER_PARTLY_CLOUDY_2:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, nightPartlyCloudy);
                break;
            case WEATHER_MOSTLY_CLOUDY_1:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, morningPartlyCloudy);
                break;
            case WEATHER_MOSTLY_CLOUDY_2:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, nightPartlyCloudy);
                break;
            case WEATHER_OVERCAST:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Overcast);
                break;
            case WEATHER_SHOWER:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Shower);
                break;
            case WEATHER_THUNDERSHOWER:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Thundershower);
                break;
            case WEATHER_THUNDERSHOWER_HAIL:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Thundershower);
                break;
            case WEATHER_LIGHT_RAIN:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, LightRain);
                break;
            case WEATHER_MODERATE_RAIN:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, ModerateRain);
                break;
            case WEATHER_HEAVY_RAIN:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, HeavyRain);
                break;
            case WEATHER_STORM:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, HeavyRain);
                break;
            case WEATHER_HEAVY_STORM:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, HeavyStorm);
                break;
            case WEATHER_SEVERE_STORM:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, SevereStorm);
                break;
            case WEATHER_ICE_RAIN:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, IceRain);
                break;
            case WEATHER_SLEET:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Sleet);
                break;
            case WEATHER_SNOW_FLURRY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, SnowFlurry);
                break;
            case WEATHER_LIGHT_SNOW:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, LightSnow);
                break;
            case WEATHER_MODERATE_SNOW:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, ModerateSnow);
                break;
            case WEATHER_HEAVY_SNOW:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, HeavySnow);
                break;
            case WEATHER_SNOWSTORM:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Snowstorm);
                break;
            case WEATHER_DUST:
            case WEATHER_SAND:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Dust_Sand);
                break;
            case WEATHER_DUSTSTORM:
            case WEATHER_SANDSTORM:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Duststorm_Sandstorm);
                break;
            case WEATHER_FOGGY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Foggy);
                break;
            case WEATHER_HAZE:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Haze);
                break;
            case WEATHER_WINDY:
            case WEATHER_BLUSTERY:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Windy_Blustery);
                break;
            case WEATHER_HURRICANE:
            case WEATHER_TROPICAL_STORM:
            case WEATHER_TORNADO:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Hurricane_TropicalStorm_Tornado);
                break;
            case WEATHER_COLD:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Cold);
                break;
            case WEATHER_HOT:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, Hot);
                break;
            case WEATHER_UNKNOWN:
            default:
                HAL::drawBMP(WeatherX, WeatherY, WeatherWidth, WeatherHeight, UnknownWeather);
                break;
        }

        // 关闭裁剪区域
        HAL::resetClipRect();
    }
    // 绘制文本（使用动画位置）
    HAL::setFont(u8g2_font_12x6LED_tr);
    const int locationY = (HAL::getSystemConfig().screenHeight - HAL::getFontHeight())/2 - 2;
    const int textY = (HAL::getSystemConfig().screenHeight + HAL::getFontHeight())/2;
    const int TemperatureY = HAL::getSystemConfig().screenHeight/2 + HAL::getFontHeight()*3/2 + 2;

    // 使用动画位置绘制文本
    HAL::drawEnglish(static_cast<int>(locationXPos), locationY, results[0].location.name);
    HAL::drawEnglish(static_cast<int>(textXPos), textY, results[0].now.text);
    HAL::drawEnglish(static_cast<int>(temperatureXPos), TemperatureY, results[0].now.temperature);

    // 绘制温度符号（跟随温度文本动画）
    HAL::drawBMP(static_cast<int>(temperatureXPos + HAL::getFontWidth(results[0].now.temperature)),
                TemperatureY + 2 - HAL::getFontHeight(),
                18, 13, TemperatureSymbol);

    HAL::setFont(getUIConfig().mainFont); // 恢复默认字体
}
    // 重置进入动画
void Weather::resetEnterAnimation() {
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
    isActive = false;

    // 重置图标展开动画
    currentIconWidth = 0;
    currentIconHeight = 0;

    int screenWidth = HAL::getSystemConfig().screenWeight;
    int screenHeight = HAL::getSystemConfig().screenHeight;

    iconX = screenWidth/2;  // 从中心点开始
    iconY = screenHeight/2;

    // 重置文本右侧插入动画
    locationXPos = screenWidth + 100;
    textXPos = screenWidth + 100;
    temperatureXPos = screenWidth + 100;

    // 重置动画状态
    isIconAnimating = true;
    isTextAnimating = true;
}

void Weather::deInit() {
    Menu :: deInit();
}

}  // namespace astra