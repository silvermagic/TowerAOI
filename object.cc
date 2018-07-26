#include <iostream>
#include "object.h"

Object::Object(long id, int32_t x, int32_t y):
    Point(x, y),
    id_(id),
    destroy_(false),
    note_(false),
    range_(-1)
{
}

Object::~Object()
{
}

void Object::onNotify(std::shared_ptr<Object> obj, bool enter)
{
    std::cout << "I [" << id_ << "] " << (enter ? "saw [" : "can't see [") << obj->id() << "] Located ("<< obj->x() + 1 <<", " << obj->y() + 1 << ")" << std::endl;
}

void Object::onWatch(std::shared_ptr<Object> obj, bool enter)
{
    std::cout << "Object [" << obj->id() << "]" << (enter ? "watch" : "unwatch") << " on me [" << id_ << "] Located ("<< obj->x() + 1 <<", " << obj->y() + 1 << ")" << std::endl;
}

long Object::id() const
{
    return id_;
}

bool Object::destroy()
{
  return destroy_;
}

bool Object::note()
{
    return note_;
}

void Object::note(bool value)
{
    note_ = value;
}

int32_t Object::range()
{
    return range_;
}

int32_t Object::type()
{
    return OBJECT_TYPE_UNKNOWN;
}
