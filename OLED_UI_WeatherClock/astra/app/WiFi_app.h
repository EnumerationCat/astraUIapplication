#pragma once

#ifndef _WiFi_app_h
#define _WiFi_app_h

#include "item.h"
#include "menu.h"
#include <vector>
#include <string>
#include "esp8266.h"
#include "Clock_app.h"

const unsigned char SwitchOFF[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x01, 0xf8, 0xff, 0x07, 0x3c, 0xff, 0x8f, 0x0e, 0xfc, 0x9f, 0x06, 0xf8, 0xdf, 0x07, 0xf8, 0xff,
    0x07, 0xf8, 0xff, 0x07, 0xf8, 0xff, 0x07, 0xf8, 0xff, 0x07, 0xf8, 0xff, 0x07, 0xf8, 0x9f, 0x0e, 0xfc, 0x9f, 0x3e, 0xff, 0x0f, 0xf8, 0xff, 0x07,
    0xf0, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
};
const unsigned char SwitchON[] = {
    0x00, 0x00, 0x00, 0xf0, 0xff, 0x03, 0x08, 0x00, 0x04, 0x04, 0x00, 0x80, 0x02, 0xe0, 0x50, 0x01, 0xf8, 0x43, 0x01, 0xf8, 0x63, 0x01, 0xfc, 0x67,
    0x01, 0xfc, 0x67, 0x01, 0xfc, 0x67, 0x01, 0xfc, 0x67, 0x01, 0xfc, 0x63, 0x01, 0xf8, 0x63, 0x01, 0xf0, 0x81, 0x02, 0x00, 0x10, 0x04, 0x00, 0x00,
    0x08, 0x00, 0x04, 0xf0, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
namespace astra {



    class WiFi:public Menu {
        // 也可以加个WiFipassword，先不加了，写累了(-_-)
        enum WiFiSelectedItem {
            WiFiID,
            WiFiSwitch,
            WiFiIP,
        };
    private:
        // 选择框动画参数（模仿时钟应用）
        float selectBoxX = 0;
        float selectBoxY = 0;
        float selectBoxW = 0;
        float selectBoxH = 0;
        float selectBoxXTrg = 0;
        float selectBoxYTrg = 0;
        float selectBoxWTrg = 0;
        float selectBoxHTrg = 0;

        WiFiSelectedItem selectedItem = WiFiID;  // 默认选中ID
    public:
        // 动画参数
        bool isEntering = true;
        bool isActive = false;
        float posY = 0;
        float posYTrg = 0;
        float enterStartY = -100;  // 从屏幕上方开始

        // 开关状态相关
        bool wifiEnabled = false;

        const int switchWidth = 22;
        const int switchHeight = 18;
        float switchX = 0;
        float switchY =0;

        // 文本动画
        float titleX = 0;
        float titleY = 0;




        std::string getType() const override;

        // 构造函数声明
        WiFi();
        explicit WiFi(const std::string& _title);
        WiFi(const std::string& _title, const std::vector<unsigned char>& _pic);

        //前景元素初始化（模仿List和Tile）
        void forePosInit() override;

        std::vector<unsigned char> generateDefaultPic();


        void render(const std::vector<float>& _camera) override;

        void resetEnterAnimation();

        // 重写父类的deInit方法
        void deInit() override;
       void popInfoConnect(std::string _info) ;
        void popInfoDisConnect(std::string _info) ;
        // 选择框相关方法
        WiFiSelectedItem getSelectedItem() const { return selectedItem; }
        void setSelectedItem(WiFiSelectedItem item) { selectedItem = item; }
        void setWiFiSwitch(){wifiEnabled = !wifiEnabled;}
        bool getWiFiSwitchState() const { return wifiEnabled; }

    };


}
extern astra::WiFi* MyWiFi;
#endif