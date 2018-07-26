#include "tower.h"

Tower::Tower(const Point& pt):
  coordinate_(pt),
  objects_(),
  typeObjects_(),
  watchers_(),
  typeWatchers_()
{
}

Tower::~Tower()
{
  objects_.clear();
  typeObjects_.clear();
  watchers_.clear();
  typeWatchers_.clear();
}

void Tower::enter(std::shared_ptr<Object> obj)
{
  objects_.insert(obj);
  typeObjects_[obj->type()].insert(obj);
}

void Tower::leave(std::shared_ptr<Object> obj)
{
  objects_.erase(obj);
  auto iter = typeObjects_.find(obj->type());
  if (iter != typeObjects_.end())
    iter->second.erase(obj);
}

void Tower::watch(std::shared_ptr<Object> obj)
{
    watchers_.insert(obj);
    typeWatchers_[obj->type()].insert(obj);
}

void Tower::unwatch(std::shared_ptr<Object> obj)
{
    watchers_.erase(obj);
    auto iter = typeWatchers_.find(obj->type());
    if (iter != typeWatchers_.end())
        iter->second.erase(obj);
}

std::set<std::shared_ptr<Object>, ObjectCompare>& Tower::objects()
{
  return objects_;
}

void Tower::objects(std::set<std::shared_ptr<Object>, ObjectCompare>& objs, int32_t type)
{
  for (auto& kv : typeObjects_)
  {
    if ((kv.first & type) == 0)
      continue;

    objs.insert(kv.second.begin(), kv.second.end());
  }
}

std::set<std::shared_ptr<Object>, ObjectCompare>& Tower::watchers()
{
  return watchers_;
}

void Tower::watchers(std::set<std::shared_ptr<Object>, ObjectCompare>& objs , int32_t type)
{
  for (auto& kv : typeWatchers_)
  {
    if ((kv.first & type) == 0)
      continue;

    objs.insert(kv.second.begin(), kv.second.end());
  }
}

Point Tower::coordinate() const
{
    return coordinate_;
}

