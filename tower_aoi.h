#pragma once

#include <vector>
#include <mutex>
#include "tower.h"

class Tower;
class TowerAoi
{
public:
  TowerAoi(int32_t id);
  virtual ~TowerAoi();

  /**
    将游戏坐标系按灯塔视野分割并构建灯塔AOI游戏坐标系
  */
  void initialize(const Point& leftBottom, int32_t width, int32_t height, int32_t radius);

  /**
    对象进入灯塔系统(输入坐标为游戏坐标)
  */
  bool enter(std::shared_ptr<Object> obj, const Point &pt);

  /**
    对象离开灯塔系统(输入坐标为游戏坐标)
  */
  bool leave(std::shared_ptr<Object> obj, const Point &pt);

  /**
    更新灯塔系统中的对象(输入坐标为游戏坐标)
  */
  bool update(std::shared_ptr<Object> obj, const Point &old, const Point &cur);

  /**
    获取灯塔列表
  */
  const std::vector<std::shared_ptr<Tower>>& towers();

  /**
    根据游戏坐标计算所属灯塔
  */
  std::shared_ptr<Tower> coordinate2tower(const Point &pt);

protected:
  /**
    根据灯塔AOI坐标计算所属灯塔
  */
  std::shared_ptr<Tower> _coordinate2tower(const Point &pt);

  /**
    判断点是否在区域内(输入坐标为游戏坐标，但返回的是灯塔坐标)
   */
  bool isInArea(const Point &pt, Point &leftTop, Point &rightBottom);

  /**
    将游戏坐标视野区域转换为灯塔坐标视野区域(红色方块转换为兰色方块)
  */
  void visibleTransform(const Point &pt, int32_t range, Point &leftBottom, Point &rightTop);

protected:
  int32_t id_; // AOI编号
  int32_t radius_; // 对应游戏地图坐标系的灯塔半径(用于坐标系转换)
  Point leftBottom_; // 游戏地图坐标系左下角的原点坐标(用于坐标系转换)
  int32_t width_; // 灯塔坐标系的灯塔地图宽度
  int32_t height_; // 灯塔坐标系的灯塔地图高度
  std::mutex mutex_; // 灯塔AOI更新锁
  std::vector<std::shared_ptr<Tower>> towers_; // 灯塔AOI坐标系统

};
