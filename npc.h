#pragma once

#include "object.h"

class NPC : public Object
{
public:
  NPC(long id, int x, int y);
  virtual ~NPC();

  /**
    自己被对象obj关注/取关了
   */
  virtual void onWatch(std::shared_ptr<Object> obj, bool enter);

  int type() { return OBJECT_TYPE_NPC; }
};
