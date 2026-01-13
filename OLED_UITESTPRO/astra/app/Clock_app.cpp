#include "Clock_app.h"
#include "HAL.h"



namespace astra {

// 构造函数实现
Clock::Clock() : Clock("Clock", generateDefaultPic()) {

}

Clock::Clock(const std::string& _title) : Clock(_title, generateDefaultPic()) {

}

    // 在Clock的构造函数中添加（例如最完整的那个构造函数）
    Clock::Clock(const std::string& _title, const std::vector<unsigned char>& _pic) {
    this->title = _title;
    this->pic = _pic;
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};

    // 初始化动画参数（原有）
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
    // 新增：初始化各元素动画位置（起始位置在屏幕外）
    static int screenWidth = HAL::getSystemConfig().screenWeight;
    static int screenHeight = HAL::getSystemConfig().screenHeight;

    // 日期（左上角）：从左侧屏幕外进入，目标X=5
    dateXPos = -100;  // 左侧场外
    dateXTrg = 5;

    // 星期（右上角）：从右侧屏幕外进入，目标X=屏幕宽-星期宽度-5
    weekXPos = screenWidth + 100;  // 右侧场外
    weekXTrg = 0;  // 临时值，实际在render中计算

    // Alm（左下角）：从左侧屏幕外进入，目标X=5
    almXPos = -50;  // 左侧场外
    almXTrg = 5;

    // Stw（右下角）：从右侧屏幕外进入，目标X=屏幕宽-Stw宽度-5
    stwXPos = screenWidth + 50;  // 右侧场外
    stwXTrg = 0;  // 临时值，实际在render中计算

    // Setting（下方中间）：从底部屏幕外进入，目标Y=屏幕高-5
    settingYPos = screenHeight + 50;  // 底部场外
    settingYTrg = screenHeight - 5;

    // 初始化时间文本动画参数（原有）
    dataXPos.push_back(DATA_ANIM_START_X);
    dataXTrg.push_back(0);
    dataYPos.push_back(DATA_ANIM_START_Y);
    dataYTrg.push_back(0);

    // 若RTC未就绪，使用预设的time结构体值
    Second = time.tm_sec;
    Minute = time.tm_min;
    Hour = time.tm_hour;
    // 分解时间为各个数字位
    uint8_t initH1 = Hour / 10;
    uint8_t initH2 = Hour % 10;
    uint8_t initM1 = Minute / 10;
    uint8_t initM2 = Minute % 10;
    uint8_t initS1 = Second / 10;
    uint8_t initS2 = Second % 10;

    // 初始化动画对象，当前值和旧值一致（避免触发初始动画）
    h1Anim.currentValue = h1Anim.previousValue = initH1;
    h2Anim.currentValue = h2Anim.previousValue = initH2;
    m1Anim.currentValue = m1Anim.previousValue = initM1;
    m2Anim.currentValue = m2Anim.previousValue = initM2;
    s1Anim.currentValue = s1Anim.previousValue = initS1;
    s2Anim.currentValue = s2Anim.previousValue = initS2;

    // 禁用初始动画状态
    h1Anim.isAnimating = false;
    h2Anim.isAnimating = false;
    m1Anim.isAnimating = false;
    m2Anim.isAnimating = false;
    s1Anim.isAnimating = false;
    s2Anim.isAnimating = false;
}

// 菜单类型标识（原有）
std::string Clock::getType() const {
    return "Clock";
}

// 前景元素初始化（原有）
void Clock::forePosInit() {
    // 空实现，与HardwareInfo保持一致
}

// 重置进入动画（原有，新增滚动变量重置）
void Clock::resetEnterAnimation() {
    static int screenWidth = HAL::getSystemConfig().screenWeight;
    static int screenHeight = HAL::getSystemConfig().screenHeight;
    isEntering = true;
    posY = enterStartY;
    posYTrg = 0;
    dateXPos = -100;
    dateXTrg = 5;
    weekXPos = screenWidth + 100;
    weekXTrg = 0;
    almXPos = -50;
    almXTrg = 5;
    stwXPos = screenWidth + 50;
    stwXTrg = 0;
    settingYPos = screenHeight + 50;
    settingYTrg = screenHeight - 5;
    isActive = false;
    colonBrightness = 0;
    colonDir = 1;

    selectedItemTimer = SETTING;
    selectBoxX = 0;
    selectBoxY = 0;
    selectBoxW = 0;
    selectBoxH = 0;
    selectBoxXTrg = 0;
    selectBoxYTrg = 0;
    selectBoxWTrg = 0;
    selectBoxHTrg = 0;
    // 重置文本动画位置（原有）
    dataXPos[0] = DATA_ANIM_START_X;
    dataYPos[0] = DATA_ANIM_START_Y;


}

