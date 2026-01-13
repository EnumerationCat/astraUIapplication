//
// Created by Fir on 2024/2/2.
//

#include "launcher.h"

namespace astra {



  //弹窗
void Launcher::popInfo(std::string _info, uint16_t _time) {
  static bool init = false;
  static unsigned long long int beginTime = this->time;;
  static bool onRender = false;

  if (!init) {
    init = true;
    beginTime = this->time;
    onRender = true;
  }

  float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin;  //宽度
  float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin;  //高度
  float yPop = 0 - hPop - 8; //从屏幕上方滑入
  float yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;  //目标位置 中间偏上
  float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;  //居中

  while (onRender) {
    time++;


    HAL::canvasClear();
    Setting::Screen_Settig();
    /*渲染一帧*/
    currentMenu->render(camera->getPosition());
    selector->render(camera->getPosition());
    camera->update(currentMenu, selector);
    /*渲染一帧*/


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

    //这里条件可以加上一个如果按键按下 就滑出
    if (time - beginTime >= _time) yPopTrg = 0 - hPop - 8;  //滑出

    HAL::keyScan();
    if (HAL::getAnyKey()) {
      for (unsigned char i = 0; i < key::KEY_NUM; i++)
        if (HAL::getKeyMap()[i] == key::CLICK) yPopTrg = 0 - hPop - 8;  //滑出
      std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
    }

    if (yPop == 0 - hPop - 8) {
      onRender = false;  //退出条件
      init = false;
    }
  }
}

void Launcher::init(Menu *_rootPage) {
  currentMenu = _rootPage;

  camera = new Camera(0, 0);
  _rootPage->childPosInit(camera->getPosition());

  selector = new Selector();
  selector->inject(_rootPage);

  camera->init(_rootPage->getType());
}

/**
 * @brief 打开选中的页面
 *
 * @return 是否成功打开
 * @warning 仅可调用一次
 */
bool Launcher::open() {

  //如果当前页面指向的当前item没有后继 那就返回false
  if (currentMenu->getNextMenu() == nullptr) {
    popInfo("unreferenced page!", 600);
    return false;
  }
  // 修改判断条件：允许带有控件的菜单项即使没有子页面也能打开
  if (currentMenu->getNextMenu()->getItemNum() == 0 &&
      currentMenu->getNextMenu()->getType() != "SerialMonitor"&&
      currentMenu->getNextMenu()->getType() != "HardwareInfo"&&
      currentMenu->getNextMenu()->getType() != "Clock") {
    popInfo("empty page!", 500);
    return false;
      }


  currentMenu->rememberCameraPos(camera->getPositionTrg());

  currentMenu->deInit();  //先析构（退场动画）再挪动指针

  currentMenu = currentMenu->getNextMenu();
  currentMenu->forePosInit();
  currentMenu->childPosInit(camera->getPosition());

  // 若打开的是SerialMonitor，重置其打开动画状态
  if (currentMenu->getType() == "SerialMonitor") {
    auto* serialMonitor = dynamic_cast<SerialMonitor*>(currentMenu);
    serialMonitor->resetEnterAnimation();
  }
  if (currentMenu->getType() == "HardwareInfo") {
    auto* hardwareInfo = dynamic_cast<HardwareInfo*>(currentMenu);
    hardwareInfo->resetEnterAnimation();
  }
  if (currentMenu->getType() == "Clock") {
    auto* clock = dynamic_cast<Clock*>(currentMenu);
    clock->resetEnterAnimation();
  }

  selector->inject(currentMenu);
  //selector->go(currentPage->selectIndex);

  return true;
}

/**
 * @brief 关闭选中的页面
 *
 * @return 是否成功关闭
 * @warning 仅可调用一次
 */
bool Launcher::close() {
  if (currentMenu->getPreview() == nullptr) {
    popInfo("unreferenced page!", 100);
    return false;
  }
  // 修改判断条件：允许带有控件的菜单项即使没有子页面也能返回
  if (currentMenu->getPreview()->getItemNum() == 0 ) {
    popInfo("empty page!", 100);
    return false;
      }

  currentMenu->rememberCameraPos(camera->getPositionTrg());

  currentMenu->deInit();  //先析构（退场动画）再挪动指针

  currentMenu = currentMenu->getPreview();
  currentMenu->forePosInit();
  currentMenu->childPosInit(camera->getPosition());
  selector->inject(currentMenu);


  //selector->go(currentPage->selectIndex);

  return true;
}

void Launcher::update() {
  HAL::canvasClear();
  Setting::Screen_Settig();


  currentMenu->render(camera->getPosition());
  if (currentWidget != nullptr) currentWidget->render(camera->getPosition());
  selector->render(camera->getPosition());
  camera->update(currentMenu, selector);

//  if (time == 500) selector->go(3);  //test
//  if (time == 800) open();  //test
//  if (time == 1200) selector->go(0);  //test
//  if (time == 1500) selector->go(1z);  //test
//  if (time == 1800) selector->go(6);  //test
//  if (time == 2100) selector->go(1);  //test
//  if (time == 2300) selector->go(0);  //test
//  if (time == 2500) open();  //test
//  if (time == 2900) close();
//  if (time == 3200) selector->go(0);  //test
//  if (time >= 3250) time = 0;  //test

  if (time > 2) {
    HAL::keyScan();
    time = 0;
  }



  if (*HAL::getKeyFlag() == key::KEY_PRESSED) {
    *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;
    for (unsigned char i = 0; i < key::KEY_NUM; i++) {
      if (HAL::getKeyMap()[i] == key::CLICK) {
        if (i == 0) {
          widget_selector = Widget_Selector_NoSelect;
          selector->goPreview();

        }//selector去到上一个项目
        else if (i == 1) {
          widget_selector = Widget_Selector_NoSelect;
          selector->goNext();

        }//selector去到下一个项目
      } else if (HAL::getKeyMap()[i] == key::PRESS) {//长按
        if (i == 0) { widget_selector = Widget_Selector_NoSelect;close(); }//退出当前项目
        else if (i == 1) {

          if (currentMenu->getType() == "SerialMonitor"||currentMenu->getType() == "HardwareInfo") {
            popInfo("Error!",500);
            return;
          }
          if (currentMenu->getType() == "Clock") {
              switch (MyClock->getSelectedItem()){
                  case Clock::ALM:
                      MyClock->ClockPopup("ALMsetting");
                      break;
                  case Clock::SETTING:
                      MyClock->ClockPopup("Timesetting");
                      break;
                  case Clock::STW:
                      MyClock->ClockPopup("StopWatch");
                      break;
              }

                return;
           }


          if (currentMenu->getNextMenu()->getWidgetNum()>0&&currentMenu->getNextMenu()->title=="-Brightness") {

            BrightnessSetting->BrightnessPopup("Brightness",currentMenu,selector,camera);
            return;
          }


          if (currentMenu->getNextMenu()->getWidgetNum()>0&&currentMenu->getNextMenu()->title=="-RGBLed") {
            checkBox-> toggle(); // 调用 CheckBox 特有的 check() 方法
            return;
          }

          if (currentMenu->getNextMenu()->getWidgetNum()>0&&currentMenu->getNextMenu()->title=="-ScreenMode") {
            if ( widget_selector==Widget_Selector_Select) {
              ScreenPopUp->selectNext();
            }

            if (widget_selector == Widget_Selector_NoSelect) {
              widget_selector= Widget_Selector_Select;
            }
            return;
          }

          else{open();}

        }//打开当前项目
      }
    }
    std::fill(HAL::getKeyMap(), HAL::getKeyMap() + key::KEY_NUM, key::INVALID);
    *HAL::getKeyFlag() = key::KEY_NOT_PRESSED;

  }


  HAL::canvasUpdate();

  //time++;
  time = HAL::millis() / 1000;
}
}