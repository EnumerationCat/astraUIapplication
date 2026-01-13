//
// Created by Fir on 2024/4/14 014.
//

#include "selector.h"

namespace astra {

  void Selector::setPosition() {
    if (menu->getType() == "Tile") {
      xTrg = menu->childMenu[menu->selectIndex]->position.xTrg - astraConfig.tileSelectBoxMargin;
      yTrg = menu->childMenu[menu->selectIndex]->position.yTrg - astraConfig.tileSelectBoxMargin;

      yText = systemConfig.screenHeight;
      yTextTrg = systemConfig.screenHeight - astraConfig.tileTextBottomMargin;

      wTrg = astraConfig.tileSelectBoxWidth;
      hTrg = astraConfig.tileSelectBoxHeight;
    } else if (menu->getType() == "List") {
      xTrg = menu->childMenu[menu->selectIndex]->position.xTrg - astraConfig.selectorMargin;
      yTrg = menu->childMenu[menu->selectIndex]->position.yTrg;

      wTrg = (float) HAL::getFontWidth(menu->childMenu[menu->selectIndex]->title) + astraConfig.listTextMargin * 2;
      hTrg = astraConfig.listLineHeight;
    }else if (menu->getType() == "SerialMonitor") {
      auto* serialMonitor = dynamic_cast<SerialMonitor*>(menu);
      if (serialMonitor->getRecvDataSize() == 0) return;
      // 关键修复：使用当前可视区域的起始行来计算显示索引
      int displayIndex = menu->selectIndex - serialMonitor->getBoundary()[0];
      // 确保显示索引在可视范围内
      displayIndex = std::clamp(displayIndex, 0, serialMonitor->maxDisplayLines - 1);
      // 计算选择框Y坐标（关键修正2：添加标题偏移+摄像机偏移）
      xTrg = serialMonitor->getBoxX() + 3;
      yTrg = serialMonitor->getBoxY() + 14.5 + displayIndex * serialMonitor->getDisplayLineHeight();  // +11px标题栏
      wTrg = systemConfig.screenWeight - 2 * (serialMonitor->getBoxX() + 4);
      hTrg = serialMonitor->getDisplayLineHeight();
    }
  }

/**
 * @brief
 *
 * @param _index
 * @note selector接管了移动选择指针的功能
 * @warning not support in loop. 不支持在循环内执行
 */
  void Selector::go(unsigned char _index) {
  Item::updateConfig();
  if (menu->getType() == "SerialMonitor") {
    auto* serialMonitor = dynamic_cast<SerialMonitor*>(menu);
    if (serialMonitor->getRecvDataSize() == 0) return;
    // 确保索引有效
    if (_index >= serialMonitor->getRecvDataSize()) {
      if (astraConfig.menuLoop) _index = 0;
      else return;
    } else if (_index < 0) {
      if (astraConfig.menuLoop) _index = serialMonitor->getRecvDataSize() - 1;
      else return;
    }
    serialMonitor->selectedLine = _index;
    menu->selectIndex = _index; // 保持兼容性
    setPosition();
    return;
  }

    // 原逻辑：非SerialMonitor类型基于childMenu校验
    if (_index > menu->childMenu.size() - 1) {
      if (astraConfig.menuLoop) _index = 0;
      else return;
    } else if (_index < 0) {
      if (astraConfig.menuLoop) _index = menu->childMenu.size() - 1;
      else return;
  }

  menu->selectIndex = _index;
  setPosition();
}

  void Selector::goNext() {
  if (menu->getType() == "SerialMonitor") {
    auto* serialMonitor = dynamic_cast<SerialMonitor*>(menu);
    if (serialMonitor->getRecvDataSize() == 0) return;
    // 选中行+1，边界：不超过数据总条数-1
    int nextIndex = menu->selectIndex + 1;
    if (nextIndex >= serialMonitor->getRecvDataSize()) {
      if (astraConfig.menuLoop) nextIndex = 0;
      else return;
    }
    go(nextIndex);
    return;
  }


  // 原逻辑：非SerialMonitor类型处理
  if (this->menu->selectIndex == this->menu->childMenu.size() - 1) {
    if (astraConfig.menuLoop) go(0);
    else return;
  } else go(menu->selectIndex + 1);
}