// 生成默认图标（原有）
std::vector<unsigned char> Clock::generateDefaultPic() {
    return {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x01, 0xe0, 0xff,
    0x7f, 0x00, 0x80, 0xff, 0x3f, 0xf8, 0x07, 0xff, 0x1f, 0xfe, 0x1f, 0xfe, 0x0f, 0xbf, 0x3f, 0xfc,
    0x87, 0x1f, 0x7f, 0xf8, 0xc7, 0x1f, 0xff, 0xf8, 0xe3, 0x1f, 0xff, 0xf1, 0xe3, 0x1f, 0xff, 0xf1,
    0xf3, 0x1f, 0xff, 0x73, 0xf1, 0x1f, 0xff, 0x63, 0xf1, 0x1f, 0xff, 0x63, 0xf1, 0x1f, 0xc0, 0x63,
    0xf1, 0x1f, 0xc0, 0x63, 0xf1, 0x3f, 0xe0, 0xe3, 0xf3, 0xff, 0xff, 0xe3, 0xe3, 0xff, 0xff, 0xf1,
    0xe3, 0xff, 0xff, 0xf1, 0xc7, 0xff, 0xff, 0xf8, 0xc7, 0xff, 0xff, 0xf8, 0x8f, 0xff, 0x7f, 0xfc,
    0x0f, 0xff, 0x3f, 0xfc, 0x1f, 0xfc, 0x0f, 0xfe, 0x3f, 0xe0, 0x01, 0xff, 0xff, 0x00, 0xc0, 0xff,
    0xff, 0x03, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x3f
    };
}

// 退出状态设置（原有）
void Clock::setIsExiting(bool value) {
    isEntering = false;
    if (value) {
        posYTrg = systemConfig.screenHeight;
    }
}

