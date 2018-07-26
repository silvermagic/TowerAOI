#pragma once

#include <cstdint>
#include <string>

/**
    朝向
*/
enum Towards
{
  SOUTH = 0,
  SOUTH_EAST = 1,
  EAST = 2,
  NORTH_EAST = 3,
  NORTH = 4,
  NORTH_WEST = 5,
  WEST = 6,
  SOUTH_WEST = 7
};

class Point
{
public:
  Point();
  Point(int32_t x, int32_t y);
  virtual ~Point();

  /**
    等值运算符重载
   */
  bool operator==(const Point& pt);
  bool operator!=(const Point& pt);

  /**
    获取/设置坐标的X轴坐标
   */
  int32_t x() const;
  void setx(int32_t value);

  /**
    获取/设置坐标的Y轴坐标
   */
  int32_t y() const;
  void sety(int32_t value);

protected:
  int32_t x_;
  int32_t y_;
};
