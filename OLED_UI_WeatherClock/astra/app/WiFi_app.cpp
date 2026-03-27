#include "WiFi_app.h"


namespace astra {


std::string WiFi::getType() const {
    return "WiFi";
}

// 默认构造函数
WiFi::WiFi() : WiFi("WiFi", generateDefaultPic()) {}

// 带标题的构造函数
WiFi::WiFi(const std::string& _title) : WiFi(_title, generateDefaultPic()) {}

// 主构造函数
WiFi::WiFi(const std::string& _title, const std::vector<unsigned char>& _pic) {
    // 初始化基类Menu的成员
    this->title = _title;
    this->pic = _pic;
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};

    // 初始化动画参数 - 从屏幕上方滑入
    isEntering = true;
    posY = enterStartY;  // 开始位置在屏幕上方
    posYTrg = 0;         // 目标位置在屏幕顶部



    // 初始化文本位置
    titleX = 0;
    titleY = -50;  // 从屏幕外开始
    // 初始化选择框位置
    selectBoxX = selectBoxY = selectBoxW = selectBoxH = 0;
    selectBoxXTrg = selectBoxYTrg = selectBoxWTrg = selectBoxHTrg = 0;

    // 初始化开关位置（将在render中计算）
    switchX = switchY = 0;




}

std::vector<unsigned char> WiFi::generateDefaultPic() {
    // 简单的WiFi图标位图数据
    return {
    0xff, 0xff, 0xff, 0xff, 0xff, 0x07, 0xe0, 0xff, 0xff, 0x01, 0x80, 0xff, 0x7f, 0x00, 0x00, 0xfe,
    0x3f, 0x00, 0x00, 0xfc, 0x1f, 0xf0, 0x0f, 0xf8, 0x0f, 0xfc, 0x3f, 0xf0, 0x07, 0xff, 0xff, 0xe0,
    0xc3, 0x0f, 0xf0, 0xc3, 0xc1, 0x03, 0xc0, 0x83, 0xe1, 0x01, 0x80, 0x87, 0xf1, 0x80, 0x03, 0xcf,
    0x7f, 0xf0, 0x0f, 0xfe, 0x3f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f, 0x1e, 0x78, 0xfc,
    0xff, 0x0f, 0xf0, 0xff, 0xff, 0x07, 0xe0, 0xff, 0xff, 0x07, 0xe0, 0xff, 0xff, 0xc7, 0xe3, 0xff,
    0xff, 0xe7, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xfc, 0xff, 0xff, 0x1f, 0xf8, 0xff,
    0xff, 0x1f, 0xf8, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0x1f, 0xf8, 0xff, 0xff, 0x1f, 0xf8, 0xff,
    0xff, 0x3f, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff
    };
}

void WiFi::forePosInit() {
    // 如果需要初始化前景元素，可以在这里实现
}

void WiFi::render(const std::vector<float>& _camera) {
    Item::updateConfig();

    HAL::setDrawType(1);

    // 处理进入动画 - 从上方滑入
    if (isEntering) {
        Animation::move(&posY, posYTrg, getUIConfig().listAnimationSpeed);
        if (posY == posYTrg) {
            isEntering = false;
            isActive = true;
        }
    }
    HAL::drawRFrame(0, posY, 126, 64, 2);
    int screenWidth = HAL::getSystemConfig().screenWeight;
    int screenHeight = HAL::getSystemConfig().screenHeight;

    // 计算各元素位置
    int WiFiIDX = (screenWidth - HAL::getFontWidth("ID: Wi-Fi")) / 2;
    int WiFiIDY = HAL::getFontHeight() + posY+2;
    int WiFiIPX = (screenWidth - HAL::getFontWidth("IP: 000.000.000.000")) / 2;
    int WiFiIPY = screenHeight - HAL::getFontHeight() + posY+6;

    // 计算开关位置（居中）
    switchX = screenWidth / 2 - switchWidth / 2;
    switchY = screenHeight / 2 - switchHeight / 2 - 3 + posY;

    // 计算各元素尺寸
    int fontHeight = HAL::getFontHeight();
    int idWidth = HAL::getFontWidth("ID: Wi-Fi");
    int ipWidth = HAL::getFontWidth("IP: 000.000.000.000");
    int IP_Width = HAL::getFontWidth("IP: ");
    // 计算选择框目标位置（模仿时钟应用的选择框）
    switch (selectedItem) {
        case WiFiID:
            selectBoxXTrg = WiFiIDX - 2;
            selectBoxYTrg = WiFiIDY - HAL::getFontHeight() - 2;
            selectBoxWTrg = idWidth + 4;
            selectBoxHTrg = fontHeight + 4;
            break;
        case WiFiSwitch:
            selectBoxXTrg = switchX - 2;
            selectBoxYTrg = switchY - 2;
            selectBoxWTrg = switchWidth + 4;
            selectBoxHTrg = switchHeight + 4;
            break;
        case WiFiIP:
            selectBoxXTrg = WiFiIPX - 2;
            selectBoxYTrg = WiFiIPY - HAL::getFontHeight() - 2;
            selectBoxWTrg = ipWidth + 4;
            selectBoxHTrg = fontHeight + 4;
            break;
    }

    // 更新选择框动画
    Animation::move(&selectBoxX, selectBoxXTrg, 90);
    Animation::move(&selectBoxY, selectBoxYTrg, 90);
    Animation::move(&selectBoxW, selectBoxWTrg, 90);
    Animation::move(&selectBoxH, selectBoxHTrg, 90);
    HAL::setDrawType(1);
    // 绘制ID和IP文本
    HAL::drawEnglish(WiFiIDX, WiFiIDY, "ID: Wi-Fi");

    // 绘制开关
    if (wifiEnabled) {
        HAL::drawBMP(switchX, switchY, switchWidth, switchHeight, SwitchON);
        HAL::drawEnglish(WiFiIPX, WiFiIPY, "IP: ");
        HAL::drawEnglish(WiFiIPX+IP_Width, WiFiIPY, wifiInfo.ip);
    } else {
        HAL::drawBMP(switchX, switchY, switchWidth, switchHeight, SwitchOFF);
        HAL::drawEnglish(WiFiIPX, WiFiIPY, "IP: 000.000.000.000");
    }

    // 绘制选择框（反色绘制，模仿时钟应用）
    HAL::setDrawType(2);  // 反色绘制
    HAL::drawRBox(selectBoxX, selectBoxY, selectBoxW, selectBoxH, 2);
    HAL::setDrawType(1);  // 恢复正常绘制

    // 按键处理（模仿时钟应用的按键处理）
    if (isActive) {
        HAL::keyScan();
        if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;

            for (unsigned char i = 0; i < key::KEY_NUM; i++) {
                if (HAL::getKeyMap()[i] == key::CLICK) {
                    // 左键（上移）
                    if (i == 0) {
                        selectedItem = static_cast<WiFiSelectedItem>(
                            (selectedItem - 1 + 3) % 3
                        );
                    }
                    // 右键（下移）
                    if (i == 1) {
                        selectedItem = static_cast<WiFiSelectedItem>(
                            (selectedItem + 1) % 3
                        );
                    }

                }
            }

            // 清空按键状态
            std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
        }
    }


}

