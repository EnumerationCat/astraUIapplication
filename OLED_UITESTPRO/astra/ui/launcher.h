//
// Created by Fir on 2024/2/2.
//
#pragma once
#ifndef ASTRA_UI_LAUNCHER_H_
#define ASTRA_UI_LAUNCHER_H_
#include "config.h"
#include "menu.h"
#include "selector.h"
#include "camera.h"
#include "astra_rocket.h"
#include "Setting_app.h"

namespace astra {

class Launcher {


private:
  Menu* currentMenu;
  Widget* currentWidget = nullptr;
  Selector* selector;
  Camera* camera;

  uint64_t time;


public:


  void popInfo(std::string _info, uint16_t _time);

  void init(Menu* _rootPage);

  bool open();
  bool close();

  void update();

  Camera* getCamera() { return camera; }
  Selector* getSelector() { return selector; }
};
}

#endif //ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_