// 退出动画完成判断（原有）
bool Clock::isExitAnimationFinished() const {
    return (posY == posYTrg) && !isEntering;
}
// 弹窗：从中间展开到全屏效果
void Clock::ClockPopup(std::string _info) {
    HAL::setFont(getUIConfig().mainFont); // 恢复默认字体
    static bool init = false;
    static bool onRender = false;

    // 屏幕尺寸获取
    const int screenWidth = HAL::getSystemConfig().screenWeight;
    const int screenHeight = HAL::getSystemConfig().screenHeight;
    // 退出按钮相关变量
    static const int exitSize = 10; // 退出按钮大小
    static const float exitMargin = 4; // 边距
    int exitX; // 退出按钮X坐标（实时计算）
    int exitY; // 退出按钮Y坐标（实时计算）
    // 底部按钮参数
    static const int arrowSize = 10;      // 三角箭头边长（建议偶数，对称）
    static const int squareSize = 10;     // 中间圆角正方形大小
    static const int btnSize = 12;        // 加减号按钮大小
    static const float btnMargin = 10;    // 按钮间距
    static const float bottomOffset = 3;  // 底部按钮偏移
    // 新增：上下箭头按钮参数（用于滚动切换）
    static const int scrollArrowSize = 8; // 滚动箭头大小
    static const float scrollArrowMargin = 8; // 与文本间距


    static float selectBoxX = 0, selectBoxY = 0;
    static float selectBoxW = 0, selectBoxH = 0;
    static float selectBoxtargetX = 0, selectBoxtargetY = 0;
    static float selectBoxtargetW = 0, selectBoxtargetH = 0;

    // 屏幕中心点坐标
    const float centerX = screenWidth / 2.0f;
    const float centerY = screenHeight / 2.0f;
    // 新增：滚动动画参数
    static float scrollY = 0;         // 当前滚动偏移量
    static float targetScrollY = 0;   // 目标滚动偏移量
    // 设置时间
    ClockSetPupFlag = true;
    SetTime setTime;
    Nowtime = My_RTC_GetTime();
    if (!init) {
        init = true;
        onRender = true;
        // 初始化选择框目标位置（退出按钮）
        selectBoxtargetW = exitSize + 4;
        selectBoxtargetH = exitSize + 4;
        // 重置时间变量为当前时间
        setTime.year = Nowtime->tm_year + 1900;
        setTime.month = Nowtime->tm_mon + 1;
        setTime.day = Nowtime->tm_mday;
        setTime.weekDay = Nowtime->tm_wday;
        setTime.hour = Nowtime->tm_hour;
        setTime.minute = Nowtime->tm_min;
        setTime.second = Nowtime->tm_sec;
        // 初始化滚动参数
        scrollY = 0;
        targetScrollY = 0;
    }

    // 弹窗尺寸动画变量
    static float currentW = 0;
    static float currentH = 0;
    static float targetW = screenWidth;
    static float targetH = screenHeight;


    static const float lineGap = 0;   // 行间距
    const float fontHeight = HAL::getFontHeight();
    const float lineHeight = fontHeight + lineGap; // 总行高（包含间距）

    MyClock->resetEnterAnimation();
    while (onRender) {
        HAL::canvasClear();
        Setting::Screen_Settig();

        // 计算弹窗位置
        float xPop = centerX - currentW / 2.0f;
        float yPop = centerY - currentH / 2.0f;
        float Xtitle = (screenWidth - HAL::getFontWidth(_info)) / 2.0f;
        // 计算退出按钮坐标
        exitX = static_cast<int>(xPop + currentW - exitMargin - exitSize);
        exitY = static_cast<int>(yPop + exitMargin);

        // 底部按钮Y坐标
        float bottomY = yPop + currentH - bottomOffset - arrowSize;

        // 左下角左箭头
        int leftArrowApexX = static_cast<int>(xPop + exitMargin + arrowSize / 2);
        int leftArrowApexY = static_cast<int>(bottomY + arrowSize / 2);

        // 右下角右箭头
        int rightArrowApexX = static_cast<int>(xPop + currentW - exitMargin - arrowSize / 2);
        int rightArrowApexY = static_cast<int>(bottomY + arrowSize / 2);

        // 中间设定按钮
        int setBtnX = static_cast<int>(xPop + (currentW - squareSize) / 2.0f);
        int setBtnY = static_cast<int>(bottomY + (arrowSize - squareSize) / 2);

        // 减号/加号按钮
        int minusBtnX = setBtnX - btnMargin - btnSize;
        int minusBtnY = static_cast<int>(bottomY + (arrowSize - btnSize) / 2);
        int plusBtnX = setBtnX + squareSize + btnMargin;
        int plusBtnY = minusBtnY;

        // 新增：上下箭头按钮位置（文本右侧）
        std::string dateStr =  // 年月日字符串
            (setTime.year < 1000 ? "0" : "") + std::to_string(setTime.year) + "-" +
            (setTime.month < 10 ? "0" : "") + std::to_string(setTime.month) + "-" +
            (setTime.day < 10 ? "0" : "") + std::to_string(setTime.day);
        std::string timeStr =  // 时分秒字符串
            (setTime.hour < 10 ? "0" : "") + std::to_string(setTime.hour) + ":" +
            (setTime.minute < 10 ? "0" : "") + std::to_string(setTime.minute) + ":" +
            (setTime.second < 10 ? "0" : "") + std::to_string(setTime.second);

        // 文本绘制基础位置（垂直居中区域）
        float baseTextY = centerY + fontHeight/2-4 ;
        float textXdata = (screenWidth - HAL::getFontWidth(dateStr)) / 2.0f;
        float textXtime = (screenWidth - HAL::getFontWidth(timeStr)) / 2.0f;
        // 上下箭头坐标（文本右侧）
        int upArrowApexX = static_cast<int>(textXdata+ HAL::getFontWidth(dateStr) + scrollArrowMargin);
        int upArrowApexY = static_cast<int>( centerY + scrollArrowSize-2);   // 上箭头尖端位置
        int downArrowApexX = upArrowApexX;
        int downArrowApexY = static_cast<int>( centerY - scrollArrowSize);  // 下箭头尖端位置
        //  // 上箭头尖端位置

        // 更新选择框目标位置
        switch (selectedItemTimerSetting) {
            case CLOSE_BTN:
                selectBoxtargetX = exitX - 2;
                selectBoxtargetY = exitY - 2;
                selectBoxtargetW = exitSize + 4;
                selectBoxtargetH = exitSize + 4;
                break;
            case LEFT_ARROW:
                selectBoxtargetX = leftArrowApexX - arrowSize / 2 - 2;
                selectBoxtargetY = leftArrowApexY - arrowSize / 2 - 2;
                selectBoxtargetW = arrowSize + 4;
                selectBoxtargetH = arrowSize + 4;
                break;
            case RIGHT_ARROW:
                selectBoxtargetX = rightArrowApexX - arrowSize / 2 - 2;
                selectBoxtargetY = rightArrowApexY - arrowSize / 2 - 2;
                selectBoxtargetW = arrowSize + 4;
                selectBoxtargetH = arrowSize + 4;
                break;
            case SET_BTN:
                selectBoxtargetX = setBtnX - 2;
                selectBoxtargetY = setBtnY - 2;
                selectBoxtargetW = squareSize + 4;
                selectBoxtargetH = squareSize + 4;
                break;
            case MINUS_BTN:
                selectBoxtargetX = minusBtnX - 2;
                selectBoxtargetY = minusBtnY - 2;
                selectBoxtargetW = btnSize + 4;
                selectBoxtargetH = btnSize + 4;
                break;
            case PLUS_BTN:
                selectBoxtargetX = plusBtnX - 2;
                selectBoxtargetY = plusBtnY - 2;
                selectBoxtargetW = btnSize + 4;
                selectBoxtargetH = btnSize + 4;
                break;
            case UP_ARROW:  // 上箭头选择框
                selectBoxtargetX = upArrowApexX - scrollArrowSize / 2 - 2;
                selectBoxtargetY = upArrowApexY - scrollArrowSize / 2 - 2;
                selectBoxtargetW = scrollArrowSize + 4;
                selectBoxtargetH = scrollArrowSize + 4;
                break;
            case DOWN_ARROW:  // 下箭头选择框
                selectBoxtargetX = downArrowApexX - scrollArrowSize / 2 - 2;
                selectBoxtargetY = downArrowApexY - scrollArrowSize / 2 - 2;
                selectBoxtargetW = scrollArrowSize + 4;
                selectBoxtargetH = scrollArrowSize + 4;
                break;
        }

        // 绘制弹窗背景和边框
        HAL::setDrawType(0);
        HAL::drawRBox(xPop, yPop, currentW, currentH, getUIConfig().popRadius);
        HAL::setDrawType(1);
        HAL::drawRFrame(xPop, yPop, currentW, currentH, getUIConfig().popRadius);

        // 绘制内容（尺寸足够时）
        if (currentW > HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin &&
            currentH > HAL::getFontHeight() + 2 * getUIConfig().popMargin) {

            // 绘制标题
            HAL::drawEnglish(Xtitle, yPop + HAL::getFontHeight(), _info);

            // 绘制退出按钮
            HAL::drawRFrame(exitX, exitY, exitSize + 1, exitSize + 1, 2);
            for (int i = 0; i <= exitSize; ++i) {
                HAL::drawPixel(exitX + i, exitY + i);
                HAL::drawPixel(exitX + exitSize - i, exitY + i);
            }

            // 绘制底部箭头和按钮（原有逻辑）
            auto drawPureLeftTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                int halfSize = size / 2;
                for (int y = 0; y < size; y++) {
                    int dy = y - halfSize;
                    int absDy = abs(dy);
                    int pixelCount = halfSize - absDy;
                    if (pixelCount > 0) {
                        for (int x = 0; x < pixelCount; x++) {
                            HAL::drawPixel(apexX - x, apexY - dy);
                        }
                    }
                }
            };
            auto drawPureRightTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                int halfSize = size / 2;
                for (int y = 0; y < size; y++) {
                    int dy = y - halfSize;
                    int absDy = abs(dy);
                    int pixelCount = halfSize - absDy;
                    if (pixelCount > 0) {
                        for (int x = 0; x < pixelCount; x++) {
                            HAL::drawPixel(apexX + x, apexY - dy);
                        }
                    }
                }
            };
            drawPureLeftTriangle(leftArrowApexX, leftArrowApexY, arrowSize);
            drawPureRightTriangle(rightArrowApexX, rightArrowApexY, arrowSize);

            // 新增：绘制上下箭头（用于滚动）
            auto drawPureUpTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);  // 实心填充
                int halfSize = size / 2;
                // 尖端朝上，底边在下
                for (int x = 0; x < size; x++) {
                    int dx = x - halfSize;
                    int absDx = abs(dx);
                    int pixelCount = halfSize - absDx;
                    if (pixelCount > 0) {
                        for (int y = 0; y < pixelCount; y++) {
                            HAL::drawPixel(apexX - dx, apexY + y);  // 从尖端向下绘制
                        }
                    }
                }
            };
            auto drawPureDownTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);  // 实心填充
                int halfSize = size / 2;
                // 尖端朝下，底边在上
                for (int x = 0; x < size; x++) {
                    int dx = x - halfSize;
                    int absDx = abs(dx);
                    int pixelCount = halfSize - absDx;
                    if (pixelCount > 0) {
                        for (int y = 0; y < pixelCount; y++) {
                            HAL::drawPixel(apexX - dx, apexY - y);  // 从尖端向上绘制
                        }
                    }
                }
            };
            drawPureUpTriangle(upArrowApexX, upArrowApexY, scrollArrowSize);    // 上箭头
            drawPureDownTriangle(downArrowApexX, downArrowApexY, scrollArrowSize);  // 下箭头upArrowApexY

            // 绘制中间按钮和加减号（原有逻辑）
            HAL::drawRBox(setBtnX, setBtnY, squareSize, squareSize, getUIConfig().popRadius);
            HAL::drawRFrame(minusBtnX, minusBtnY, btnSize, btnSize, 2);
            HAL::drawHLine(minusBtnX + 2, minusBtnY + btnSize / 2, btnSize - 4);
            HAL::drawRFrame(plusBtnX, plusBtnY, btnSize, btnSize, 2);
            HAL::drawHLine(plusBtnX + 2, plusBtnY + btnSize / 2, btnSize - 4);
            HAL::drawVLine(plusBtnX + btnSize / 2, plusBtnY + 2, btnSize - 4);

            // 新增：分行绘制年月日和时分秒（带滚动效果）
            // 计算两行的实际绘制位置（受滚动偏移影响）
            float dateDrawY = baseTextY - lineHeight/2 + scrollY;  // 上一行（年月日）
            float timeDrawY = baseTextY + lineHeight/2 + scrollY;  // 下一行（时分秒）
            HAL::drawEnglish(textXdata, dateDrawY, dateStr);  // 年月日行
            HAL::drawEnglish(textXtime, timeDrawY, timeStr);  // 时分秒行

            // 绘制选择框
            HAL::setDrawType(2);
            HAL::drawRBox(selectBoxX, selectBoxY, selectBoxW, selectBoxH, 2);
            HAL::setDrawType(1);
        }

        HAL::canvasUpdate();

        // 执行动画
        Animation::move(&currentW, targetW, getUIConfig().popSpeed);
        Animation::move(&currentH, targetH, getUIConfig().popSpeed);
        Animation::move(&selectBoxX, selectBoxtargetX, getUIConfig().popSpeed);
        Animation::move(&selectBoxY, selectBoxtargetY, getUIConfig().popSpeed);
        Animation::move(&selectBoxW, selectBoxtargetW, getUIConfig().popSpeed);
        Animation::move(&selectBoxH, selectBoxtargetH, getUIConfig().popSpeed);
        Animation::move(&scrollY, targetScrollY, getUIConfig().popSpeed);  // 滚动动画

        // 按键处理
        HAL::keyScan();
        if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
            for (unsigned char i = 0; i < key::KEY_NUM; i++) {
                if (HAL::getKeyMap()[i] == key::CLICK) {
                    // 切换选择项（考虑新增的上下箭头）
                    if (i == 1) {  // 右按键：下一个选项
                        selectedItemTimerSetting = static_cast<SelectedItemCLockSetting>(
                            (selectedItemTimerSetting + 1) % 8  // 总数变为8个
                        );
                    }
                    if (i == 0) {  // 左按键：上一个选项
                        selectedItemTimerSetting = static_cast<SelectedItemCLockSetting>(
                            (selectedItemTimerSetting - 1 + 8) % 8
                        );
                    }
                } else if (HAL::getKeyMap()[i] == key::PRESS) {
                    if (i == 1) {
                        switch (selectedItemTimerSetting) {
                            case CLOSE_BTN:
                                targetW = 0;
                                targetH = 0;
                                break;
                            case UP_ARROW:  // 上箭头：滚动到上一行（时分秒居中）
                                targetScrollY = -lineHeight/2;  // 上移半行高
                                break;
                            case DOWN_ARROW:  // 下箭头：滚动到下一行（年月日居中）
                                targetScrollY = lineHeight/2;   // 下移半行高
                                break;
                            // 其他按钮逻辑保持不变
                        }
                    }
                }
            }
            std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
        }

        // 退出条件
        if (currentW <= 8.0f && currentH <= 8.0f) {
            onRender = false;
            init = false;
            currentW = 0;
            currentH = 0;
            targetW = screenWidth;
            targetH = screenHeight;
        }
    }
}
void Clock::updateTime() {
    ClockSetPupFlag=false;

    if (ClocksetFlag == true) {
        My_RTC_SetTime(&time);  // 首次运行时设置RTC时间
        ClocksetFlag = false;
    }



    Nowtime = My_RTC_GetTime();
    if (Nowtime != nullptr&&!ClockSetPupFlag) {
        Hour = Nowtime->tm_hour;
        Minute = Nowtime->tm_min;
        Second = Nowtime->tm_sec;
        // 获取日期信息
         Year = Nowtime->tm_year + 1900;  // tm_year是从1900开始的年数
         Month = Nowtime->tm_mon + 1;     // tm_mon是0-11
         Day = Nowtime->tm_mday ;
         Wday = Nowtime->tm_wday;         // 0-6, 0是周日

        // 格式化时间字符串（原有）
        char timeBuffer[9];
        sprintf(timeBuffer, "%02d:%02d:%02d", Hour, Minute, Second);
        timeStr = timeBuffer;
        // 格式化日期字符串
        char dateBuffer[11];
        sprintf(dateBuffer, "%04d-%02d-%02d", Year, Month, Day);
        dateStr = dateBuffer;
        // 星期英文映射
        const std::vector<std::string> weekdays = {
            "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
        };
        weekStr = weekdays[Wday];
    }

    // 分解时分秒为六位数字（核心）
    uint8_t newH1 = Hour / 10;
    uint8_t newH2 = Hour % 10;
    uint8_t newM1 = Minute / 10;
    uint8_t newM2 = Minute % 10;
    uint8_t newS1 = Second / 10;
    uint8_t newS2 = Second % 10;

    // 获取字体高度（用于限制滚动范围）
    HAL::setFont(u8g2_font_chargen_92_mn);
    float hmDigitHeight = static_cast<float>(HAL::getFontHeight()); // 时分数字高度
    HAL::setFont(u8g2_font_9x6LED_tr);
    float secDigitHeight = static_cast<float>(HAL::getFontHeight()); // 秒数字高度
    HAL::setFont(getUIConfig().mainFont);

    // 动画触发逻辑：数字变化时，保存旧值并初始化动画
    auto triggerScrollAnim = [&](DigitAnimation& anim, uint8_t newValue, float digitHeight) {
        // 仅当数字变化且无动画时触发（避免重复动画）
        if (anim.currentValue != newValue && !anim.isAnimating) {
            anim.previousValue = anim.currentValue; // 保存上一次的旧值
            anim.currentValue = newValue;            // 更新为当前新值
            anim.scrollOffset = 0.0f;                // 重置滚动偏移
            anim.isAnimating = true;                 // 标记动画开始
        }
    };

    // 触发各数字位的滚动动画（传入对应新值和字体高度）
    triggerScrollAnim(h1Anim, newH1, hmDigitHeight);
    triggerScrollAnim(h2Anim, newH2, hmDigitHeight);
    triggerScrollAnim(m1Anim, newM1, hmDigitHeight);
    triggerScrollAnim(m2Anim, newM2, hmDigitHeight);
    triggerScrollAnim(s1Anim, newS1, secDigitHeight);
    triggerScrollAnim(s2Anim, newS2, secDigitHeight);
    // 冒号闪烁逻辑（原有）
    colonBrightness = (Second % 2 == 0) ? 1 : 0;
}


