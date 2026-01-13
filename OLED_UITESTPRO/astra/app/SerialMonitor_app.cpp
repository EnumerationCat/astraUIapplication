#include "SerialMonitor_app.h"

namespace astra {

std::string SerialMonitor::getType() const {
    return "SerialMonitor";
}

SerialMonitor::SerialMonitor() : SerialMonitor("Serial", generateDefaultPic()) {}

SerialMonitor::SerialMonitor(const std::string& _title) : SerialMonitor(_title, generateDefaultPic()) {}

    SerialMonitor::SerialMonitor(const std::string& _title, const std::vector<unsigned char>& _pic) {
    this->title = _title;
    this->pic = _pic;
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};

    // 初始化接收数据
    recvData.push_back("Serial Ready");
    recvData.push_back("Waiting for data...");

    // 初始化初始数据的动画参数
    // 对应 "Serial Ready" 和 "Waiting for data..." 两个初始数据
    dataXPos.push_back(DATA_ANIM_START_X);
    dataXTrg.push_back(0);
    dataYPos.push_back(DATA_ANIM_START_Y);
    dataYTrg.push_back(0);

    dataXPos.push_back(DATA_ANIM_START_X);
    dataXTrg.push_back(0);
    dataYPos.push_back(DATA_ANIM_START_Y);
    dataYTrg.push_back(0);

    // 初始化打开动画参数
    isEntering = true;  // 进入时触发打开动画
    enterStartY = -systemConfig.screenHeight;  // 起始位置：屏幕顶部外（完全看不见）
    posY = enterStartY;  // 当前Y坐标从起始位置开始
    posYTrg = 0;  // 目标位置：屏幕内（正常显示位置）
}



std::vector<unsigned char> SerialMonitor::generateDefaultPic() {
    return {
        0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F,
        0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF0, 0xE7, 0x3F, 0x7F, 0xE0, 0xE7, 0x3F,
        0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xC3, 0x3F, 0x7F, 0xE0, 0xE7, 0x3F,
        0xFF, 0xF0, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F,
        0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xC3, 0x3F, 0xFF, 0xF9, 0x81, 0x3F, 0xFF, 0xF0, 0x81, 0x3F,
        0xFF, 0xF0, 0x81, 0x3F, 0xFF, 0xF0, 0x81, 0x3F, 0xFF, 0xF9, 0x81, 0x3F, 0xFF, 0xF9, 0xC3, 0x3F,
        0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xF9, 0xE7, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F,
        0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFF, 0x3F
    };
}

    void SerialMonitor::forePosInit() {
    // 滚动指示条初始化
    //indicatorHeight = 0;
    // 目标高度：默认最小高度
    indicatorHeightTrg = MIN_INDICATOR_HEIGHT;
    // 初始位置：屏幕外（不可见）
    indicatorY = systemConfig.screenHeight;
    // 目标位置：数据框内（后续会根据滚动进度动态调整）
    indicatorYTrg = BOX_Y_OFFSET + Title_Y_OFFSET;

}

bool SerialMonitor::getAutoScrollTarget(float& targetY) {
    if (needAutoScroll) {
        targetY = autoScrollTargetY;
        needAutoScroll = false;
        return true;
    }
    return false;
}

    void SerialMonitor::addData(const std::string& data) {
    if (!isActive) {
        return;
    }
    recvData.push_back(data);
    dataXPos.push_back(DATA_ANIM_START_X);
    dataXTrg.push_back(0);
    dataYPos.push_back(DATA_ANIM_START_Y);  // 新增
    dataYTrg.push_back(0);                  // 新增


    // 数据行数限制逻辑
    const int MAX_HISTORY_LINES = 1000;
    if (recvData.size() > MAX_HISTORY_LINES) {
        recvData.erase(recvData.begin());
        dataXPos.erase(dataXPos.begin());
        dataXTrg.erase(dataXTrg.begin());
    }

    //将选中行设置为最新数据行
    selectedLine = recvData.size() - 1;  // 最新行索引
    this->selectIndex = selectedLine;    // 同步menu的选中索引，确保选择器跟随

    // 自动滚动逻辑
    if (recvData.size() > maxDisplayLines) {
        // 目标位置调整为最新行可见（在可视区域底部）
        autoScrollTargetY = -((recvData.size() - maxDisplayLines) * DISPLAY_LINE_HEIGHT);
        needAutoScroll = true;
    }
}

    // 同样在uint8_t重载版本中确保同步（如果需要）
    void SerialMonitor::addData(const uint8_t* data, size_t length) {
    if (!isActive) {
        return;
    }
    std::string strData(reinterpret_cast<const char*>(data), length);
    addData(strData);  // 复用上面修改后的字符串版本，自动包含选中行更新
}
//手动查看数据
void SerialMonitor::startManualScroll() {
    isManualScrolling = true;
    needAutoScroll = false;
}
//停止手动查看数据
void SerialMonitor::stopManualScroll() {
    isManualScrolling = false;
}
//是否手动查看数据
bool SerialMonitor::isInManualScroll() const {
    return isManualScrolling;
}

