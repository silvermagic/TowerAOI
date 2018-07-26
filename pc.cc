#include <iostream>
#include "pc.h"

extern int32_t PLAYER_RANGE; // 玩家视野半径

PC::PC(long id, int x, int y):
    Object(id, x, y)
{
    range_ = PLAYER_RANGE;
}

PC::~PC()
{

}

/**
  对象obj进入/离开自身视野范围
 */
void PC::onNotify(std::shared_ptr<Object> obj, bool enter)
{
    obj->note(enter);
}
