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

// 退出动画完成判断
bool Clock::isExitAnimationFinished() const {
    return (posY == posYTrg) && !isEntering;
}

    // 辅助函数：计算某年某月的天数
int daysInMonth(int year, int month) {
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2) {
        // 闰年判断
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        return isLeapYear ? 29 : 28;
    }

    if (month >= 1 && month <= 12) {
        return daysInMonth[month - 1];
    }

    return 31; // 默认返回31天
}
// 弹窗：从中间展开到全屏效果
void Clock::ClockPopup(std::string _info) {

    HAL::setFont(getUIConfig().mainFont);
    static bool init = false;
    static bool onRender = false;

    const int screenWidth = HAL::getSystemConfig().screenWeight;
    const int screenHeight = HAL::getSystemConfig().screenHeight;
    static const int exitSize = 10;
    static const float exitMargin = 4;
    int exitX;
    int exitY;
    static const int arrowSize = 10;
    static const int squareSize = 10;
    static const int btnSize = 10;
    static const float btnMargin = 10;
    static const float bottomOffset = 3;
    static const int scrollArrowSize = 8;
    static const float scrollArrowMargin = 8;

    static float selectBoxX = 0, selectBoxY = 0;
    static float selectBoxW = 0, selectBoxH = 0;
    static float selectBoxtargetX = 0, selectBoxtargetY = 0;
    static float selectBoxtargetW = 0, selectBoxtargetH = 0;

    // ========== 新增：日期和时间选择框变量（带动画目标）==========
    static float timedateSelectBoxX = 0, timedateSelectBoxY = 0;
    static float timedateSelectBoxW = 0, timedateSelectBoxH = 0;
    static float timedateSelectBoxtargetX = 0, timedateSelectBoxtargetY = 0;
    static float timedateSelectBoxtargetW = 0, timedateSelectBoxtargetH = 0;


    // ========== 新增：日期和时间选择状态变量 ==========
    static SelectedItemCLockDateSetting selectedDatePart = DateYear;
    static SelectedItemCLockTimeSetting selectedTimePart = TimeHour;
    static bool dateSelected = false;  // 日期是否被选中
    static bool timeSelected = false;  // 时间是否被选中

    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;

    // ========== 老虎机滚动系统 ==========
    static int datePos = -1;
    static int timePos = 0;
    static float dateOffset = 0;
    static float timeOffset = 0;
    static float targetDateOffset = 0;
    static float targetTimeOffset = 0;
    static bool isAnimating = false;
    static bool scrollUpDirection = true;

    // 设置时间
    ClockSetPupFlag = true;
    SetTime setTime;
    Nowtime = My_RTC_GetTime();
    if (!init) {
        init = true;
        onRender = true;
        selectBoxtargetW = exitSize + 4;
        selectBoxtargetH = exitSize + 4;
        setTime.year = Nowtime->tm_year + 1900;
        setTime.month = Nowtime->tm_mon + 1;
        setTime.day = Nowtime->tm_mday;
        setTime.weekDay = Nowtime->tm_wday;
        setTime.hour = Nowtime->tm_hour;
        setTime.minute = Nowtime->tm_min;
        setTime.second = Nowtime->tm_sec;

        // 初始化滚动状态
        datePos = -1;
        timePos = 0;
        dateOffset = 0;
        timeOffset = 0;
        targetDateOffset = 0;
        targetTimeOffset = 0;
        isAnimating = false;

        // 初始化选择状态
        selectedDatePart = DateYear;
        selectedTimePart = TimeHour;
        dateSelected = (datePos == 0);  // 日期在中间时选中
        timeSelected = (timePos == 0);  // 时间在中间时选中

        // 初始化日期和时间选择框目标位置
        timedateSelectBoxtargetW = 0;
        timedateSelectBoxtargetH = 0;
    }

    static float currentW = 0;
    static float currentH = 0;
    static float targetW = screenWidth;
    static float targetH = screenHeight;

    static const float lineGap = 0;
    const float fontHeight = HAL::getFontHeight();
    const float lineHeight = fontHeight + lineGap;



    MyClock->resetEnterAnimation();

    while (onRender) {
        HAL::canvasClear();
        Setting::Screen_Settig();

        const float xPop = centerX - currentW / 2;
        const float yPop = centerY - currentH / 2;
        const float Xtitle = (screenWidth - HAL::getFontWidth(_info)) / 2;
        exitX = static_cast<int>(xPop + currentW - exitMargin - exitSize);
        exitY = static_cast<int>(yPop + exitMargin);

        const float bottomY = yPop + currentH - bottomOffset - arrowSize;

        const int leftArrowApexX = static_cast<int>(xPop + exitMargin + arrowSize / 2);
        const int leftArrowApexY = static_cast<int>(bottomY + arrowSize / 2);

        const int rightArrowApexX = static_cast<int>(xPop + currentW - exitMargin - arrowSize / 2);
        const int rightArrowApexY = static_cast<int>(bottomY + arrowSize / 2);

        const int setBtnX = static_cast<int>(xPop + (currentW - squareSize) / 2);
        const int setBtnY = static_cast<int>(bottomY + (arrowSize - squareSize) / 2);

        const int minusBtnX = setBtnX - btnMargin - btnSize;
        const int minusBtnY = static_cast<int>(bottomY + (arrowSize - btnSize) / 2);
        const int plusBtnX = setBtnX + squareSize + btnMargin;
        const int plusBtnY = minusBtnY;

        const std::string dateStr =
            (setTime.year < 1000 ? "0" : "") + std::to_string(setTime.year) + "-" +
            (setTime.month < 10 ? "0" : "") + std::to_string(setTime.month) + "-" +
            (setTime.day < 10 ? "0" : "") + std::to_string(setTime.day);
        const std::string timeStr =
            (setTime.hour < 10 ? "0" : "") + std::to_string(setTime.hour) + " : " +
            (setTime.minute < 10 ? "0" : "") + std::to_string(setTime.minute) + " : " +
            (setTime.second < 10 ? "0" : "") + std::to_string(setTime.second);

        const float baseTextY = centerY + fontHeight/2 - 4;
        const float textXdata = (screenWidth - HAL::getFontWidth(dateStr)) / 2;
        const float textXtime = (screenWidth - HAL::getFontWidth(timeStr)) / 2;
        const int upArrowApexX = static_cast<int>(textXdata + HAL::getFontWidth(dateStr) + scrollArrowMargin);
        const int upArrowApexY = static_cast<int>(centerY + scrollArrowSize - 2);
        const int downArrowApexX = upArrowApexX;
        const int downArrowApexY = static_cast<int>(centerY - scrollArrowSize);

        // ========== 计算日期和时间各部分的位置 ==========
        std::string yearStr = (setTime.year < 1000 ? "0" : "") + std::to_string(setTime.year);
        std::string monthStr = (setTime.month < 10 ? "0" : "") + std::to_string(setTime.month);
        std::string dayStr = (setTime.day < 10 ? "0" : "") + std::to_string(setTime.day);

        std::string hourStr = (setTime.hour < 10 ? "0" : "") + std::to_string(setTime.hour);
        std::string minuteStr = (setTime.minute < 10 ? "0" : "") + std::to_string(setTime.minute);
        std::string secondStr = (setTime.second < 10 ? "0" : "") + std::to_string(setTime.second);

        int yearWidth = HAL::getFontWidth(yearStr);
        int monthWidth = HAL::getFontWidth(monthStr);
        int dayWidth = HAL::getFontWidth(dayStr);
        int hyphenWidth = HAL::getFontWidth("-");

        int hourWidth = HAL::getFontWidth(hourStr);
        int minuteWidth = HAL::getFontWidth(minuteStr);
        int secondWidth = HAL::getFontWidth(secondStr);
        int colonWidth = HAL::getFontWidth(":");

        float dateDrawY = baseTextY + (datePos * fontHeight) + dateOffset;
        float timeDrawY = baseTextY + (timePos * fontHeight) + timeOffset;

        // 计算日期各部分起始X位置
        float dateStartX = textXdata;
        float yearX = dateStartX;
        float monthX = yearX + yearWidth + hyphenWidth;
        float dayX = monthX + monthWidth + hyphenWidth;

        // 计算时间各部分起始X位置
        float timeStartX = textXtime;
        float hourX = timeStartX;
        float minuteX = hourX + hourWidth + colonWidth;
        float secondX = minuteX + minuteWidth + colonWidth;

        // ========== 更新日期和时间选择框目标位置 ==========
        if (dateSelected) {
            // 更新日期选择框目标位置（加上垂直偏移避免太近）
            switch (selectedDatePart) {
                case DateYear:
                    timedateSelectBoxtargetX = yearX - 2;
                    timedateSelectBoxtargetW = yearWidth + 4;
                    break;
                case DateMonth:
                    timedateSelectBoxtargetX = monthX + 3 - 2;
                    timedateSelectBoxtargetW = monthWidth + 4;
                    break;
                case DataDay:
                    timedateSelectBoxtargetX = dayX + 4 - 2;
                    timedateSelectBoxtargetW = dayWidth + 4;
                    break;
            }
            timedateSelectBoxtargetY = dateDrawY - fontHeight; // 加上垂直偏移
            timedateSelectBoxtargetH = fontHeight + 2;
        }
        if (timeSelected) {
            // 更新时间选择框目标位置（加上垂直偏移避免太近）
            switch (selectedTimePart) {
                case TimeHour:
                    timedateSelectBoxtargetX = hourX - 2;
                    timedateSelectBoxtargetW = hourWidth + 4;
                    break;
                case TimeMinute:
                    timedateSelectBoxtargetX = minuteX + 14 - 2;
                    timedateSelectBoxtargetW = minuteWidth + 4;
                    break;
                case TimeSecond:
                    timedateSelectBoxtargetX = secondX + 28 - 2;
                    timedateSelectBoxtargetW = secondWidth + 4;
                    break;
            }
            timedateSelectBoxtargetY = timeDrawY  -fontHeight; // 加上垂直偏移
            timedateSelectBoxtargetH = fontHeight + 2;
        }

        // 更新按钮选择框目标位置（保持原有逻辑）
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
            case UP_ARROW:
                selectBoxtargetX = upArrowApexX - scrollArrowSize / 2 - 2;
                selectBoxtargetY = upArrowApexY - scrollArrowSize / 2 - 2;
                selectBoxtargetW = scrollArrowSize + 4;
                selectBoxtargetH = scrollArrowSize + 4;
                break;
            case DOWN_ARROW:
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

        if (currentW > HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin &&
            currentH > HAL::getFontHeight() + 2 * getUIConfig().popMargin) {

            // 绘制标题
            HAL::drawEnglish(Xtitle, yPop + HAL::getFontHeight() - 3, _info);

            // 绘制退出按钮
            HAL::drawRFrame(exitX, exitY, exitSize + 1, exitSize + 1, 2);
            for (int i = 0; i <= exitSize; ++i) {
                HAL::drawPixel(exitX + i, exitY + i);
                HAL::drawPixel(exitX + exitSize - i, exitY + i);
            }

            // 绘制底部箭头和按钮
            auto drawPureLeftTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                const int halfSize = size / 2;
                for (int y = 0; y < size; y++) {
                    const int dy = y - halfSize;
                    const int absDy = abs(dy);
                    const int pixelCount = halfSize - absDy;
                    if (pixelCount > 0) {
                        for (int x = 0; x < pixelCount; x++) {
                            HAL::drawPixel(apexX - x, apexY - dy);
                        }
                    }
                }
            };
            auto drawPureRightTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                const int halfSize = size / 2;
                for (int y = 0; y < size; y++) {
                    const int dy = y - halfSize;
                    const int absDy = abs(dy);
                    const int pixelCount = halfSize - absDy;
                    if (pixelCount > 0) {
                        for (int x = 0; x < pixelCount; x++) {
                            HAL::drawPixel(apexX + x, apexY - dy);
                        }
                    }
                }
            };
            drawPureLeftTriangle(leftArrowApexX, leftArrowApexY, arrowSize);
            drawPureRightTriangle(rightArrowApexX, rightArrowApexY, arrowSize);

            // 绘制上下箭头
            auto drawPureUpTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                const int halfSize = size / 2;
                for (int x = 0; x < size; x++) {
                    const int dx = x - halfSize;
                    const int absDx = abs(dx);
                    const int pixelCount = halfSize - absDx;
                    if (pixelCount > 0) {
                        for (int y = 0; y < pixelCount; y++) {
                            HAL::drawPixel(apexX - dx, apexY + y);
                        }
                    }
                }
            };

            auto drawPureDownTriangle = [&](int apexX, int apexY, int size) {
                HAL::setDrawType(1);
                const int halfSize = size / 2;
                for (int x = 0; x < size; x++) {
                    const int dx = x - halfSize;
                    const int absDx = abs(dx);
                    const int pixelCount = halfSize - absDx;
                    if (pixelCount > 0) {
                        for (int y = 0; y < pixelCount; y++) {
                            HAL::drawPixel(apexX - dx, apexY - y);
                        }
                    }
                }
            };
            drawPureUpTriangle(upArrowApexX, upArrowApexY, scrollArrowSize);
            drawPureDownTriangle(downArrowApexX, downArrowApexY, scrollArrowSize);

            // 绘制中间按钮和加减号
            HAL::drawRBox(setBtnX, setBtnY, squareSize, squareSize, getUIConfig().popRadius);
            HAL::drawRFrame(minusBtnX, minusBtnY, btnSize, btnSize, 2);
            HAL::drawHLine(minusBtnX + 2, minusBtnY + btnSize / 2, btnSize - 4);
            HAL::drawRFrame(plusBtnX, plusBtnY, btnSize, btnSize, 2);
            HAL::drawHLine(plusBtnX + 2, plusBtnY + btnSize / 2, btnSize - 4);
            HAL::drawVLine(plusBtnX + btnSize / 2, plusBtnY + 2, btnSize - 4);

            // ========== 老虎机滚动绘制逻辑 ==========

            // 应用裁剪区域绘制文本
            HAL::setClipRect(textXdata-2, 2*fontHeight+1, 66, fontHeight);
            HAL::drawEnglish(textXdata, dateDrawY, dateStr);
            HAL::drawEnglish(textXtime, timeDrawY, timeStr);
            HAL::resetClipRect();

            HAL::setClipRect(textXdata-2, 4*fontHeight+1, 66, fontHeight);
            HAL::drawEnglish(textXdata, dateDrawY, dateStr);
            HAL::drawEnglish(textXtime, timeDrawY, timeStr);
            HAL::resetClipRect();

            // ========== 绘制日期和时间选择框 ==========
            HAL::setDrawType(2);  // 反色绘制


            HAL::drawRBox(timedateSelectBoxX, timedateSelectBoxY,timedateSelectBoxW, timedateSelectBoxH, 2);


            HAL::setDrawType(1);  // 恢复正常绘制

            // 绘制按钮选择框
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
        Animation::move(&dateOffset, targetDateOffset, getUIConfig().popSpeed);
        Animation::move(&timeOffset, targetTimeOffset, getUIConfig().popSpeed);

        // ========== 新增：日期和时间选择框动画 ==========
        Animation::move(&timedateSelectBoxX, timedateSelectBoxtargetX, 90);
        Animation::move(&timedateSelectBoxY, timedateSelectBoxtargetY, 90);
        Animation::move(&timedateSelectBoxW, timedateSelectBoxtargetW, 90);
        Animation::move(&timedateSelectBoxH, timedateSelectBoxtargetH, 90);


        // ========== 动画完成逻辑 ==========
        if (fabs(dateOffset - targetDateOffset) < 0.5f &&
            fabs(timeOffset - targetTimeOffset) < 0.5f &&
            isAnimating) {
            isAnimating = false;

            // 动画完成后，更新位置状态
            if (scrollUpDirection) {
                datePos = (datePos == 1) ? 0 : ((datePos == 0) ? -1 : 1);
                timePos = (timePos == 1) ? 0 : ((timePos == 0) ? -1 : 1);
            } else {
                datePos = (datePos == -1) ? 0 : ((datePos == 0) ? 1 : -1);
                timePos = (timePos == -1) ? 0 : ((timePos == 0) ? 1 : -1);
            }

            // 重置偏移
            dateOffset = 0;
            timeOffset = 0;
            targetDateOffset = 0;
            targetTimeOffset = 0;

            // 更新日期和时间选中状态（显示在中间的被选中）
            dateSelected = (datePos == 0);
            timeSelected = (timePos == 0);
        }

