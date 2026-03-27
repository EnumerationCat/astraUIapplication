#pragma once
#ifndef _GYRO_app_h
#define _GYRO_app_h

#include "item.h"
#include "menu.h"
#include <vector>
#include <string>
#include "MPU6050.h"
namespace astra {

    class GYRO : public Menu {
    private:
        // 页面滑入动画
        bool isEntering = true;
        float posY = 0.0f;
        float frameAnim = 0.0f;
        const float enterStartY = -100.0f; // 入场起始Y坐标

        // 文本滑入动画
        bool isTextAnimating = true;
        float titlePos = -100.0f;
        float pitchPos = 0.0f;
        float rollPos = 0.0f;
        float yawPos = 0.0f;
        float titleTarget = 0.0f;
        float pitchTarget = 0.0f;
        float rollTarget = 0.0f;
        float yawTarget = 0.0f;

        // 陀螺仪数据
        float pitch = 0.0f;
        float roll = 0.0f;
        float yaw = 0.0f;

    public:
        std::string getType() const override;

        // 构造函数
        GYRO();
        explicit GYRO(const std::string& _title);
        GYRO(const std::string& _title, const std::vector<unsigned char>& _pic);

        // 重写父类方法
        void forePosInit() override {}
        void render(const std::vector<float>& _camera) override;
        void deInit() override;
        // 新增：统一重置动画（对齐WiFi/Clock页面）
        void resetEnterAnimation();

        // 生成默认图标
        std::vector<unsigned char> generateDefaultPic();
    };

} // namespace astra

extern astra::GYRO* MyGYRO;
#endif
