#include "GYRO_app.h"
#include "HAL.h"
#include <cmath>

namespace astra {

std::string GYRO::getType() const {
    return "GYRO";
}

GYRO::GYRO() : GYRO("GYRO", generateDefaultPic()) {}
GYRO::GYRO(const std::string& _title) : GYRO(_title, generateDefaultPic()) {}

GYRO::GYRO(const std::string& _title, const std::vector<unsigned char>& _pic) {
    this->title = _title;
    this->pic = _pic;
    this->selectIndex = 0;
    this->parent = nullptr;
    this->childMenu.clear();
    this->childWidget.clear();
    this->position = {};
    resetEnterAnimation();
}

void GYRO::resetEnterAnimation() {
    isEntering = true;
    posY = -40;       // 从屏幕外上方开始
    isTextAnimating = true;

    int w = HAL::getSystemConfig().screenWeight;
    titlePos = -100.0f;
    pitchPos = w + 100.0f;
    rollPos = w + 100.0f;
    yawPos = w + 100.0f;

    titleTarget = 0.0f;
    pitchTarget = 0.0f;
    rollTarget = 0.0f;
    yawTarget = 0.0f;
}

std::vector<unsigned char> GYRO::generateDefaultPic() {
    return {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xff, 0xff, 0x03, 0xe0, 0xff,
        0xff, 0xe1, 0xc7, 0xff, 0xff, 0x30, 0x9c, 0xff, 0x7f, 0x18, 0x30, 0xff, 0x3f, 0x0c, 0xe0, 0xff, 0x3f, 0xce, 0xe1, 0xff, 0x3f, 0xee, 0xc3, 0xff,
        0x1f, 0xef, 0xc7, 0xff, 0x1f, 0xff, 0x87, 0xf0, 0x1f, 0xff, 0xff, 0xf1, 0x1f, 0x07, 0xfc, 0xf1, 0x1f, 0x03, 0x3c, 0xf8, 0x1f, 0x8f, 0x07, 0xfe,
        0x1f, 0xff, 0xc0, 0xff, 0x0f, 0x06, 0xf8, 0xff, 0x0f, 0x06, 0xcf, 0xfd, 0x3f, 0xfc, 0xc7, 0xfc, 0x7f, 0x78, 0x43, 0xfe, 0xff, 0x70, 0x60, 0xfe,
        0xff, 0xe0, 0x11, 0xff, 0xff, 0xc1, 0x8f, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x0f, 0xe0, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xf7, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };
}

void GYRO::deInit() {
    Menu::deInit();
    isEntering = true;
    isTextAnimating = true;
}

// =========================== 标准正交 3D 坐标轴 + 立方体 ===========================
#define LEN 14
#define CX 32
#define CY 42

static float rad(float d) { return d * 3.14159265f / 180.0f; }

static void line(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int s = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (s == 0) { HAL::drawPixel(x1, y1); return; }
    for (int i = 0; i <= s; i++) {
        float f = i / (float)s;
        int x = x1 + dx * f;
        int y = y1 + dy * f;
        HAL::drawPixel(x, y);
    }
}

static void drawXYZ(float pitch, float roll, float yaw) {
    float p = rad(pitch);
    float r = rad(roll);
    float y = rad(yaw);

    auto rot = [&](float x, float yy, float z) -> void {
        float t;
        t = x; x = x*cos(y) - yy*sin(y); yy = t*sin(y) + yy*cos(y);
        t = yy; yy = yy*cos(p) - z*sin(p); z = t*sin(p) + z*cos(p);
        t = x; x = x*cos(r) + z*sin(r); z = -t*sin(r) + z*cos(r);

        float sc = 1.0f / (1.0f + z*0.02f);
        int rx = CX + x*sc;
        int ry = CY - yy*sc;
        line(CX, CY, rx, ry);
    };

    rot(LEN, 0, 0);
    rot(0, LEN, 0);
    rot(0, 0, LEN);
}

static void drawCube(float pitch, float roll, float yaw) {
    float p = rad(pitch);
    float r = rad(roll);
    float y = rad(yaw);
    int s = 10;

    float vx[8] = {(float)-s, (float)s, (float)s, (float)-s, (float)-s, (float)s, (float)s, (float)-s};
    float vy[8] = {(float)-s, (float)-s, (float)s, (float)s, (float)-s, (float)-s, (float)s, (float)s};
    float vz[8] = {(float)-s, (float)-s, (float)-s, (float)-s, (float)s, (float)s, (float)s, (float)s};
    int px[8], py[8];

    for (int i=0; i<8; i++) {
        float x = vx[i], yy = vy[i], z = vz[i];
        float t;

        t = x; x = x*cos(y) - yy*sin(y); yy = t*sin(y) + yy*cos(y);
        t = yy; yy = yy*cos(p) - z*sin(p); z = t*sin(p) + z*cos(p);
        t = x; x = x*cos(r) + z*sin(r); z = -t*sin(r) + z*cos(r);

        float sc = 1.0f / (1.0f + z*0.02f);
        px[i] = CX + x*sc;
        py[i] = CY - yy*sc;
    }

    for (int i=0; i<4; i++) {
        line(px[i], py[i], px[(i+1)%4], py[(i+1)%4]);
        line(px[i+4], py[i+4], px[(i+1)%4+4], py[(i+1)%4+4]);
        line(px[i], py[i], px[i+4], py[i+4]);
    }
}

void GYRO::render(const std::vector<float>& _camera) {
    Item::updateConfig();
    HAL::setDrawType(1);

    int scr_w = HAL::getSystemConfig().screenWeight;
    int scr_h = HAL::getSystemConfig().screenHeight;
    int fh = HAL::getFontHeight();

    // ====================== 背景框从上往下滑入 ======================
    if (isEntering) {
        Animation::move(&posY, 0.0f, 70);

        // 还没滑到目标位置
        if (posY < -1.0f) {
            HAL::drawRFrame(0, (int)posY, scr_w, scr_h, 2);
            return; // 只画框，不画立方体
        }

        // 滑入结束
        posY = 0.0f;
        isEntering = false;
    }

    // ====================== 滑入完成后才绘制全部内容 ======================
    HAL::drawRFrame(0, 0, scr_w, scr_h, 2);

    // 标题
    int titleX = 12;
    int titleY = fh + 4;
    if (isTextAnimating)
        Animation::move(&titlePos, (float)titleX, 90);
    HAL::drawEnglish((int)titlePos, titleY, title.c_str());

    // 数据
    const int dataBaseX = scr_w * 3 / 4-4;
    const int pitchY = (scr_h - fh)/2 - 2;
    const int rollY  = (scr_h + fh)/2;
    const int yawY   = scr_h/2 + fh*3/2 + 2;

    float p = MM.pitch;
    float r = MM.roll;
    float y = MM.yaw;

    char buf[64];
    snprintf(buf, sizeof(buf), "pitch:%.2f", p);
    int pitchX = dataBaseX - HAL::getFontWidth(buf)/2;
    if (isTextAnimating) Animation::move(&pitchPos, (float)pitchX, 90);
    HAL::drawEnglish((int)pitchPos, pitchY, buf);

    snprintf(buf, sizeof(buf), "roll:%.2f", r);
    int rollX = dataBaseX - HAL::getFontWidth(buf)/2;
    if (isTextAnimating) Animation::move(&rollPos, (float)rollX, 90);
    HAL::drawEnglish((int)rollPos, rollY, buf);

    snprintf(buf, sizeof(buf), "yaw:%.2f", y);
    int yawX = dataBaseX - HAL::getFontWidth(buf)/2;
    if (isTextAnimating) Animation::move(&yawPos, (float)yawX, 90);
    HAL::drawEnglish((int)yawPos, yawY, buf);

    // 文字动画结束
    if (isTextAnimating) {
        bool ok = (fabs(titlePos - titleX) < 1 &&
                   fabs(pitchPos - pitchX) < 1 &&
                   fabs(rollPos - rollX) < 1 &&
                   fabs(yawPos - yawX) < 1);
        if (ok) isTextAnimating = false;
    }

    //只有滑入全部结束，才会画立方体
    if (!isEntering && !isTextAnimating) {
        drawCube(p, r, y);
        drawXYZ(p, r, y);
    }
}

}