// ========== 按键处理 ==========
HAL::keyScan();
if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
    *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
    for (unsigned char i = 0; i < key::KEY_NUM; i++) {
        if (HAL::getKeyMap()[i] == key::CLICK) {
            if (i == 1) {
                selectedItemTimerSetting = static_cast<SelectedItemCLockSetting>(
                    (selectedItemTimerSetting + 1) % 8
                );

            }
            if (i == 0) {
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
                    case UP_ARROW:
                        if (!isAnimating) {
                            isAnimating = true;
                            scrollUpDirection = true;

                            // 切换日期和时间的选择状态
                            if (datePos == 0) {  // 日期在中间
                                // 将日期移到上方，时间移到中间
                                dateSelected = false;
                                timeSelected = true;

                                // 重置时间选择部分为小时
                                selectedTimePart = TimeHour;

                                if (timePos != 1) {
                                    timePos = 1;
                                    timeOffset = 0;
                                }
                                targetDateOffset = -fontHeight;
                                targetTimeOffset = -fontHeight;
                            } else if (timePos == 0) {  // 时间在中间
                                // 将时间移到上方，日期移到中间
                                timeSelected = false;
                                dateSelected = true;

                                // 重置日期选择部分为年
                                selectedDatePart = DateYear;

                                if (datePos != 1) {
                                    datePos = 1;
                                    dateOffset = 0;
                                }
                                targetDateOffset = -fontHeight;
                                targetTimeOffset = -fontHeight;
                            }
                        }
                        break;
                    case DOWN_ARROW:
                        if (!isAnimating) {
                            isAnimating = true;
                            scrollUpDirection = false;

                            // 切换日期和时间的选择状态
                            if (datePos == 0) {  // 日期在中间
                                // 将日期移到下方，时间移到中间
                                dateSelected = false;
                                timeSelected = true;

                                // 重置时间选择部分为小时
                                selectedTimePart = TimeHour;

                                if (timePos != -1) {
                                    timePos = -1;
                                    timeOffset = 0;
                                }
                                targetDateOffset = fontHeight;
                                targetTimeOffset = fontHeight;
                            } else if (timePos == 0) {  // 时间在中间
                                // 将时间移到下方，日期移到中间
                                timeSelected = false;
                                dateSelected = true;

                                // 重置日期选择部分为年
                                selectedDatePart = DateYear;

                                if (datePos != -1) {
                                    datePos = -1;
                                    dateOffset = 0;
                                }
                                targetDateOffset = fontHeight;
                                targetTimeOffset = fontHeight;
                            }
                        }
                        break;
                    case LEFT_ARROW:  // 左箭头长按：选择框左移
                        // 日期选择框左移
                        if (dateSelected) {
                            selectedDatePart = static_cast<SelectedItemCLockDateSetting>(
                                (selectedDatePart - 1 + 3) % 3
                            );
                        }
                        // 时间选择框左移
                        if (timeSelected) {
                            selectedTimePart = static_cast<SelectedItemCLockTimeSetting>(
                                (selectedTimePart - 1 + 3) % 3
                            );
                        }
                        break;
                    case RIGHT_ARROW:  // 右箭头长按：选择框右移
                        // 日期选择框右移
                        if (dateSelected) {
                            selectedDatePart = static_cast<SelectedItemCLockDateSetting>(
                                (selectedDatePart + 1) % 3
                            );
                        }
                        // 时间选择框右移
                        if (timeSelected) {
                            selectedTimePart = static_cast<SelectedItemCLockTimeSetting>(
                                (selectedTimePart + 1) % 3
                            );
                        }
                        break;
                    case MINUS_BTN:  // 减号长按：减少值
                        if (dateSelected) {
                            switch (selectedDatePart) {
                                case DateYear:
                                    setTime.year = std::max(setTime.year - 1, 2000);
                                    break;
                                case DateMonth:
                                    setTime.month = (setTime.month > 1) ? setTime.month - 1 : 12;
                                    // 调整天数
                                    if (setTime.day > daysInMonth(setTime.year, setTime.month)) {
                                        setTime.day = daysInMonth(setTime.year, setTime.month);
                                    }
                                    break;
                                case DataDay:
                                    setTime.day = (setTime.day > 1) ? setTime.day - 1 :
                                                  daysInMonth(setTime.year, setTime.month);
                                    break;
                            }
                        }
                        if (timeSelected) {
                            switch (selectedTimePart) {
                                case TimeHour:
                                    setTime.hour = (setTime.hour > 0) ? setTime.hour - 1 : 23;
                                    break;
                                case TimeMinute:
                                    setTime.minute = (setTime.minute > 0) ? setTime.minute - 1 : 59;
                                    break;
                                case TimeSecond:
                                    setTime.second = (setTime.second > 0) ? setTime.second - 1 : 59;
                                    break;
                            }
                        }
                        break;
                    case PLUS_BTN:  // 加号长按：增加值
                        if (dateSelected) {
                            switch (selectedDatePart) {
                                case DateYear:
                                    setTime.year = std::min(setTime.year + 1, 2099);
                                    break;
                                case DateMonth:
                                    setTime.month = (setTime.month < 12) ? setTime.month + 1 : 1;
                                    // 调整天数
                                    if (setTime.day > daysInMonth(setTime.year, setTime.month)) {
                                        setTime.day = daysInMonth(setTime.year, setTime.month);
                                    }
                                    break;
                                case DataDay:
                                    setTime.day = (setTime.day < daysInMonth(setTime.year, setTime.month)) ?
                                                  setTime.day + 1 : 1;
                                    break;
                            }
                        }
                        if (timeSelected) {
                            switch (selectedTimePart) {
                                case TimeHour:
                                    setTime.hour = (setTime.hour < 23) ? setTime.hour + 1 : 0;
                                    break;
                                case TimeMinute:
                                    setTime.minute = (setTime.minute < 59) ? setTime.minute + 1 : 0;
                                    break;
                                case TimeSecond:
                                    setTime.second = (setTime.second < 59) ? setTime.second + 1 : 0;
                                    break;
                            }
                        }
                        break;
                        case  SET_BTN:
                        struct tm newTime;
                        newTime.tm_year = setTime.year - 1900;
                        newTime.tm_mon = setTime.month - 1;
                        newTime.tm_mday = setTime.day;
                        newTime.tm_hour = setTime.hour;
                        newTime.tm_min = setTime.minute;
                        newTime.tm_sec = setTime.second;
                        newTime.tm_wday = setTime.weekDay;
                        newTime.tm_yday = 0;
                        newTime.tm_isdst = 0;
                        if (_info=="Timesetting") {
                            My_RTC_SetTime(&newTime);
                        }
                        else if(_info=="ALMsetting") {
                            My_RTC_SetOneTimeAlarm(&newTime);
                        }

                        // 退出弹窗
                        targetW = 0;
                        targetH = 0;
                        break;
                }
            }
        }else if (HAL::getKeyMap()[i] == key::HOLD) {
            if (i == 0) {
                switch (selectedItemTimerSetting) {
                case MINUS_BTN:  // 减号长按：减少值
                        if (dateSelected) {
                            switch (selectedDatePart) {
                                case DateYear:
                                    setTime.year = std::max(setTime.year - 1, 2000);
                                    break;
                                case DateMonth:
                                    setTime.month = (setTime.month > 1) ? setTime.month - 1 : 12;
                                    // 调整天数
                                    if (setTime.day > daysInMonth(setTime.year, setTime.month)) {
                                        setTime.day = daysInMonth(setTime.year, setTime.month);
                                    }
                                    break;
                                case DataDay:
                                    setTime.day = (setTime.day > 1) ? setTime.day - 1 :
                                                  daysInMonth(setTime.year, setTime.month);
                                    break;
                            }
                        }
                        if (timeSelected) {
                            switch (selectedTimePart) {
                                case TimeHour:
                                    setTime.hour = (setTime.hour > 0) ? setTime.hour - 1 : 23;
                                    break;
                                case TimeMinute:
                                    setTime.minute = (setTime.minute > 0) ? setTime.minute - 1 : 59;
                                    break;
                                case TimeSecond:
                                    setTime.second = (setTime.second > 0) ? setTime.second - 1 : 59;
                                    break;
                            }
                        }
                        break;
                    case PLUS_BTN:  // 加号长按：增加值
                        if (dateSelected) {
                            switch (selectedDatePart) {
                                case DateYear:
                                    setTime.year = std::min(setTime.year + 1, 2099);
                                    break;
                                case DateMonth:
                                    setTime.month = (setTime.month < 12) ? setTime.month + 1 : 1;
                                    // 调整天数
                                    if (setTime.day > daysInMonth(setTime.year, setTime.month)) {
                                        setTime.day = daysInMonth(setTime.year, setTime.month);
                                    }
                                    break;
                                case DataDay:
                                    setTime.day = (setTime.day < daysInMonth(setTime.year, setTime.month)) ?
                                                  setTime.day + 1 : 1;
                                    break;
                            }
                        }
                        if (timeSelected) {
                            switch (selectedTimePart) {
                                case TimeHour:
                                    setTime.hour = (setTime.hour < 23) ? setTime.hour + 1 : 0;
                                    break;
                                case TimeMinute:
                                    setTime.minute = (setTime.minute < 59) ? setTime.minute + 1 : 0;
                                    break;
                                case TimeSecond:
                                    setTime.second = (setTime.second < 59) ? setTime.second + 1 : 0;
                                    break;
                            }
                        }
                        break;
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
            datePos = -1;
            timePos = 0;
            dateOffset = 0;
            timeOffset = 0;
            targetDateOffset = 0;
            targetTimeOffset = 0;
            isAnimating = false;

            // 重置选择状态
            selectedDatePart = DateYear;
            selectedTimePart = TimeHour;
            dateSelected = false;
            timeSelected = false;

            // 重置选择框位置
            timedateSelectBoxX = timedateSelectBoxY = timedateSelectBoxW = timedateSelectBoxH = 0;
            timedateSelectBoxtargetX = timedateSelectBoxtargetY = timedateSelectBoxtargetW = timedateSelectBoxtargetH = 0;

        }
    }
}

