#pragma once

#include "object.h"

class PC : public Object
{
public:
  PC(long id, int x, int y);
  virtual ~PC();

  /**
    对象obj进入/离开自身视野范围
   */
  virtual void onNotify(std::shared_ptr<Object> obj, bool enter);

  int type() { return OBJECT_TYPE_PC; }
};