  void Selector::goPreview() {

  if (menu->getType() == "SerialMonitor") {
    auto* serialMonitor = dynamic_cast<SerialMonitor*>(menu);
    if (serialMonitor) {
      int dataSize = serialMonitor->getRecvDataSize();
      if (dataSize == 0) return;
      // 选中行-1，边界：不小于0
      int prevIndex = menu->selectIndex - 1;
      if (prevIndex < 0) {
        if (astraConfig.menuLoop) prevIndex = dataSize - 1;
        else return;
      }
      go(prevIndex);
      return;
    }
  }


  // 原逻辑：非SerialMonitor类型处理
  if (this->menu->selectIndex == 0) {
    if (astraConfig.menuLoop) go(this->menu->childMenu.size() - 1);
    else return;
  } else go(menu->selectIndex - 1);
}

bool Selector::inject(Menu *_menu) {
  if (_menu == nullptr) return false;

  //if (this->menu != nullptr) return false;
  this->menu = _menu;

  go(this->menu->selectIndex);  //注入之后要初始化选择框的位置

  return true;
}

bool Selector::destroy() {
  if (this->menu == nullptr) return false;

  delete this->menu;
  this->menu = nullptr;
}

void Selector::render(std::vector<float> _camera) {
  Item::updateConfig();

  //实际上 这里已经实现过渡动画了
  Animation::move(&x, xTrg, astraConfig.selectorXAnimationSpeed);
  Animation::move(&y, yTrg, astraConfig.selectorYAnimationSpeed);
  Animation::move(&h, hTrg, astraConfig.selectorHeightAnimationSpeed);
  Animation::move(&w, wTrg, astraConfig.selectorWidthAnimationSpeed);

  if (menu->getType() == "Tile") {
    Animation::move(&yText, yTextTrg, astraConfig.selectorYAnimationSpeed);

    //draw text.
    //文字不受摄像机的影响
    HAL::setDrawType(1);
    HAL::drawChinese((systemConfig.screenWeight -
                      (float) HAL::getFontWidth(menu->childMenu[menu->selectIndex]->title)) / 2.0,
                     yText + astraConfig.tileTitleHeight,
                     menu->childMenu[menu->selectIndex]->title);

    //draw box.
    //大框需要受摄像机的影响
    HAL::setDrawType(2);
    HAL::drawPixel(x + _camera[0], y + _camera[1]);
    //左上角
    HAL::drawHLine(x + _camera[0], y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);
    HAL::drawVLine(x + _camera[0], y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);
    //左下角
    HAL::drawHLine(x + _camera[0], y + _camera[1] + h - 1, astraConfig.tileSelectBoxLineLength + 1);
    HAL::drawVLine(x + _camera[0],
                   y + _camera[1] + h - astraConfig.tileSelectBoxLineLength - 1,
                   astraConfig.tileSelectBoxLineLength);
    //右上角
    HAL::drawHLine(x + _camera[0] + w - astraConfig.tileSelectBoxLineLength - 1,
                   y + _camera[1],
                   astraConfig.tileSelectBoxLineLength);
    HAL::drawVLine(x + _camera[0] + w - 1, y + _camera[1], astraConfig.tileSelectBoxLineLength + 1);
    //右下角
    HAL::drawHLine(x + _camera[0] + w - astraConfig.tileSelectBoxLineLength - 1,
                   y + _camera[1] + h - 1,
                   astraConfig.tileSelectBoxLineLength);
    HAL::drawVLine(x + _camera[0] + w - 1,
                   y + _camera[1] + h - astraConfig.tileSelectBoxLineLength - 1,
                   astraConfig.tileSelectBoxLineLength);

    HAL::drawPixel(x + _camera[0] + w - 1, y + _camera[1] + h - 1);
  } else if (menu->getType() == "List") {
    //animation(&h, hTrg, astraConfig.selectorAnimationSpeed);

    //draw select box.
    //受摄像机的影响
    HAL::setDrawType(2);
    HAL::drawRBox(x + _camera[0], y + _camera[1], w, h - 1, astraConfig.selectorRadius);
    //HAL::drawRBox(x, y, w, astraConfig.listLineHeight, astraConfig.selectorRadius);
    HAL::setDrawType(2);
  }else if (menu->getType() == "SerialMonitor") {
      auto* serialMonitor = dynamic_cast<astra::SerialMonitor*>(menu);

      if (serialMonitor->getRecvDataSize() == 0) return;

      int displayIndex = menu->selectIndex - serialMonitor->getBoundary()[0];
      displayIndex = std::clamp(displayIndex, 0, serialMonitor->maxDisplayLines -1);

      // 限制选择框在数据框内：左右边距各4px，高度与行高一致
      xTrg = serialMonitor->getBoxX() + 3;  // 左边界：数据框左+4px
      yTrg = serialMonitor->getBoxY() + 14.5 + (displayIndex-2) * serialMonitor->getDisplayLineHeight();  // 上边界：数据框内对应行
      wTrg = systemConfig.screenWeight - serialMonitor->getBoxX() - 8;  // 宽度：数据框宽度 - 左右边距（4*2）
      hTrg = serialMonitor->getDisplayLineHeight()-1;  // 高度：与行高一致

    Animation::move(&x, xTrg, astraConfig.selectorXAnimationSpeed);
    Animation::move(&y, yTrg, astraConfig.selectorYAnimationSpeed);
    Animation::move(&h, hTrg, astraConfig.selectorHeightAnimationSpeed);
    Animation::move(&w, wTrg, astraConfig.selectorWidthAnimationSpeed);

      // 绘制选择框（受摄像机偏移影响，确保在数据框内）
      HAL::setDrawType(2);
      HAL::drawRBox(x+ _camera[0], y , w, h, astraConfig.selectorRadius);
      HAL::setDrawType(1);

  }
}

std::vector<float> Selector::getPosition() {
  return {xTrg, yTrg};
}
}