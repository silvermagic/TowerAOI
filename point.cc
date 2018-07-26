#include <cmath>
#include <algorithm>
#include "point.h"

Point::Point():
  x_(0),
  y_(0)
{
}

Point::Point(int32_t x, int32_t y):
  x_(x),
  y_(y)
{
}

Point::~Point()
{
}

bool Point::operator==(const Point& pt)
{
  return (pt.x() == x_ && pt.y() == y_);
}

bool Point::operator!=(const Point& pt)
{
    return (pt.x() != x_ || pt.y() != y_);
}

int32_t Point::x() const
{
  return x_;
}

void Point::setx(int32_t value)
{
  x_ = value;
}

int32_t Point::y() const
{
  return y_;
}

void Point::sety(int32_t value)
{
  y_ = value;
}
