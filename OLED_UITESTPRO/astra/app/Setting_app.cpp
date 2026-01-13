#include "Setting_app.h"

namespace astra {

Setting *BrightnessSetting = new Setting();
void Setting::Screen_Settig() {

    // 中心坐标（屏幕中心）
    static const float centerX = 64;
    static const float centerY = 32;
    // 目标尺寸（全屏）
    static const float targetW = 128;
    static const float targetH = 64;

    // 动画变量：当前宽高、动画状态、展开/收起标志
    static float currentW = 0;
    static float currentH = 0;
    static bool isAnimating = false;
    static bool isExpanding = false;  // true=展开，false=收起

    // 背景色为白色时
    if (ScreenPopUp_Value == 1) {

        if (!isAnimating && !isExpanding) {
            // 初始化展开状态
            isExpanding = true;
            isAnimating = true;
            currentW = 0;  // 从0开始展开
            currentH = 0;
        }

        // 执行展开动画
        if (isExpanding && isAnimating) {
            Animation::move(&currentW, targetW, 80);  // 从0到全屏宽度
            Animation::move(&currentH, targetH, 80);  // 从0到全屏高度

            // 检查展开是否完成（考虑浮点数精度）
            if (std::abs(currentW - targetW) < 0.1f && std::abs(currentH - targetH) < 0.1f) {
                currentW = targetW;
                currentH = targetH;
                isAnimating = false;  // 展开结束
            }
        }
    }
    // 2. 触发收起动画（当需要关闭弹窗时）
    else {
        if (!isAnimating && isExpanding) {
            // 初始化收起状态（从全屏开始）
            isExpanding = false;
            isAnimating = true;
            currentW = targetW;  // 从全屏开始收起
            currentH = targetH;
        }

        // 执行收起动画
        if (!isExpanding && isAnimating) {
            Animation::move(&currentW, 0, 80);  // 从全屏宽度缩小到0
            Animation::move(&currentH, 0, 80);  // 从全屏高度缩小到0

            // 检查收起是否完成
            if (std::abs(currentW) < 0.1f && std::abs(currentH) < 0.1f) {
                currentW = 0;
                currentH = 0;
                isAnimating = false;  // 收起结束
            }
        }
    }

    // 绘制白色背景（随动画缩放，中心不变）
    if (currentW > 0 && currentH > 0) {  // 宽高>0时才绘制，避免无效操作
        HAL::setDrawType(2);  // 白色绘制模式（根据实际配置调整）
        // 始终以中心为原点偏移，确保缩放时中心不变
        HAL::drawBox(
          centerX - currentW / 2,  // X坐标：中心向左偏移半宽
          centerY - currentH / 2,  // Y坐标：中心向上偏移半高
          currentW,
          currentH
        );
    }

}

void Setting::BrightnessPopup(std::string _info, Menu* currentMenu, Selector* selector, Camera* camera) {
    static bool init = false;
    static bool onRender = false;

    static int brightnessValue = this->brightness;
    static const int minBrightness = 0;
    static const int maxBrightness = 200;

    // 选择状态枚举（0:关闭按钮 1:减号按钮 2:加号按钮）
    enum SelectedItem { CLOSE_BTN, MINUS_BTN, PLUS_BTN };
    static SelectedItem selectedItem = CLOSE_BTN;  // 默认选中关闭按钮
    // 选择框动画参数
    static float selectBoxX = 0, selectBoxY = 0;
    static float selectBoxW = 0, selectBoxH = 0;
    static float targetX = 0, targetY = 0;
    static float targetW = 0, targetH = 0;

    if (!init) {
        init = true;
        onRender = true;
        selectedItem = CLOSE_BTN;  // 重置选择状态
    }

    float wPop = 100;
    float hPop = 30;
    float yPop = 0 - hPop - 8;
    float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 4;
    float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;

    bool isAnimFinished = std::abs(yPop - yPopTrg) < 0.1f;

    while (onRender) {


        // 元素坐标定义（随yPop实时更新）
        const float exitMargin = 0;
        const int exitSize = 10;
        int exitX = static_cast<int>(xPop + wPop - exitMargin - exitSize);
        int exitY = static_cast<int>(yPop + exitMargin);  // 依赖yPop，需实时计算

        float barMargin = 5;
        float btnSize = 12;
        float minusBtnX = xPop + barMargin;
        float plusBtnX = xPop + wPop - barMargin - btnSize;
        float barY = yPop + 15;  // 依赖yPop，需实时计算
        float btnY = barY + 14;  // 依赖barY（即依赖yPop），需实时计算

        // 更新选择框目标位置（根据当前选中项）
        switch (selectedItem) {
            case CLOSE_BTN:
                targetX = exitX - 2;
                targetY = exitY - 2;
                targetW = exitSize + 4;
                targetH = exitSize + 4;
                break;
            case MINUS_BTN:
                targetX = minusBtnX - 2;
                targetY = btnY - 2;
                targetW = btnSize + 4;
                targetH = btnSize + 4;
                break;
            case PLUS_BTN:
                targetX = plusBtnX - 2;
                targetY = btnY - 2;
                targetW = btnSize + 4;
                targetH = btnSize + 4;
                break;
        }

        HAL::canvasClear();
        Setting::Screen_Settig();
        currentMenu->render(camera->getPosition());
        selector->render(camera->getPosition());
        camera->update(currentMenu, selector);

        // 绘制弹窗背景和边框
        HAL::setDrawType(0);
        HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 20, getUIConfig().popRadius + 2);
        HAL::setDrawType(1);
        HAL::drawRFrame(xPop - 4, yPop - 4, wPop + 8, hPop + 20, getUIConfig().popRadius + 2);

        // 绘制右上角退出号（×）
        HAL::drawRFrame(exitX, exitY, exitSize+1, exitSize+1, 2);
        for (int i = 0; i <= exitSize; ++i) {
            HAL::drawPixel(exitX + i, exitY + i);
            HAL::drawPixel(exitX + exitSize - i, exitY + i);
        }

        // 绘制文字标题
        HAL::drawEnglish((HAL::getSystemConfig().screenWeight - HAL::getFontWidth(_info)) / 2,
                         yPop - 2 + HAL::getFontHeight(),
                         _info);

        // 绘制进度条
        float barWidth = wPop - 2 * barMargin;
        HAL::setDrawType(1);
        float progressWidth = (barWidth - 2) * brightnessValue / maxBrightness;
        progressWidth = std::max(progressWidth, 2.0f);
        HAL::drawRBox(xPop + barMargin + 1, barY + 1, progressWidth, 6, 1);
        HAL::drawRFrame(xPop + barMargin, barY, barWidth, 8, 1);

        // 绘制亮度百分比
        int percentage = static_cast<int>((brightnessValue * 100.0f) / maxBrightness + 0.5f);
        std::string brightnessText = std::to_string(percentage) + "%";
        HAL::drawEnglish(
            (HAL::getSystemConfig().screenWeight - HAL::getFontWidth(brightnessText)) / 2,
            barY + 8 + 10,
            brightnessText
        );

        // 绘制减号按钮
        HAL::drawRFrame(minusBtnX, btnY, btnSize, btnSize, 2);
        HAL::drawHLine(minusBtnX + 2, btnY + btnSize/2, btnSize - 4);

        // 绘制加号按钮
        HAL::drawRFrame(plusBtnX, btnY, btnSize, btnSize, 2);
        HAL::drawHLine(plusBtnX + 2, btnY + btnSize/2, btnSize - 4);
        HAL::drawVLine(plusBtnX + btnSize/2, btnY + 2, btnSize - 4);

        // 绘制选择框（带动画效果）
        Animation::move(&selectBoxX, targetX, getUIConfig().popSpeed);
        Animation::move(&selectBoxY, targetY, getUIConfig().popSpeed);
        Animation::move(&selectBoxW, targetW, getUIConfig().popSpeed);
        Animation::move(&selectBoxH, targetH, getUIConfig().popSpeed);
        HAL::setDrawType(2);  // 高亮选择框（反色）
        HAL::drawRBox(selectBoxX, selectBoxY, selectBoxW, selectBoxH, 2);
        HAL::setDrawType(1);  // 恢复绘制类型

        HAL::canvasUpdate();

        Animation::move(&yPop, yPopTrg, getUIConfig().popSpeed);
        isAnimFinished = std::abs(yPop - yPopTrg) < 0.1f;
        HAL::keyScan();

        // 按键处理
        if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;

            for (unsigned char i = 0; i < key::KEY_NUM; i++) {
                // 长按首次触发（PRESS）：单次增减/关闭
                if (HAL::getKeyMap()[i] == key::PRESS && isAnimFinished) {
                    if (i == 1) {
                        switch (selectedItem) {
                            case CLOSE_BTN:
                                yPopTrg = 0 - hPop - 20;  // 关闭弹窗
                                break;
                        }
                    } else if (i == 0) {
                        // 可添加KEY_0的PRESS逻辑，暂无则留空
                        switch (selectedItem) {
                            case MINUS_BTN:
                                brightnessValue = std::max(brightnessValue - 2, minBrightness);
                                slider->sub();
                                OLED_SetBrightness(brightnessValue);
                                break;
                            case PLUS_BTN:
                                brightnessValue = std::min(brightnessValue + 2, maxBrightness);
                                slider->add();
                                OLED_SetBrightness(brightnessValue);
                                break;
                        }
                    }
                }
                // 持续长按（HOLD）：持续增减亮度
                else if (HAL::getKeyMap()[i] == key::HOLD && isAnimFinished) {
                    if (i == 1) {
                        switch (selectedItem) {
                            case MINUS_BTN:
                                brightnessValue = std::max(brightnessValue - 2, minBrightness);
                                slider->sub();
                                OLED_SetBrightness(brightnessValue);
                                break;
                            case PLUS_BTN:
                                brightnessValue = std::min(brightnessValue + 2, maxBrightness);
                                slider->add();
                                OLED_SetBrightness(brightnessValue);
                                break;
                            default:
                                break;
                        }
                    }
                }
                // 短按选择：切换选中项
                else if (HAL::getKeyMap()[i] == key::CLICK && isAnimFinished) {
                    if (i == 0) {
                        selectedItem = static_cast<SelectedItem>((selectedItem - 1 + 3) % 3);
                    } else if (i == 1) {
                        selectedItem = static_cast<SelectedItem>((selectedItem + 1) % 3);
                    }
                }
            }

            std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
        }

