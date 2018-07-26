#include <cmath>
#include <iostream>
#include "tower_aoi.h"

TowerAoi::TowerAoi(int32_t id) :
    id_(id),
    mutex_(),
    towers_()
{
}

TowerAoi::~TowerAoi()
{
    towers_.clear();
}

void TowerAoi::initialize(const Point& leftBottom, int width, int height, int radius)
{
  // 灯塔视野半径(包含几个坐标点)校验
  radius_ = radius > 1 ? radius : 1;
  leftBottom_ = leftBottom;
  // 灯塔正方形视野的边长(前后视野半径包含的坐标点 + 自身坐标点)
  int32_t edge = 2 * radius_ + 1;
  // 根据游戏地图坐标系的长宽(多少个坐标点)来计算灯塔AOI坐标系的长宽(多少个坐标点)
  width_ = static_cast<int>(std::ceil(static_cast<float>(width) / edge));
  height_ = static_cast<int>(std::ceil(static_cast<float>(height) / edge));
  towers_.reserve(static_cast<size_t>(width_ * height_));
  // 将游戏地图坐标映射为灯塔坐标
  for (int y = 0; y < height_; ++y)
  {
    for (int x = 0; x < width_; ++x)
    {
      towers_.emplace_back(std::make_shared<Tower>(Point(x, y)));
    }
  }
}

bool TowerAoi::enter(std::shared_ptr<Object> obj, const Point &pt)
{
  std::unique_lock<std::mutex> guard(mutex_);
  std::shared_ptr<Tower> pTower = coordinate2tower(pt);
  // 对象进入灯塔的监控范围
  pTower->enter(obj);
  // 通知灯塔的观察者
  auto& watchers = pTower->watchers();
  for (auto iter = watchers.begin(); iter != watchers.end(); )
  {
    if ((*iter)->destroy())
    {
      iter = watchers.erase(iter);
      continue;
    }
    // 通知观察者iter更新可见对象列表
    if (obj->id() != (*iter)->id())
      (*iter)->onNotify(obj, true);
    ++iter;
  }

  // 如果对象有视野，则绑定到视野内的灯塔
  int32_t range = obj->range();
  if (range > 0)
  {
    Point leftBottom, rightTop;
    // 首先计算出对象视野范围内的灯塔
    visibleTransform(pt, range, leftBottom, rightTop);
    for (int32_t x = leftBottom.x(); x <= rightTop.x(); ++x)
    {
      for (int32_t y = leftBottom.y(); y <= rightTop.y(); ++y)
      {
        pTower = _coordinate2tower(Point(x, y));
        // 绑定对象为灯塔的观察者
        pTower->watch(obj);
        // 将灯塔范围内的现有对象发送给新的观察者
        auto& objects = pTower->objects();
        for (auto iter = objects.begin(); iter != objects.end(); )
        {
          if ((*iter)->destroy())
          {
            iter = objects.erase(iter);
            continue;
          }
          if (obj->id() != (*iter)->id())
          {
            // 通知观察者obj更新可见对象列表
            obj->onNotify(*iter, true);
            // 提示对象iter被关注了
            (*iter)->onWatch(obj, true);
          }
          ++iter;
        }
      }
    }
  }

  return true;
}

bool TowerAoi::leave(std::shared_ptr<Object> obj, const Point &pt)
{
  std::unique_lock<std::mutex> guard(mutex_);
  std::shared_ptr<Tower> pTower = coordinate2tower(pt);
  // 对象离开灯塔的监控范围
  pTower->leave(obj);
  // 通知灯塔的观察者
  auto& watchers = pTower->watchers();
  for (auto iter = watchers.begin(); iter != watchers.end(); )
  {
    if ((*iter)->destroy())
    {
      iter = watchers.erase(iter);
      continue;
    }
    if (obj->id() != (*iter)->id())
      (*iter)->onNotify(obj, false);
    ++iter;
  }

  // 如果对象有视野，则取消之前设置的观察者
  int32_t range = obj->range();
  if (range > 0)
  {
    Point leftBottom, rightTop;
    // 首先计算出对象视野范围内的灯塔
    visibleTransform(pt, range, leftBottom, rightTop);
    for (int32_t x = leftBottom.x(); x <= rightTop.x(); ++x)
    {
      for (int32_t y = leftBottom.y(); y <= rightTop.y(); ++y)
      {
        pTower = _coordinate2tower(Point(x, y));
        // 取消观察者绑定
        pTower->unwatch(obj);
        // 通知灯塔范围内的现有对象
        auto& objects = pTower->objects();
        for (auto iter = objects.begin(); iter != objects.end(); )
        {
          if ((*iter)->destroy())
          {
            iter = objects.erase(iter);
            continue;
          }
          if (obj->id() != (*iter)->id())
          {
            // 通知观察者obj更新可见对象列表
            obj->onNotify(*iter, false);
            // 提示对象iter被取消关注了
            (*iter)->onWatch(obj, false);
          }
          ++iter;
        }
      }
    }
  }

  return true;
}

