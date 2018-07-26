#pragma once

#include <set>
#include <map>
#include "object.h"

class ObjectCompare
{
public:
    bool operator()(const std::shared_ptr<Object> &lhs, const std::shared_ptr<Object> &rhs) const
    {
        return lhs->id() < rhs->id();
    }
};

class Tower
{
public:
  Tower(const Point& pt);
  virtual ~Tower();

  /**
    对象进入灯塔监控范围
  */
  void enter(std::shared_ptr<Object> obj);

  /**
    对象离开灯塔监控范围
  */
  void leave(std::shared_ptr<Object> obj);

  /**
    设置对象为灯塔的观察者
  */
  void watch(std::shared_ptr<Object> obj);

  /**
    取消对象为灯塔观察者的设置
  */
  void unwatch(std::shared_ptr<Object> obj);

  /**
    获取灯塔监控范围内的全部/指定类型(可以是多种类型的组合)对象
  */
  std::set<std::shared_ptr<Object>, ObjectCompare>& objects();
  void objects(std::set<std::shared_ptr<Object>, ObjectCompare>& objs, int32_t type);

  /**
    获取绑定到灯塔的全部/指定类型(可以是多种类型的组合)的观察者对象
  */
  std::set<std::shared_ptr<Object>, ObjectCompare>& watchers();
  void watchers(std::set<std::shared_ptr<Object>, ObjectCompare>& objs, int32_t type);

  /**
    属性存取
  */
  Point coordinate() const;

protected:
  Point coordinate_; // 灯塔AOI坐标系中灯塔的坐标
  std::set<std::shared_ptr<Object>, ObjectCompare> objects_; // 灯塔监控范围内的对象
  std::map<int32_t, std::set<std::shared_ptr<Object>, ObjectCompare>> typeObjects_; // 将灯塔监控范围内的对象按类型分类
  std::set<std::shared_ptr<Object>, ObjectCompare> watchers_; // 灯塔绑定的观察者对象
  std::map<int32_t, std::set<std::shared_ptr<Object>, ObjectCompare>> typeWatchers_; // 将灯塔绑定的观察者对象按类型分类
};