        // 退出条件
        if (yPop == 0 - hPop - 20) {
            onRender = false;
            init = false;
            this->brightness = brightnessValue;
        }
    }
}
    std::string HardwareInfo::getType() const {
    return "HardwareInfo";
}
HardwareInfo::HardwareInfo() : HardwareInfo("Device Info") {}

HardwareInfo::HardwareInfo(const std::string& _title) {
    this->title = _title;
    this->pic = generateDefaultPic(); // 空图标
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};

    initHardwareData();

    // 初始化文本动画参数
    for (size_t i = 0; i < infoData.size(); ++i) {
        dataXPos.push_back(DATA_ANIM_START_X);
        dataXTrg.push_back(0);
        dataYPos.push_back(DATA_ANIM_START_Y);
        dataYTrg.push_back(0);
    }

    // 初始化进入动画
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
}

void HardwareInfo::initHardwareData() {
    // 硬件信息内容
    infoData.push_back("Version: AstraUI v2.0");
    infoData.push_back("MCU: STM32F103RCT6");
    infoData.push_back("RAM:   20KB");
    infoData.push_back("FLASH: 64KB");

}


void HardwareInfo::forePosInit() {
    // 初始化前景元素（无滚动条，固定内容）
}

void HardwareInfo::resetEnterAnimation() {
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
    isActive = false;
}

