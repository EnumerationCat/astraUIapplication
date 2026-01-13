#pragma once
#ifndef CLOCK_APP_H
#define CLOCK_APP_H
#include "OLED.h"
#include "Menu.h"
#include "RTCtimer.h"
#include "Setting_app.h"
#include <vector>
#include <string>


namespace astra {

    class Clock : public Menu {
    private:
        struct DigitAnimation {
            float scrollOffset = 0.0f;  // 滚动偏移量（0 ~ 数字高度，限制滚动范围）
            uint8_t currentValue = 0;   // 当前数字（新值，最终显示的数字）
            uint8_t previousValue = 0;  // 上一次的数字（旧值，滚动时向上消失的数字）
            bool isAnimating = false;   // 是否正在执行滚动动画（避免重复触发）
        };
        struct tm *Nowtime;
        struct tm time = {
            .tm_sec = 55,
            .tm_min = 59,
            .tm_hour = 23,
            .tm_mday = 21-1,
            .tm_mon = 12-1,         //tm_mon是0-11
            .tm_year = 2025 - 1900,
        };
        bool ClocksetFlag = true;
        bool ClockSetPupFlag = false;
        bool enableClip = true;

        // 新增：各元素动画位置与目标位置
        float dateXPos;         // 日期（左上角）X位置
        float dateXTrg;         // 日期目标X位置
        float weekXPos;         // 星期（右上角）X位置
        float weekXTrg;         // 星期目标X位置
        float almXPos;          // Alm（左下角）X位置
        float almXTrg;          // Alm目标X位置
        float stwXPos;          // Stw（右下角）X位置
        float stwXTrg;          // Stw目标X位置
        float settingYPos;      // Setting（下方中间）Y位置
        float settingYTrg;      // Setting目标Y位置

        // 为每个数字位创建动画实例
        DigitAnimation h1Anim, h2Anim, m1Anim, m2Anim, s1Anim, s2Anim;
    public:

        // 用SetTime结构体封装时间变量（初始值均为0）
        struct SetTime {
            int year = 0;        // 年
            int month = 0;       // 月
            int day = 0;         // 日
            int weekDay = 0;     // 星期（0-6）
            int hour = 0;        // 时
            int minute = 0;      // 分
            int second = 0;      // 秒
        };
        // 时间数据（原有）
        uint8_t Hour = 0, Minute = 0, Second = 0;
        int Year = 0, Month = 0, Day = 0,Wday = 0;


        std::string timeStr;  // 格式化时间字符串
        std::string dateStr;    // 日期字符串 2025-09-12
        std::string weekStr;    // 星期英文字符串

        // 动画参数（适配列表风格，原有）
        bool isEntering = true;
        float posY = 0;
        float posYTrg = 0;
        float enterStartY = -systemConfig.screenHeight;
        bool isActive = false;

        // 文本动画参数（原有，可保留用于整体位移）
        std::vector<float> dataXPos;
        std::vector<float> dataXTrg;
        std::vector<float> dataYPos;
        std::vector<float> dataYTrg;
        const float DATA_ANIM_START_X = 0;
        const float DATA_ANIM_START_Y = -50;

        // 冒号呼吸灯参数（原有，可替换为闪烁逻辑）
        int colonBrightness = 0;
        int colonDir = 1;
        // 选择项枚举
        enum SelectedItemClock { ALM, SETTING, STW };
        // 新增：所有按钮的选择状态枚举
        enum SelectedItemCLockSetting {
            CLOSE_BTN,       // 右上角退出
            UP_ARROW,        // 上箭头（滚动到上一行）
            DOWN_ARROW,      // 下箭头（滚动到下一行）
            LEFT_ARROW,      // 左下角箭头
            MINUS_BTN,       // 减号按钮
            SET_BTN,         // 中间正方形设定按钮
            PLUS_BTN,        // 加号按钮
            RIGHT_ARROW      // 右下角箭头
        };

    private:

        SelectedItemClock selectedItemTimer = SETTING;  // 默认选择Setting
        SelectedItemCLockSetting selectedItemTimerSetting= CLOSE_BTN;  // 默认选中退出按钮
        // 选择框动画参数
        float selectBoxX = 0;
        float selectBoxY = 0;
        float selectBoxW = 0;
        float selectBoxH = 0;
        float selectBoxXTrg = 0;
        float selectBoxYTrg = 0;
        float selectBoxWTrg = 0;
        float selectBoxHTrg = 0;

    public:
        // 构造函数
        Clock();
        explicit Clock(const std::string& _title);
        Clock(const std::string& _title, const std::vector<unsigned char>& _pic);

        // 实现Menu基类接口
        std::string getType() const override;
        void forePosInit() override;
        void render(const std::vector<float>& _camera) override;
        void resetEnterAnimation();

        // 生成默认图标
        std::vector<unsigned char> generateDefaultPic();

        // 退出动画控制
        void setIsExiting(bool value);
        bool isExitAnimationFinished() const;
        void ClockPopup(std::string _info);
    private:
        // 时间更新逻辑（新增数字分解）
        void updateTime();
    public:
        void DisableClip() {
           enableClip = false;
        }
        void EnableClip() {
            enableClip = true ;
        }
        bool getClip() {
            return enableClip;
        }
        SelectedItemClock getSelectedItem() {
            return selectedItemTimer;
        }

    };
}  // namespace astra
extern astra::Clock* MyClock;
#endif // CLOCK_APP_H