void WiFi::resetEnterAnimation() {
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
    isActive = false;

    // 重置标题位置
    titleY = -50;

    // 重置选择框位置（模仿时钟应用）
    selectBoxX = selectBoxY = selectBoxW = selectBoxH = 0;
    selectBoxXTrg = selectBoxYTrg = selectBoxWTrg = selectBoxHTrg = 0;

    // 重置选中项（默认选中开关）
    selectedItem = WiFiID;
}

void WiFi::deInit() {
    // 清理资源
    Menu::deInit();
}

//弹窗
void WiFi::popInfoConnect(std::string _info) {
    static bool init = false;
    static bool onRender = false;

    if (!init) {
        init = true;
        onRender = true;
    }

    float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin;  //宽度
    float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin;  //高度
    float yPop = 0 - hPop - 8; //从屏幕上方滑入
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;  //目标位置 中间偏上
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;  //居中

    while (onRender) {


        HAL::canvasClear();
        //Setting::Screen_Settig();

        HAL::setDrawType(0);
        HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
        HAL::setDrawType(1);  //反色显示
        HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius);  //绘制一个圆角矩形
        //drawEnglish
        HAL::drawEnglish(xPop + getUIConfig().popMargin,
                         yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                         _info);  //绘制文字




        HAL::canvasUpdate();

        Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed);  //动画
        if (yPop==yPopTrg&&WiFi_Connect_Flag==0) {
            HAL::wifiConnect("Wi-Fi","chuang123.");
            MyClock->setTime();
            if (WiFi_Connect_Flag) yPopTrg = 0 - hPop - 8;  //滑出
        }




        if (yPop == 0 - hPop - 8) {
            onRender = false;  //退出条件
            init = false;
        }
    }
}
    //弹窗
    void WiFi::popInfoDisConnect(std::string _info) {
    static bool init = false;
    static bool onRender = false;

    if (!init) {
        init = true;
        onRender = true;
    }

    float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin;  //宽度
    float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin;  //高度
    float yPop = 0 - hPop - 8; //从屏幕上方滑入
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;  //目标位置 中间偏上
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;  //居中

    while (onRender) {


        HAL::canvasClear();
        //Setting::Screen_Settig();

        HAL::setDrawType(0);
        HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
        HAL::setDrawType(1);  //反色显示
        HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius);  //绘制一个圆角矩形
        //drawEnglish
        HAL::drawEnglish(xPop + getUIConfig().popMargin,
                         yPop + getUIConfig().popMargin + HAL::getFontHeight(),
                         _info);  //绘制文字




        HAL::canvasUpdate();

        Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed);  //动画
        if (yPop==yPopTrg&&WiFi_Connect_Flag==1) {
            HAL::wifiDisConnect();


        }
        if (!WiFi_Connect_Flag) yPopTrg = 0 - hPop - 8;  //滑出


        if (yPop == 0 - hPop - 8) {
            onRender = false;  //退出条件
            init = false;
        }
    }
}

}  // namespace astra