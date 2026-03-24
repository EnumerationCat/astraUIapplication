#pragma once
#ifndef ASTRA_APP_SETTING_H_
#define ASTRA_APP_SETTING_H_

#include "menu.h"
#include <vector>
#include <string>
#include "astra_rocket.h"
namespace astra {
//设置
    class Setting {
        uint8_t brightness=200;
    public:
       void BrightnessPopup(std::string _info, Menu* currentMenu, Selector* selector, Camera* camera);
       static  void Screen_Settig(void);
    };

    extern Setting *BrightnessSetting;
//硬件信息
    class HardwareInfo : public Menu {
    public:
        int startLine = 0;
        const int maxDisplayLines = 4; // 固定显示4行
        std::vector<std::string> infoData;  // 硬件信息数据

        // 动画参数（适配列表风格）
        bool isEntering = true;
        float posY = 0;
        float posYTrg = 0;
        float enterStartY = -systemConfig.screenHeight;
        bool isActive = false;

        // 文本动画参数
        std::vector<float> dataXPos;
        std::vector<float> dataXTrg;
        std::vector<float> dataYPos;
        std::vector<float> dataYTrg;
        const float DATA_ANIM_START_X = -50;
        const float DATA_ANIM_START_Y = 38;

        // 布局参数（使用列表相关配置保持一致性）
        const int DISPLAY_LINE_HEIGHT = astraConfig.listLineHeight; // 与列表行高一致
        const int BOX_Y_OFFSET = 13;
        const int Title_Y_OFFSET = 11;
        const float RADIUS = 3.0f;

    public:
        std::string getType() const override;


        // 构造函数（无图标，适配列表项）
        HardwareInfo();
        explicit HardwareInfo(const std::string& _title);

        void forePosInit() override;
        void render(const std::vector<float>& _camera) override;
        void resetEnterAnimation();

    private:
        void initHardwareData(); // 初始化硬件信息
    };
}

#endif