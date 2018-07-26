#include <iostream>
#include "npc.h"

NPC::NPC(long id, int x, int y):
    Object(id, x, y)
{
}

NPC::~NPC()
{
}

/**
  自己被对象obj关注/取关了
*/
void NPC::onWatch(std::shared_ptr<Object> obj, bool enter)
{
    note(enter);
}