//向上滚动
void SerialMonitor::scrollUp() {
    startManualScroll();
    // 上滚一行：目标Y = 当前Y + 行高（最多到0，不超过顶部）
    autoScrollTargetY = std::min(autoScrollTargetY + DISPLAY_LINE_HEIGHT, 0.0f);

    needAutoScroll = true;
}
//向下滚动
void SerialMonitor::scrollDown() {
    startManualScroll();
    int maxScrollLines = std::max(0, (int)recvData.size() - maxDisplayLines);
    float maxScroll = -(maxScrollLines * DISPLAY_LINE_HEIGHT);  // 最大下滚位置
    // 下滚一行：目标Y = 当前Y - 行高（不超过最大下滚位置）
    autoScrollTargetY = std::max(autoScrollTargetY - DISPLAY_LINE_HEIGHT, maxScroll);
    needAutoScroll = true;
}
//渲染数据监视界面
void SerialMonitor::render(const std::vector<float>& _camera) {
    Item::updateConfig();
    HAL::setDrawType(1);
    //进入动画
    if (isEntering) {
        Animation::move(&posY, posYTrg, astraConfig.tileAnimationSpeed);
        if (posY == posYTrg) {
            isEntering = false;
            isActive = true;

            // 动画结束后，自动滚动到选中行（最新一行）
            if (!recvData.empty()) {
                // 触发摄像机滚动到选中行
                needAutoScroll = true;
                autoScrollTargetY = -(selectedLine - (maxDisplayLines - 1)) * DISPLAY_LINE_HEIGHT;
                autoScrollTargetY = std::max(autoScrollTargetY, 0.0f);  // 不超过顶部
            }
        }
    }

    // 处理数据框展开动画
    if (isFrameExpanding && isFrameAnimating) {
        // 从0过渡到目标宽高（速度使用配置的动画速度）
        Animation::move(&frameCurrentW, frameTargetW, astraConfig.tileAnimationSpeed);
        Animation::move(&frameCurrentH, frameTargetH, astraConfig.tileAnimationSpeed);

        // 检查动画是否完成（考虑浮点数精度）
        if (std::abs(frameCurrentW - frameTargetW) < 0.1f &&
            std::abs(frameCurrentH - frameTargetH) < 0.1f) {
            frameCurrentW = frameTargetW;
            frameCurrentH = frameTargetH;
            isFrameAnimating = false;  // 动画结束
            }
    }
    // 计算绘制数据区域参数
    const int boxX = posX;
    const int boxY = BOX_Y_OFFSET + posY;
    const int boxWidth = systemConfig.screenWeight;
    const int boxHeight = systemConfig.screenHeight - BOX_Y_OFFSET;
    const int indicatorX = boxX + systemConfig.screenWeight - INDICATOR_WIDTH - INDICATOR_MARGIN;
    // 计算数据框的中心点坐标（目标状态下）
    float frameCenterX = boxX + boxWidth / 2.0f;  // X中心
    float frameCenterY = boxY + boxHeight / 2.0f; // Y中心
    // 绘制标题
    HAL::setDrawType(1);
    HAL::drawRBox(boxX, -posY, HAL::getFontWidth(title) + 3, HAL::getFontHeight() - 1, 2);
    HAL::setDrawType(2);
    HAL::drawChinese(boxX + 2, -posY + 10, title);
    HAL::setDrawType(1);

    // 绘制数据框
    //HAL::drawRFrame(boxX, boxY + posY, boxWidth, boxHeight, RADIUS);
    // 新代码：从中心展开的动画绘制
    if (frameCurrentW > 0 && frameCurrentH > 0) {  // 宽高>0时才绘制
        // 计算当前帧的左上角坐标（基于中心点偏移）
        float currentX = frameCenterX - frameCurrentW / 2.0f;  // 中心向左偏移半宽
        float currentY = frameCenterY - frameCurrentH / 2.0f;  // 中心向上偏移半高

        // 绘制带圆角的边框（使用当前动画尺寸）
        HAL::drawRFrame(currentX, currentY, frameCurrentW, frameCurrentH, RADIUS);
    }


    // 关键修复：根据摄像机位置计算实际显示的起始行
    float cameraY = _camera[1]; // 摄像机Y坐标（负值表示向下滚动）
    int totalLines = recvData.size();

    // 计算应该显示的起始行（基于摄像机位置）
    startLine = static_cast<int>(-cameraY / DISPLAY_LINE_HEIGHT);
    startLine = std::max(0, startLine);
    startLine = std::min(startLine, std::max(0, totalLines - maxDisplayLines));

    // 关键修复：正确计算结束行（应该是 startLine + maxDisplayLines - 1）
    int endLine = std::min(startLine + maxDisplayLines - 1, totalLines - 1);
    refreshBoundary(startLine, endLine);

    // 绘制数据内容 - 处理所有行，仅绘制可视区域内的行
    const float targetDataX = boxX + 4;

    // 遍历所有数据行（0到最后一行）
    for (int i = 0; i < recvData.size(); ++i) {
        // 计算当前行的目标Y坐标（基于全局索引和滚动偏移）
        // 核心：目标Y = 基础位置 + 全局索引*行高 - 摄像机Y偏移（实现滚动效果）
        float targetDataY = boxY + Title_Y_OFFSET + i * DISPLAY_LINE_HEIGHT + _camera[1];

        // 初始化动画参数（所有行都需初始化，无论是否可见）
        if (i >= dataXPos.size()) {
            //初始化X/Y轴动画起始位置
            dataXPos.resize(i + 1, DATA_ANIM_START_X);
            dataXTrg.resize(i + 1, targetDataX);
            dataYPos.resize(i + 1, DATA_ANIM_START_Y);
            dataYTrg.resize(i + 1, targetDataY);
        } else {
            // 已有行：更新目标坐标（随滚动实时变化）
            dataXTrg[i] = targetDataX;
            dataYTrg[i] = targetDataY;
        }

        // 对所有行执行动画计算（包括不可见行，确保进入可视区域时动画连贯）
        Animation::move(&dataXPos[i], dataXTrg[i], astraConfig.tileAnimationSpeed);
        Animation::move(&dataYPos[i], dataYTrg[i], astraConfig.tileAnimationSpeed);

        // 仅绘制可视区域内的行（减少无效绘制，优化性能）
        if (dataYPos[i] >= boxY+8 && dataYPos[i] <= boxY + boxHeight + Title_Y_OFFSET - DISPLAY_LINE_HEIGHT) {
            HAL::drawEnglish(dataXPos[i], dataYPos[i], recvData[i]);
        }
    }

    // 绘制滚动指示条
    if (totalLines > maxDisplayLines) {
        float scrollRatio = static_cast<float>(startLine) / std::max(1, totalLines - maxDisplayLines);

        indicatorHeightTrg = (static_cast<float>(maxDisplayLines) / totalLines) * boxHeight;
        indicatorHeightTrg = std::max(indicatorHeightTrg, static_cast<float>(MIN_INDICATOR_HEIGHT));

        indicatorYTrg = boxY + scrollRatio * (boxHeight - indicatorHeightTrg);

        Animation::move(&indicatorY, indicatorYTrg, astraConfig.listAnimationSpeed);
        Animation::move(&indicatorHeight, indicatorHeightTrg, astraConfig.listAnimationSpeed);

        HAL::setDrawType(2);
        HAL::drawVLine(indicatorX + INDICATOR_WIDTH / 2, boxY, boxHeight);
        HAL::setDrawType(1);
        HAL::drawRBox(
            indicatorX,
            indicatorY,
            INDICATOR_WIDTH,
            static_cast<int>(indicatorHeight),
            1
        );
    } else {
        indicatorHeight = 0;
    }
}

    void SerialMonitor::deInit() {
    setIsExiting(true);
    isActive = false; // 禁用新数据处理
    Menu :: deInit();
}

}  // namespace astra