void Clock::render(const std::vector<float>& _camera) {
    Item::updateConfig();
    HAL::canvasClear();
    Setting::Screen_Settig();
    HAL::setDrawType(1);

    const int screenWidth = HAL::getSystemConfig().screenWeight;
    const int screenHeight = HAL::getSystemConfig().screenHeight;

    // 处理进入动画（原有逻辑）
    if (isEntering) {
        Animation::move(&posY, posYTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&dateXPos, dateXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&weekXPos, weekXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&almXPos, almXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&stwXPos, stwXTrg, astraConfig.clokDataAnimationSpeed);
        Animation::move(&settingYPos, settingYTrg, astraConfig.clokDataAnimationSpeed);
        if (posY == posYTrg) {
            isEntering = false;
            isActive = true;
        }
    }

    updateTime();


    // --------------------------
    // 计算各元素目标位置（依赖实时宽度）
    // --------------------------
    HAL::setFont(u8g2_font_9x6LED_tr);  // 使用统一字体计算
    // 星期目标X：屏幕右侧 - 星期宽度 - 5
    weekXTrg = screenWidth - HAL::getFontWidth(weekStr.c_str()) - 5;
    // Stw目标X：屏幕右侧 - Stw宽度 - 5
    stwXTrg = screenWidth - HAL::getFontWidth("Stw") - 5;
    HAL::setFont(u8g2_font_12x6LED_tr);  // 使用统一字体计算
    int settingWidth = HAL::getFontWidth("Setting");
    settingYTrg = screenHeight -5;
    int settingXTrg = (screenWidth - settingWidth) / 2;

    int smallFontHeight = HAL::getFontHeight();

    // 左上角：日期（从左侧插入）
    HAL::setFont(u8g2_font_9x6LED_tr);
    HAL::drawEnglish(dateXPos, smallFontHeight, dateStr);
    // 右上角：星期（从右侧插入）
    HAL::drawEnglish(weekXPos, smallFontHeight, weekStr);
    // 左下角：Alm（从左侧插入）
    HAL::drawEnglish(almXPos, screenHeight - 5, "Alm");
    // 下方中间：Setting（从下方弹入）
    HAL::setFont(u8g2_font_12x6LED_tr);
    HAL::drawEnglish(settingXTrg, settingYPos, "Setting");

    // 右下角：Stw（从右侧插入）
    HAL::setFont(u8g2_font_9x6LED_tr);
    HAL::drawEnglish(stwXPos, screenHeight - 5, "Stw");

    // --------------------------
    // 字体配置（时分和秒分别配置）
    // --------------------------
    HAL::setFont(u8g2_font_chargen_92_mn);
    const int hmDigitWidth = HAL::getFontWidth("0");
    const float hmDigitHeight = static_cast<float>(HAL::getFontHeight()); // 浮点型，用于动画计算
    const int colonWidth = HAL::getFontWidth(":");

    HAL::setFont(u8g2_font_9x6LED_tr);
    const int secDigitWidth = HAL::getFontWidth("0");
    const float secDigitHeight = static_cast<float>(HAL::getFontHeight());
    HAL::setFont(u8g2_font_chargen_92_mn); // 恢复时分字体

    // --------------------------
    // 屏幕与位置计算（居中显示）
    // --------------------------


    // 定义间距（统一管理，方便调整）
    const int spacingColon = colonWidth/1.5; // 冒号与数字的间距
    const int spacingSec = 1;                  // 分钟与秒的间距

    // 计算整体总宽度（含所有元素和间距）
    int totalWidth =
        4 * hmDigitWidth +          // H1/H2/M1/M2（4个时分数字）
        2 * spacingColon +          // 冒号两侧的间距
        2 * secDigitWidth +         // S1/S2（2个秒数字）
        spacingSec-hmDigitWidth ;                // 分钟到秒的间距


    // 水平居中起始X，垂直居中起始Y（以时分字体高度为基准）
    int startX = (screenWidth - totalWidth) / 2;
    int startY = (screenHeight - static_cast<int>(hmDigitHeight)) / 2;

    // 应用整体动画位移（原有逻辑）
    float animX = startX + dataXPos[0];
    float animY = startY + dataYPos[0] + posY;

    // --------------------------
    // 滚动动画更新：限制偏移在数字高度内
    // --------------------------
    auto updateScrollAnim = [&](DigitAnimation& anim, float speed, float maxOffset) {
        if (anim.isAnimating) {
            // 滚动偏移递增（从0 → maxOffset，maxOffset=数字高度）
            Animation::move(&anim.scrollOffset, maxOffset, speed);
            // 动画结束条件：偏移达到最大高度（浮点精度处理，避免死循环）
            if (anim.scrollOffset >= maxOffset - 0.1f) {
                anim.isAnimating = false;
                anim.scrollOffset = 0.0f; // 重置偏移
            }
        }
    };

    // 更新所有数字位的滚动动画（传入对应字体高度）
    updateScrollAnim(h1Anim, astraConfig.clokDataAnimationSpeed, hmDigitHeight);
    updateScrollAnim(h2Anim, astraConfig.clokDataAnimationSpeed, hmDigitHeight);
    updateScrollAnim(m1Anim, astraConfig.clokDataAnimationSpeed, hmDigitHeight);
    updateScrollAnim(m2Anim, astraConfig.clokDataAnimationSpeed, hmDigitHeight);
    updateScrollAnim(s1Anim, astraConfig.clokDataAnimationSpeed, secDigitHeight);
    updateScrollAnim(s2Anim, astraConfig.clokDataAnimationSpeed, secDigitHeight);

    // --------------------------
    // 封装绘制函数：处理旧值和新值的滚动绘制
    // --------------------------
    auto drawHmDigit = [&](int x, float baseY, DigitAnimation& anim) {

        // 计算裁剪区域：覆盖整个时分数字的绘制范围
        // 起始Y = 基线Y - 字体高度（字体顶部）
        // 高度 = 时分字体高度
        int clipX = x;
        int clipY = static_cast<int>(baseY - hmDigitHeight+1);
        int clipW = hmDigitWidth;
        int clipH = static_cast<int>(hmDigitHeight);


        HAL::setClipRect(clipX, clipY, clipW, clipH); // 开启裁剪

        if (anim.isAnimating) {
            // 旧值：向上滚动，超出裁剪区域的部分会被截去
            float oldDigitY = baseY - anim.scrollOffset;
            HAL::drawEnglish(x, oldDigitY, std::string(1, '0' + anim.previousValue));

            // 新值：从下方进入，仅显示裁剪区域内的部分
            float newDigitY = baseY + (hmDigitHeight - anim.scrollOffset);
            HAL::drawEnglish(x, newDigitY, std::string(1, '0' + anim.currentValue));
        } else {
            // 无动画时，直接绘制（完全在裁剪区域内）
            HAL::drawEnglish(x, baseY, std::string(1, '0' + anim.currentValue));
        }

       HAL::resetClipRect(); // 关闭裁剪，避免影响其他元素
    };

    auto drawSecDigit = [&](int x, float baseY, DigitAnimation& anim, float secOffset) {
        float secBaseY = baseY + secOffset; // 秒数字基线

        // 计算裁剪区域：覆盖整个秒数字的绘制范围
        int clipX = x;
        int clipY = static_cast<int>(secBaseY - secDigitHeight+1); // 秒字体顶部
        int clipW = secDigitWidth;
        int clipH = static_cast<int>(secDigitHeight);

        HAL::setClipRect(clipX, clipY, clipW, clipH); // 开启裁剪

        if (anim.isAnimating) {
            // 旧值：向上滚动，超出部分被截去
            float oldDigitY = secBaseY - anim.scrollOffset;
            HAL::drawEnglish(x, oldDigitY, std::string(1, '0' + anim.previousValue));

            // 新值：从下方进入，仅显示裁剪区域内的部分
            float newDigitY = secBaseY + (secDigitHeight - anim.scrollOffset);
            HAL::drawEnglish(x, newDigitY, std::string(1, '0' + anim.currentValue));
        } else {
            // 无动画时，完全在裁剪区域内绘制
            HAL::drawEnglish(x, secBaseY, std::string(1, '0' + anim.currentValue));
        }

        HAL::resetClipRect(); // 关闭裁剪
    };
    // --------------------------
    // 逐个绘制元素
    // --------------------------
    int currentX = static_cast<int>(animX);
    float hmBaseY = animY + hmDigitHeight; // 时分数字的基线（统一对齐核心）
    float secYOffset = 0; // 秒数字的基线偏移（与时分对齐）

    // 1. 小时十位（H1）
    drawHmDigit(currentX, hmBaseY, h1Anim);
    currentX += hmDigitWidth;

    // 2. 小时个位（H2）
    drawHmDigit(currentX, hmBaseY, h2Anim);
    currentX += spacingColon;

    // 3. 第一个冒号（时和分之间）
    if (colonBrightness == 1) {
        HAL::drawEnglish(currentX, hmBaseY, ":");
    }

    currentX += spacingColon;

    // 分钟十位（M1）
    drawHmDigit(currentX, hmBaseY, m1Anim);
    currentX += hmDigitWidth;

    // 分钟个位（M2）
    drawHmDigit(currentX, hmBaseY, m2Anim);
    currentX += hmDigitWidth + spacingSec; // 分钟到秒的间距

    // 秒钟十位（S1）：切换到秒字体
    HAL::setFont(u8g2_font_9x6LED_tr);
    drawSecDigit(currentX, hmBaseY, s1Anim, secYOffset);
    currentX += secDigitWidth + spacingSec;

    // 秒钟个位（S2）
    drawSecDigit(currentX, hmBaseY, s2Anim, secYOffset);


    // 计算各元素尺寸（用于选择框定位）
    HAL::setFont(u8g2_font_9x6LED_tr);
    static float almWidth = HAL::getFontWidth("Alm");
    static float stwWidth = HAL::getFontWidth("Stw");
    // 补充Setting的X坐标计算（居中）
    HAL::setFont(u8g2_font_12x6LED_tr);
    static float settingWidth_box = HAL::getFontWidth("Setting");
    static float settingXTrg_box = (screenWidth - settingWidth) / 2;

    // 确保Alm和Stw的Y坐标统一
    const int bottomY = screenHeight - 5;  // 底部边距5px

    // 计算选择框目标位置
    switch (selectedItemTimer) {
        case ALM:
            selectBoxXTrg = almXPos - 2;
            selectBoxYTrg = screenHeight - smallFontHeight - 2;  // Alm的Y坐标
            selectBoxWTrg = almWidth + 4;
            selectBoxHTrg = smallFontHeight + 4;
            break;
        case STW:
            selectBoxXTrg = stwXPos - 2;
            selectBoxYTrg = screenHeight - smallFontHeight - 2;  // Stw的Y坐标
            selectBoxWTrg = stwWidth + 4;
            selectBoxHTrg = smallFontHeight + 4;
            break;
        case SETTING:
            selectBoxXTrg = settingXTrg_box - 2;
            selectBoxYTrg = screenHeight - smallFontHeight - 2;  // Setting的Y坐标
            selectBoxWTrg = settingWidth_box + 4;
            selectBoxHTrg = smallFontHeight + 4;
            break;
    }


    Animation::move(&selectBoxX, selectBoxXTrg, 90);
    Animation::move(&selectBoxY, selectBoxYTrg, 90);
    Animation::move(&selectBoxW, selectBoxWTrg, 90);
    Animation::move(&selectBoxH, selectBoxHTrg,90);

    HAL::setDrawType(2);  // 反色绘制
    HAL::drawRBox(selectBoxX, selectBoxY, selectBoxW, selectBoxH, 2);
    HAL::setDrawType(1);  // 恢复正常绘制
    // 按键处理
    HAL::keyScan();
    if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
        *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;

        for (unsigned char i = 0; i < key::KEY_NUM; i++) {
            if (HAL::getKeyMap()[i] == key::CLICK) {
                // 左键（假设i=0为左键）
                if (i == 0) {
                    selectedItemTimer = static_cast<SelectedItemClock>((selectedItemTimer - 1 + 3) % 3);
                }
                // 右键（假设i=1为右键）
                if (i == 1) {
                    selectedItemTimer = static_cast<SelectedItemClock>((selectedItemTimer + 1 +3) % 3);
                }
            }
        }
        std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
        *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
    }

    // --------------------------
    // 原有动画更新与画布刷新
    // --------------------------
    Animation::move(&dataXPos[0], dataXTrg[0], astraConfig.clokDataAnimationSpeed);
    Animation::move(&dataYPos[0], dataYTrg[0], astraConfig.clokDataAnimationSpeed);

    HAL::canvasUpdate();
    HAL::setFont(getUIConfig().mainFont); // 恢复默认字体
}
}  // namespace astra