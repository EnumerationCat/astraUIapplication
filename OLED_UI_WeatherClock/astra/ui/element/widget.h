#pragma once
#ifndef ASTRA_WIDGET__H
#define ASTRA_WIDGET__H

#include "config.h"
#include "item.h"

extern unsigned char ScreenPopUp_Value;

enum Widget_Selector {
  Widget_Selector_Select=0,
  Widget_Selector_NoSelect=1
};
extern  Widget_Selector widget_selector;
namespace astra {

class Widget : public Item {
public:
  void *parent{};
  unsigned char value{};
  bool isActive = false;

public:
  [[nodiscard]] virtual std::string getType() const { return "base"; }

public:
  Widget() = default;

public:
  virtual void init() {}
  virtual void deInit() {}
  //open and close是启动器的事情 与控件无关

public:
  //绘制控件在列表中的指示器
  virtual void renderIndicator(float _x, float _y, const std::vector<float> &_camera) {}

public:
  virtual void render(const std::vector<float> &_camera) {}
};

class CheckBox : public Widget {
public:
  [[nodiscard]] std::string getType() const override { return "CheckBox"; }

private:
  bool isCheck;

public:
  explicit CheckBox(bool &_value);  //check box.

public:
  bool check();
  bool uncheck();
  bool toggle();

public:
  void init() override;
  void deInit() override;

public:
  void renderIndicator(float _x, float _y, const std::vector<float> &_camera) override;

public:
  void render(const std::vector<float> &_camera) override;
};

class PopUp : public Widget {
  // 新增：文本弹入动画变量
  float textCurrentX = 0;       // 当前文本X坐标（动画过程中实时更新）
  float textTargetX = 0;        // 文本目标X坐标（最终位置）
public:
  [[nodiscard]] std::string getType() const override { return "PopUp"; }

public:
  typedef struct Position {
    float x, xTrg;
    float y, yTrg;
  } Position;

  Position position{};

private:
  std::string title;
  std::vector<std::string> options;
  unsigned char direction;
  unsigned char boundary;

public:
  // 0: left 1: top 2: right 3: bottom
  PopUp(unsigned char _direction,
        const std::string &_title,
        const std::vector<std::string> &_options,
        unsigned char &_value);  //pop up.

public:
  void selectNext();
  void selectPreview();
  bool select(unsigned char _index);

public:
  void init() override;
  void deInit() override;

public:
  void renderIndicator(float _x, float _y, const std::vector<float> &_camera) override;

public:
  void render(const std::vector<float> &_camera) override;
};

class Slider : public Widget {
public:
  [[nodiscard]] std::string getType() const override { return "Slider"; }


public:
  typedef struct Position {
    float x, xTrg;
    float y, yTrg;

    float l, lTrg; //slider
  } Position;

  Position position{};

private:
  std::string title;
  unsigned char maxLength;
  unsigned char min;
  unsigned char max;
  unsigned char step;

  bool valueOverflow;

  unsigned char lengthIndicator;

public:
  Slider(const std::string &_title,
         unsigned char _min,
         unsigned char _max,
         unsigned char _step,
         unsigned char &_value);  //slider.

public:
  unsigned char add();
  unsigned char sub();

public:
  void init() override;
  void deInit() override;

public:
  void renderIndicator(float _x, float _y, const std::vector<float> &_camera) override;

public:
  void render(const std::vector<float> &_camera) override;
};
}


#endif //ASTRA_WIDGET__H