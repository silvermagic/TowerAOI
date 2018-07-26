#pragma once

#include <memory>
#include "point.h"

const uint32_t OBJECT_TYPE_UNKNOWN                = 0x00000000;
const uint32_t OBJECT_TYPE_PC                     = 0x00000001;
const uint32_t OBJECT_TYPE_NPC                    = 0x00000002;
const uint32_t OBJECT_TYPE_PET                    = 0x00000004;

class Object : public Point
{
public:
  Object(long id, int32_t x, int32_t y);
  ~Object();

  /**
    对象obj进入/离开自身视野范围
   */
  virtual void onNotify(std::shared_ptr<Object> obj, bool enter);

  /**
    自己被对象obj关注/取关了
   */
  virtual void onWatch(std::shared_ptr<Object> obj, bool enter);

  /**
    属性存取
   */
  bool destroy();
  long id() const;
  bool note();
  void note(bool value);
  int32_t range();
  virtual int32_t type();

protected:
  long id_;
  bool destroy_;
  bool note_;
  int32_t range_;
};