bool TowerAoi::update(std::shared_ptr<Object> obj, const Point &old, const Point &cur)
{
  std::unique_lock<std::mutex> guard(mutex_);
  std::shared_ptr<Tower> pOldTower = coordinate2tower(old);
  std::shared_ptr<Tower> pCurTower = coordinate2tower(cur);
  // 对象所属灯塔发生了变化
  if (pOldTower->coordinate() != pCurTower->coordinate())
  {
    // 从旧灯塔移除
    {
      pOldTower->leave(obj);
      auto& watchers = pOldTower->watchers();
      for (auto iter = watchers.begin(); iter != watchers.end(); )
      {
        if ((*iter)->destroy())
        {
          iter = watchers.erase(iter);
          continue;
        }
        // 通知观察者iter更新可见对象列表
        if (obj->id() != (*iter)->id())
          (*iter)->onNotify(obj, false);
        ++iter;
      }
    }

    // 添加到新灯塔
    {
      pCurTower->enter(obj);
      auto& watchers = pCurTower->watchers();
      for (auto iter = watchers.begin(); iter != watchers.end(); )
      {
        if ((*iter)->destroy())
        {
          iter = watchers.erase(iter);
          continue;
        }
        // 通知观察者iter更新可见对象列表
        if (obj->id() != (*iter)->id())
          (*iter)->onNotify(obj, true);
        ++iter;
      }
    }
  }

  // 如果对象有视野，则还需要检测下视野内的灯塔是否发生变化
  int32_t range = obj->range();
  if (range > 0)
  {
    Point oldLeftBottom, oldRightTop;
    visibleTransform(old, range, oldLeftBottom, oldRightTop);
    Point curLeftBottom, curRightTop;
    visibleTransform(cur, range, curLeftBottom, curRightTop);
    // 视野内的灯塔发生了变化
    if (oldLeftBottom != curLeftBottom || oldRightTop != curRightTop)
    {
      // 在旧的视野范围内但是不在新的视野范围的灯塔要取消观察者设置
      for (int32_t x = oldLeftBottom.x(); x <= oldRightTop.x(); ++x)
      {
        for (int32_t y = oldLeftBottom.y(); y <= oldRightTop.y(); ++y)
        {
          if (!isInArea(Point(x, y), curLeftBottom, curRightTop))
          {
            std::shared_ptr<Tower> pTower = _coordinate2tower(Point(x, y));
            // 取消观察者绑定
            pTower->unwatch(obj);
            // 通知灯塔范围内的现有对象
            auto& objects = pTower->objects();
            for (auto iter = objects.begin(); iter != objects.end(); )
            {
              if ((*iter)->destroy())
              {
                iter = objects.erase(iter);
                continue;
              }
              if (obj->id() != (*iter)->id())
              {
                // 通知观察者obj更新可见对象列表
                obj->onNotify(*iter, false);
                // 提示对象iter被取消关注了
                (*iter)->onWatch(obj, false);
              }
              ++iter;
            }
          }
        }
      }

      // 在新的视野范围内但是不在旧的视野范围的灯塔要设置观察者
      for (int32_t x = curLeftBottom.x(); x <= curRightTop.x(); x++)
      {
        for (int32_t y = curLeftBottom.y(); y <= curRightTop.y(); y++)
        {
          if (!isInArea(Point(x, y), oldLeftBottom, oldRightTop))
          {
            std::shared_ptr<Tower> pTower = _coordinate2tower(Point(x, y));
            // 绑定对象为灯塔的观察者
            pTower->watch(obj);
            // 将灯塔范围内的现有对象发送给新的观察者
            auto& objects = pTower->objects();
            for (auto iter = objects.begin(); iter != objects.end(); )
            {
              if ((*iter)->destroy())
              {
                iter = objects.erase(iter);
                continue;
              }
              if (obj->id() != (*iter)->id())
              {
                // 通知观察者obj更新可见对象列表
                obj->onNotify(*iter, true);
                // 提示对象iter被关注了
                (*iter)->onWatch(obj, true);
              }
              ++iter;
            }
          }
        }
      }
    }
  }

  return true;
}

const std::vector<std::shared_ptr<Tower>>& TowerAoi::towers()
{
  return towers_;
}

std::shared_ptr<Tower> TowerAoi::coordinate2tower(const Point &pt)
{
  int32_t diffX = pt.x() - leftBottom_.x();
  if (diffX < 0)
    diffX = 0;
  int32_t diffY = pt.y() - leftBottom_.y();
  if (diffY < 0)
    diffY = 0;

  // 和初始化构造灯塔AOI一样，用灯塔边长来计算灯塔坐标
  int32_t edge = 2 * radius_ + 1;
  int32_t x = static_cast<int>(std::floor(static_cast<float>(diffX) / edge));
  if (x >= width_)
    x = width_ - 1;

  int32_t y = static_cast<int>(std::floor(static_cast<float>(diffY) / edge));
  if (y >= height_)
    y = height_ - 1;

  return _coordinate2tower(Point(x, y));
}

std::shared_ptr<Tower> TowerAoi::_coordinate2tower(const Point &pt)
{
  // 坐标是二维的，但是灯塔是用一维数组保存的，所以需要转换下
  return towers_[static_cast<size_t>(pt.x() + pt.y() * width_)];
}

bool TowerAoi::isInArea(const Point &pt, Point &leftBottom, Point &rightTop)
{
  if (leftBottom.x() <= pt.x() && pt.x() <= rightTop.x() && leftBottom.y() <= pt.y() && pt.y() <= rightTop.y())
    return true;

  return false;
}

void TowerAoi::visibleTransform(const Point &pt, int32_t range, Point &leftBottom, Point &rightTop)
{
  Point pos;
  std::shared_ptr<Tower> pTower = nullptr;
  // 计算视野左下脚的灯塔坐标
  pos.setx(pt.x() - range);
  pos.sety(pt.y() - range);
  pTower = coordinate2tower(pos);
  leftBottom = pTower->coordinate();
  // 计算视野右上脚的灯塔坐标
  pos.setx(pt.x() + range);
  pos.sety(pt.y() + range);
  pTower = coordinate2tower(pos);
  rightTop = pTower->coordinate();
}