void Clock::StopwatchPopup(std::string _info) {
    HAL::setFont(getUIConfig().mainFont);
    static bool init = false;
    static bool onRender = false;

    const int screenWidth = HAL::getSystemConfig().screenWeight;
    const int screenHeight = HAL::getSystemConfig().screenHeight;

    // 按钮尺寸定义
    static const int exitSize = 10;
    static const float exitMargin = 4;
    static const int btnWidth = 40;
    static const int btnHeight = 20;
    static const float btnRadius = 4;
    static const float btnSpacing = 10;  // 按钮之间的间距

    // 位置变量
    int exitX, exitY;
    float startBtnX, startBtnY;
    float stopBtnX, stopBtnY;

    // 选择框变量
    static float selectBoxX = 0, selectBoxY = 0;
    static float selectBoxW = 0, selectBoxH = 0;
    static float selectBoxtargetX = 0, selectBoxtargetY = 0;
    static float selectBoxtargetW = 0, selectBoxtargetH = 0;

    // 秒表状态
    static bool stopwatchRunning = false;
    static unsigned long stopwatchStartTime = 0;
    static unsigned long stopwatchElapsedTime = 0;
    static unsigned long stopwatchLastUpdate = 0;
    static bool isStopped = false;  // 新增：标记秒表是否已停止

    // 秒表弹窗特有的选择项枚举 - 现在有3个选项
    enum SelectedItemStopwatch {
        STOPWATCH_EXIT_BTN,
        STOPWATCH_STOP_BTN,
        STOPWATCH_START_BTN
    };
    static SelectedItemStopwatch selectedItemStopwatch = STOPWATCH_EXIT_BTN;

    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;

    // 动画变量
    static float currentW = 0;
    static float currentH = 0;
    static float targetW = screenWidth;
    static float targetH = screenHeight;

    // 初始化
    if (!init) {
        init = true;
        onRender = true;

        // 初始化选择框尺寸
        selectBoxtargetW = exitSize + 4;
        selectBoxtargetH = exitSize + 4;

        // 初始化秒表状态
        stopwatchRunning = false;
        isStopped = false;
        stopwatchElapsedTime = 0;
        stopwatchStartTime = HAL_GetTick();
        stopwatchLastUpdate = HAL_GetTick();
    }

    MyClock->resetEnterAnimation();

    while (onRender) {
        HAL::canvasClear();
        Setting::Screen_Settig();

        // 计算弹窗位置
        const float xPop = centerX - currentW / 2;
        const float yPop = centerY - currentH / 2;

        // 计算标题位置（居中）
        const float Xtitle = (screenWidth - HAL::getFontWidth(_info)) / 2;

        // 计算退出按钮位置（右上角）
        exitX = static_cast<int>(xPop + currentW - exitMargin - exitSize);
        exitY = static_cast<int>(yPop + exitMargin);

        // 计算按钮总宽度（两个按钮+间距）
        float totalButtonsWidth = (2 * btnWidth) + btnSpacing;

        // 计算按钮区域起始X位置（居中）
        float buttonsStartX = xPop + (currentW - totalButtonsWidth) / 2;

        // 计算Stop按钮位置（左侧）
        stopBtnX = buttonsStartX;
        stopBtnY = yPop + currentH - exitMargin - btnHeight;

        // 计算Start按钮位置（右侧）
        startBtnX = stopBtnX + btnWidth + btnSpacing;
        startBtnY = stopBtnY;

        // 计算选择框目标位置
        switch (selectedItemStopwatch) {
            case STOPWATCH_EXIT_BTN:
                selectBoxtargetX = exitX - 2;
                selectBoxtargetY = exitY - 2;
                selectBoxtargetW = exitSize + 4;
                selectBoxtargetH = exitSize + 4;
                break;
            case STOPWATCH_STOP_BTN:
                selectBoxtargetX = stopBtnX - 2;
                selectBoxtargetY = stopBtnY - 2;
                selectBoxtargetW = btnWidth + 4;
                selectBoxtargetH = btnHeight + 4;
                break;
            case STOPWATCH_START_BTN:
                selectBoxtargetX = startBtnX - 2;
                selectBoxtargetY = startBtnY - 2;
                selectBoxtargetW = btnWidth + 4;
                selectBoxtargetH = btnHeight + 4;
                break;
        }

        // ========== 绘制弹窗背景和边框 ==========
        HAL::setDrawType(0);
        HAL::drawRBox(xPop, yPop, currentW, currentH, getUIConfig().popRadius);
        HAL::setDrawType(1);
        HAL::drawRFrame(xPop, yPop, currentW, currentH, getUIConfig().popRadius);

        // 只有当弹窗展开到一定大小后才绘制内容
        if (currentW > HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin &&
            currentH > HAL::getFontHeight() + 2 * getUIConfig().popMargin) {

            // ========== 绘制标题 ==========
            HAL::drawEnglish(Xtitle, yPop + HAL::getFontHeight() - 3, _info);

            // ========== 绘制退出按钮（叉号） ==========
            HAL::drawRFrame(exitX, exitY, exitSize + 1, exitSize + 1, 2);
            for (int i = 0; i <= exitSize; ++i) {
                HAL::drawPixel(exitX + i, exitY + i);
                HAL::drawPixel(exitX + exitSize - i, exitY + i);
            }

            // ========== 绘制Stop按钮 ==========
            HAL::setDrawType(0);
            HAL::drawRBox(stopBtnX, stopBtnY, btnWidth, btnHeight, btnRadius);
            HAL::setDrawType(1);
            HAL::drawRFrame(stopBtnX, stopBtnY, btnWidth, btnHeight, btnRadius);

            // 在Stop按钮上绘制文字
            std::string stopText = "Stop";
            float stopTextX = stopBtnX + (btnWidth - HAL::getFontWidth(stopText)) / 2;
            float stopTextY = stopBtnY + (btnHeight - HAL::getFontHeight()) / 2 + HAL::getFontHeight();
            HAL::drawEnglish(stopTextX, stopTextY, stopText);

            // ========== 绘制Start按钮 ==========
            HAL::setDrawType(0);
            HAL::drawRBox(startBtnX, startBtnY, btnWidth, btnHeight, btnRadius);
            HAL::setDrawType(1);
            HAL::drawRFrame(startBtnX, startBtnY, btnWidth, btnHeight, btnRadius);

            // 在Start按钮上绘制文字
            std::string startText = stopwatchRunning ? "Pause" : "Start";
            float startTextX = startBtnX + (btnWidth - HAL::getFontWidth(startText)) / 2;
            float startTextY = startBtnY + (btnHeight - HAL::getFontHeight()) / 2 + HAL::getFontHeight();
            HAL::drawEnglish(startTextX, startTextY, startText);

            // ========== 秒表时间显示区域 ==========
            float contentAreaX = xPop + getUIConfig().popMargin;
            float contentAreaY = yPop + HAL::getFontHeight() + 10;
            float contentAreaW = currentW - 2 * getUIConfig().popMargin;
            float contentAreaH = stopBtnY - contentAreaY - 10;

            // 更新秒表时间（如果正在运行）
            unsigned long currentTime = HAL_GetTick();
            if (stopwatchRunning) {
                if (currentTime - stopwatchLastUpdate >= 10) { // 每10ms更新一次
                    stopwatchElapsedTime += (currentTime - stopwatchLastUpdate);
                    stopwatchLastUpdate = currentTime;
                }
            }

            // 将毫秒转换为时:分:秒.毫秒格式
            unsigned long totalMs = stopwatchElapsedTime;
            unsigned int hours = totalMs / 3600000;
            unsigned int minutes = (totalMs % 3600000) / 60000;
            unsigned int seconds = (totalMs % 60000) / 1000;
            unsigned int milliseconds = totalMs % 1000;

            // 格式化时间字符串
            char timeStr[32];
            if (hours > 0) {
                snprintf(timeStr, sizeof(timeStr), "%02u:%02u:%02u.%03u",
                         hours, minutes, seconds, milliseconds);
            } else {
                snprintf(timeStr, sizeof(timeStr), "%02u:%02u.%03u",
                         minutes, seconds, milliseconds);
            }

            // 居中显示时间
            float timeX = contentAreaX + (contentAreaW - HAL::getFontWidth(timeStr)) / 2;
            float timeY = contentAreaY + (contentAreaH - HAL::getFontHeight()) / 2 + HAL::getFontHeight();
            HAL::drawEnglish(timeX, timeY, timeStr);



            // ========== 绘制选择框 ==========
            HAL::setDrawType(2);  // 反色绘制
            HAL::drawRBox(selectBoxX, selectBoxY, selectBoxW, selectBoxH, 2);
            HAL::setDrawType(1);  // 恢复正常绘制
        }

        HAL::canvasUpdate();

        // ========== 执行动画 ==========
        Animation::move(&currentW, targetW, getUIConfig().popSpeed);
        Animation::move(&currentH, targetH, getUIConfig().popSpeed);
        Animation::move(&selectBoxX, selectBoxtargetX, getUIConfig().popSpeed);
        Animation::move(&selectBoxY, selectBoxtargetY, getUIConfig().popSpeed);
        Animation::move(&selectBoxW, selectBoxtargetW, getUIConfig().popSpeed);
        Animation::move(&selectBoxH, selectBoxtargetH, getUIConfig().popSpeed);

        // ========== 按键处理 ==========
        HAL::keyScan();
        if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
            for (unsigned char i = 0; i < key::KEY_NUM; i++) {
                if (HAL::getKeyMap()[i] == key::CLICK) {
                    // 短按：切换选择项（现在有3个选项）
                    if (i == 1) {  // 右箭头
                        selectedItemStopwatch = static_cast<SelectedItemStopwatch>(
                            (selectedItemStopwatch + 1) % 3
                        );
                    }
                    if (i == 0) {  // 左箭头
                        switch (selectedItemStopwatch) {
                            case STOPWATCH_STOP_BTN:
                                // 长按Stop按钮：完全重置
                                stopwatchRunning = false;
                                isStopped = false;
                                stopwatchElapsedTime = 0;
                                stopwatchStartTime = HAL_GetTick();
                                stopwatchLastUpdate = stopwatchStartTime;
                                break;

                            case STOPWATCH_START_BTN:
                                // Start按钮：开始/暂停计时
                                if (isStopped) {
                                    // 如果之前已停止，先重置状态
                                    isStopped = false;
                                    stopwatchElapsedTime = 0;
                                }

                                stopwatchRunning = !stopwatchRunning;
                                if (stopwatchRunning) {
                                    stopwatchStartTime = HAL_GetTick();
                                    stopwatchLastUpdate = stopwatchStartTime;
                                }
                                break;

                        }

                    }
                } else if (HAL::getKeyMap()[i] == key::PRESS) {
                    // 长按：执行操作
                    if (i == 1) {
                        switch (selectedItemStopwatch) {

                            case STOPWATCH_EXIT_BTN:
                                // 退出弹窗
                                targetW = 0;
                                targetH = 0;
                                break;
                        }


                    }
                } else if (HAL::getKeyMap()[i] == key::HOLD) {
                    // 长按保持：特定功能
                    if (i == 0) {  // 左箭头长按

                    }
                }
            }
            // 清空按键状态
            std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
            *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
        }

        // ========== 退出条件 ==========
        if (currentW <= 8.0f && currentH <= 8.0f) {
            onRender = false;
            init = false;
            currentW = 0;
            currentH = 0;
            targetW = screenWidth;
            targetH = screenHeight;

            // 重置选择状态
            selectedItemStopwatch = STOPWATCH_EXIT_BTN;

            // 重置选择框位置
            selectBoxX = selectBoxY = selectBoxW = selectBoxH = 0;
            selectBoxtargetX = selectBoxtargetY = selectBoxtargetW = selectBoxtargetH = 0;

            // 可选：保存秒表状态到全局变量，以便下次打开时恢复
            // 或者直接重置，根据您的需求
        }
    }
}

 void Clock::setTime() {
    if (ClocksetFlag == true) {
        struct tm BeiJingTime;
        BeiJingTime.tm_year = nwt.year - 1900;
        BeiJingTime.tm_mon = nwt.month - 1;
        BeiJingTime.tm_mday = nwt.date;
        BeiJingTime.tm_hour = nwt.hour;
        BeiJingTime.tm_min = nwt.min;
        BeiJingTime.tm_sec = nwt.sec;
        BeiJingTime.tm_wday = 0;
        BeiJingTime.tm_yday = 0;
        BeiJingTime.tm_isdst = 0;
        My_RTC_SetTime(&BeiJingTime);
        //My_RTC_SetTime(&time);  // 首次运行时设置RTC时间
        ClocksetFlag = false;
    }
}
void Clock::updateTime() {
    ClockSetPupFlag=false;





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


    HAL::setFont(getUIConfig().mainFont); // 恢复默认字体
}
}  // namespace astra