//
// Created by Fir on 2024/2/1.
//

#pragma once
#ifndef ASTRA_APP_ASTRA_APP_H_
#define ASTRA_APP_ASTRA_APP_H_

#include "menu.h"
#include <vector>
#include <string>

namespace astra {
class SerialMonitor : public Menu {
public:
    int startLine = 0; // 当前可视区域起始行（补充声明）
    const int maxDisplayLines = 4; // 最大可视行数（补充声明，与数据绘制逻辑一致）
    std::vector<std::string> recvData;  // 存储接收的串口数据列表
    const uint8_t MAX_LINES = 64;  // 数据存储的最大行数（超过则删除旧数据，防止内存溢出）
    bool needAutoScroll = false;  // 自动滚动标志：是否需要触发自动滚动
    float autoScrollTargetY = 0;  // 自动滚动的目标Y坐标（用于Camera类实现滚动动画）
    bool isManualScrolling = false;  // 手动滚动标志：标记当前是否处于手动滚动状态
    // 可视范围边界 [0] = startLine（上边界）, [1] = endLine（下边界）
    std::vector<int> boundary = {0, 0};
    bool initFlag = false; // 初始化标志（首次进入时初始化）
    // 每页最大显示行数（根据屏幕高度和行高计算）
    // 绘制配置参数（与Tile类风格统一，使用成员常量，便于维护和统一风格）
    const int DISPLAY_LINE_HEIGHT = 12;
    const int maxLinesPerPage = maxDisplayLines; // 统一为4行，与可视行数一致
    const int BOX_Y_OFFSET = 13;  // 数据显示框的Y轴偏移（标题栏下方）
    const int Title_Y_OFFSET = 11;

    const float RADIUS = 3.0f;  // 数据框圆角半径
    const int INDICATOR_WIDTH = 3;  // 滚动指示条（滑块）宽度
    const int INDICATOR_MARGIN = 0;  // 滚动指示条与屏幕边缘的间距
    //滚动指示条动画参数
    float indicatorY = 0;         // 当前Y位置（用于动画）
    float indicatorYTrg = 0;      // 目标Y位置
    float indicatorHeight = 0;    // 当前高度（用于动画）
    float indicatorHeightTrg = 0; // 目标高度
    const int MIN_INDICATOR_HEIGHT = 8;  // 最小高度（避免过短看不见）
    int selectedLine = 0;


    // 数据框动画变量
    float frameCurrentW = 0;  // 当前宽度（从0开始）
    float frameCurrentH = 0;  // 当前高度（从0开始）
    float frameTargetW;       // 目标宽度（boxWidth）
    float frameTargetH;       // 目标高度（boxHeight）
    bool isFrameAnimating = false;  // 动画是否进行中
    bool isFrameExpanding = false;  // 是否处于展开状态


    //打开动画控制
    bool isEntering = true;  // 是否处于打开动画状态
    bool isExiting = false;
    bool isActive = false;
    float enterStartY;       // 打开动画的起始Y坐标（屏幕外）
    float exitTrgY = systemConfig.screenHeight;  // 退出时的目标Y位置（屏幕顶部外）
    // 新增：数据项动画参数（每个数据项独立的X轴位置）
    std::vector<float> dataXPos;   // 每个数据项当前X坐标
    std::vector<float> dataXTrg;   // 每个数据项目标X坐标（最终显示位置）
    const float DATA_ANIM_START_X = -50;  // 动画起始X（屏幕左侧外，根据实际宽度调整）
    // 新增：数据项Y轴动画参数（每个数据项独立的Y轴位置）
    std::vector<float> dataYPos;   // 每个数据项当前Y坐标
    std::vector<float> dataYTrg;   // 每个数据项目标Y坐标（最终显示位置）
    const float DATA_ANIM_START_Y = 38;  // Y轴动画起始位置（屏幕下方外）
    // 当前位置（X轴，用于水平滑出；也可根据需求用Y轴垂直滑出）
    float posX = 0;
    // 目标位置（退出时的最终位置，通常是屏幕外部）
    float posXTrg = 0;
    // 可选：如果需要垂直退出，添加Y轴变量
    float posY = 0;
    float posYTrg = 0;

public:
    std::string getType() const override;

    // 构造函数声明
    SerialMonitor();
    explicit SerialMonitor(const std::string& _title);
    SerialMonitor(const std::string& _title, const std::vector<unsigned char>& _pic);