void HardwareInfo::render(const std::vector<float>& _camera) {
    Item::updateConfig();
    HAL::setDrawType(1);

    // 处理进入动画
    if (isEntering) {
        Animation::move(&posY, posYTrg, astraConfig.hardwareInfoAnimationSpeed); // 使用列表动画速度
        if (posY == posYTrg) {
            isEntering = false;
            isActive = true;
        }
    }

    // 绘制标题栏
    const int boxX = 42;
    const int DataboxX = 0;
    const int boxY = BOX_Y_OFFSET + posY;
    const int boxWidth = systemConfig.screenWeight;
    const int boxHeight = systemConfig.screenHeight - BOX_Y_OFFSET;

    // 标题背景
    HAL::drawRBox(boxX-1, -posY, HAL::getFontWidth(title) +1, HAL::getFontHeight() - 1, 2);
    // 标题文字
    HAL::setDrawType(1);
    HAL::drawEnglish(0, -posY + 10, ">>>>>>>");
    HAL::drawEnglish(84 , -posY + 10, "<<<<<<<");
    HAL::setDrawType(2);
    HAL::drawEnglish(boxX, -posY + 10, title);
    HAL::setDrawType(1);

    // 绘制数据框边框
    HAL::drawRFrame(DataboxX, boxY, boxWidth, boxHeight, RADIUS);

    // 绘制硬件信息文本
    const float targetDataX = DataboxX + astraConfig.listTextMargin; // 与列表文本边距一致
    for (size_t i = 0; i < infoData.size(); ++i) {
        // 计算行位置（基于列表行高）
        float targetDataY = boxY + Title_Y_OFFSET + i* DISPLAY_LINE_HEIGHT* 0.8+ _camera[1];

        // 初始化/更新动画目标位置
        if (i >= dataXPos.size()) {
            dataXPos.resize(i + 1, DATA_ANIM_START_X);
            dataXTrg.resize(i + 1, targetDataX);
            dataYPos.resize(i + 1, DATA_ANIM_START_Y);
            dataYTrg.resize(i + 1, targetDataY);
        } else {
            dataXTrg[i] = targetDataX;
            dataYTrg[i] = targetDataY;
        }

        // 执行文本动画
        Animation::move(&dataXPos[i], dataXTrg[i], astraConfig.hardwareInfoAnimationSpeed);
        Animation::move(&dataYPos[i], dataYTrg[i], astraConfig.hardwareInfoAnimationSpeed);

        // 绘制文本（英文）
        HAL::drawEnglish(dataXPos[i], dataYPos[i], infoData[i]);
    }
}





}