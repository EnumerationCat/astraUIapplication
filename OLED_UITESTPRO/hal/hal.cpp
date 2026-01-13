//
// Created by Fir on 2024/2/8.
//

#include <cstring>
#include "hal.h"

HAL *HAL::hal = nullptr;

HAL *HAL::get() {
  return hal;
}

bool HAL::check() {
  return hal != nullptr;
}

bool HAL::inject(HAL *_hal) {
  if (_hal == nullptr) {
    return false;
  }

  _hal->init();
  hal = _hal;
  return true;
}

void HAL::destroy() {
  if (hal == nullptr) return;

  delete hal;
  hal = nullptr;
}

/**
 * @brief log printer. 自动换行的日志输出
 *
 * @param _msg message want to print. 要输出的信息
 * @note cannot execute within a loop. 不能在循环内执行
 */
void HAL::_printInfo(std::string _msg) {
  static std::vector<std::string> _infoCache = {};
  static const unsigned char _max = getSystemConfig().screenHeight / getFontHeight();
  static const unsigned char _fontHeight = getFontHeight();

  if (_infoCache.size() >= _max) _infoCache.clear();
  _infoCache.push_back(_msg);

  canvasClear();
  setDrawType(2); //反色显示
  for (unsigned char i = 0; i < _infoCache.size(); i++) {
    drawEnglish(0, _fontHeight + i * (1 + _fontHeight), _infoCache[i]);
  }
  canvasUpdate();
  setDrawType(1); //回归实色显示
}

bool HAL::_getAnyKey() {
  for (int i = 0; i < key::KEY_NUM; i++) {
    if (getKey(static_cast<key::KEY_INDEX>(i))) return true;
  }
  return false;
}

/**
 * @brief key scanner default. 默认按键扫描函数
 *
 * @note run per 5 ms.
 * @return key::keyValue
 */
void HAL::_keyScan() {
  static unsigned char _timeCnt = 0;
  static bool _lock = false;
  static key::KEY_FILTER _keyFilter = key::CHECKING;
  //标记是否进入长按持续状态（用于输出HOLD）
  static bool _isLongPressSustained = false;
  //是否支持持续长按
  static const unsigned char HOLD_ON_OFF = 1;
  // 标记PRESS状态是否已被处理，解决状态残留
  static bool _isPressHandled = false;

  switch (_keyFilter) {
    case key::CHECKING:
      // 重置所有状态变量（包括新增的标记）
      _timeCnt = 0;
      _lock = false;
      _isLongPressSustained = false;
      _isPressHandled = false; // 重置PRESS处理标记
      // 清空按键状态，避免残留
      key[key::KEY_0] = key::INVALID;
      key[key::KEY_1] = key::INVALID;
      if (getAnyKey()) {
        if (getKey(key::KEY_0)) _keyFilter = key::KEY_0_CONFIRM;
        if (getKey(key::KEY_1)) _keyFilter = key::KEY_1_CONFIRM;
      }
      break;

    case key::KEY_0_CONFIRM:
    case key::KEY_1_CONFIRM:
      //filter
      if (getAnyKey()) {
        if (!_lock) _lock = true;
        _timeCnt++;

        //timer：原有长按首次触发逻辑（1秒触发PRESS）
        if (_timeCnt > 100) {
          keyFlag = key::KEY_PRESSED;
          if (!_isLongPressSustained) {
            // 首次长按：输出PRESS，标记进入长按持续状态
            if (getKey(key::KEY_0)) {
              key[key::KEY_0] = key::PRESS;
              key[key::KEY_1] = key::INVALID;
            }
            if (getKey(key::KEY_1)) {
              key[key::KEY_1] = key::PRESS;
              key[key::KEY_0] = key::INVALID;
            }
            _isLongPressSustained = true;
            _isPressHandled = false; // 标记PRESS未被处理
            // 取消原有跳转到RELEASED的逻辑，保留在当前状态以输出HOLD
            // 注释掉：_timeCnt = 0; _lock = false; _keyFilter = key::RELEASED;
          } else {
            // 持续长按：先清空PRESS状态（如果未处理），再判断是否输出HOLD
            if (!_isPressHandled) {
              // PRESS状态仅生效一次，之后置为INVALID
              if (key[key::KEY_0] == key::PRESS) key[key::KEY_0] = key::INVALID;
              if (key[key::KEY_1] == key::PRESS) key[key::KEY_1] = key::INVALID;
              _isPressHandled = true;
            }

            if (HOLD_ON_OFF) {
              keyFlag = key::KEY_PRESSED;
              if (getKey(key::KEY_0)) {
                key[key::KEY_0] = key::HOLD;
                key[key::KEY_1] = key::INVALID;
              }
              if (getKey(key::KEY_1)) {
                key[key::KEY_1] = key::HOLD;
                key[key::KEY_0] = key::INVALID;
              }

            } else {
              if (key[key::KEY_0] == key::HOLD) key[key::KEY_0] = key::INVALID;
              if (key[key::KEY_1] == key::HOLD) key[key::KEY_1] = key::INVALID;
            }
          }
        } else {
          // 未达到长按阈值时，清空所有非CLICK状态（避免残留）
          if (key[key::KEY_0] != key::INVALID && key[key::KEY_0] != key::CLICK) {
            key[key::KEY_0] = key::INVALID;
          }
          if (key[key::KEY_1] != key::INVALID && key[key::KEY_1] != key::CLICK) {
            key[key::KEY_1] = key::INVALID;
          }
        }
      } else {
        // 按键释放：按原有逻辑处理
        if (_lock) {
          // 短按：未进入长按持续状态时输出CLICK
          if (!_isLongPressSustained) {
            if (_keyFilter == key::KEY_0_CONFIRM) {
              key[key::KEY_0] = key::CLICK;
              key[key::KEY_1] = key::INVALID;
            }
            if (_keyFilter == key::KEY_1_CONFIRM) {
              key[key::KEY_1] = key::CLICK;
              key[key::KEY_0] = key::INVALID;
            }
            keyFlag = key::KEY_PRESSED;
          } else {
            // 长按释放：清空所有按键状态
            key[key::KEY_0] = key::INVALID;
            key[key::KEY_1] = key::INVALID;
          }
          // 无论短按/长按，释放后都跳转到RELEASED
          _keyFilter = key::RELEASED;
        } else {
          _keyFilter = key::CHECKING;
          key[key::KEY_0] = key::INVALID;
          key[key::KEY_1] = key::INVALID;
        }
        // 重置所有标记
        _isLongPressSustained = false;
        _isPressHandled = false;
      }
      break;

    case key::RELEASED:
      if (!getAnyKey()) _keyFilter = key::CHECKING;
      break;

    default: break;
  }
}

/**
 * @brief default key tester. 默认按键测试函数
 */
void HAL::_keyTest() {
  if (getAnyKey()) {
    for (uint8_t i = 0; i < key::KEY_NUM; i++) {
      if (key[i] == key::CLICK) {
        //do something when key clicked
        if (i == 0) break;
        if (i == 1) break;
      } else if (key[i] == key::PRESS) {
        //do something when key pressed
        if (i == 0) break;
        if (i == 1) break;
      }
    }
    memset(key, key::INVALID, sizeof(key));
  }
}