    //前景元素初始化（模仿List和Tile）
    void forePosInit() override;
    /**
    * 生成默认图标像素数据
    * @return 图标像素数组（16进制表示，用于UI绘制）
    */
    std::vector<unsigned char> generateDefaultPic();


    /**
     * 获取自动滚动的目标位置（供Camera类使用）
     * @param targetY 输出参数，接收目标滚动Y坐标
     * @return 是否需要滚动（true表示需要，且targetY有效）
     */
    bool getAutoScrollTarget(float& targetY);

    /**
     * 添加字符串类型的串口数据
     * @param data 待添加的字符串数据
     */
    void addData(const std::string& data);

    /**
     * 重载addData，支持uint8_t数组类型的串口数据
     * @param data 待添加的uint8_t数组
     * @param length 数组长度
     */
    void addData(const uint8_t* data, size_t length);

    /**
     * 开始手动滚动（关闭自动滚动）
     * 手动滚动时，禁用自动滚动逻辑，避免冲突
     */
    void startManualScroll();

    /**
     * 结束手动滚动（允许自动滚动恢复）
     */
    void stopManualScroll();

    /**
     * 检查当前是否处于手动滚动状态
     * @return 手动滚动状态（true为正在手动滚动）
     */
    bool isInManualScroll() const;

    /**
     * 向上滚动一行
     * 滚动步长为配置的行高，且不超过顶部边界（Y=0）
     */
    void scrollUp();

    /**
    * 向下滚动一行
    * 滚动步长为配置的行高，且不超过底部边界
    */
    void scrollDown();

    /**
     * 重写渲染方法，绘制串口监控页面
     * @param _camera 摄像机位置（用于坐标偏移，实现滚动效果）
     */
    void render(const std::vector<float>& _camera) override;

public:
    // 新增：获取接收数据的总条数
    int getRecvDataSize() const {
        return recvData.size();
    }
    // 获取指定索引的接收数据
    const std::string& getRecvDataAt(int index) const {
        // 确保索引有效（超出范围返回空字符串）
        if (index >= 0 && index < (int)recvData.size()) {
            return recvData[index];
        }
        static const std::string emptyStr = "";
        return emptyStr;
    }
    // 获取数据框X坐标（对应 render 中的 boxX）
    int getBoxX() const {
        return static_cast<int>(posX);
    }

    // 获取数据框Y坐标（对应 render 中的 boxY）
    float getBoxY() const {
        return BOX_Y_OFFSET +Title_Y_OFFSET+ posY;  // 去掉static_cast<int>，保留浮点精度
    }
    // 新增：获取每行数据的高度（DISPLAY_LINE_HEIGHT）
    int getDisplayLineHeight() const {
        return DISPLAY_LINE_HEIGHT;
    }

    //重置打开动画状态，
    void resetEnterAnimation() {
        isEntering = true;
        posY = -systemConfig.screenHeight;
        posYTrg = 0;
        isActive = false;

        // 新增：初始化数据框动画
        frameTargetW = systemConfig.screenWeight;  // 目标宽度
        frameTargetH = systemConfig.screenHeight - BOX_Y_OFFSET;  // 目标高度
        frameCurrentW = 0;  // 从0开始展开
        frameCurrentH = 0;
        isFrameExpanding = true;
        isFrameAnimating = true;

        // 初始化选中行为最新一行（若有数据）
        if (!recvData.empty()) {
            selectedLine = recvData.size() - 1;  // 最新数据行索引
        } else {
            selectedLine = 0;
        }
    }
    // 新增：判断退出动画是否完成
    bool isExitAnimationFinished() const {
        return (posX == posXTrg) && isExiting;
    }
    // 添加：设置退出状态的公共接口
    void setIsExiting(bool value) {
        isExiting = value;
        if (isExiting) {
            posYTrg = exitTrgY;  // 退出时目标Y位置设为屏幕外
        }

    }

    // 获取当前边界（供Camera调用）
    const std::vector<int>& getBoundary() const { return boundary; }

    // 刷新边界（当滚动时更新可视范围）
    void refreshBoundary(int newStart, int newEnd) {
        boundary[0] = newStart;
        boundary[1] = newEnd;
    }
public:
    // 重写父类的deInit方法
    void deInit() override;  // 添加此声明


};
}  // namespace astra

#endif //ASTRA_APP_ASTRA_APP_H_
