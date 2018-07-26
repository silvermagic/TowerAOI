#include <iostream>
#include <QPainter>
#include <QKeyEvent>
#include "widget.h"

extern int32_t TILE_SIZE;
extern int32_t WIDTH;
extern int32_t HEIGHT;
extern int32_t TIME_INTERVAL;
extern int32_t PLAYER_RANGE;
extern int32_t PLAYER_ID;
extern int32_t TOWER_RADIUS;
extern bool DRAW_TOWER;
extern bool NPC_RANDOM_MOVE;

Widget::Widget(QWidget *parent, TowerAoi &aoi, std::vector<std::shared_ptr<Object>>& objects)
    : QWidget(parent),
      bg_(WIDTH*TILE_SIZE, HEIGHT*TILE_SIZE),
      aoi_(aoi),
      objects_(objects)
{
    // 设置窗口大小
    setFixedSize(bg_.size());

    // 绘制背景
    QPainter painter(&bg_);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
    // 稍微解释下画图逻辑：虽然游戏坐标是点，是没有大小的，但是实际上客户端是用小正方表示的，大小为TILE_SIZE，玩家每移动一个坐标就是移动一个小方块，所以不会存在在两个小方块中间的位置
    for (int i = 1; i < WIDTH; i++)
        painter.drawLine(i * TILE_SIZE, 0, i * TILE_SIZE, HEIGHT*TILE_SIZE);
    for (int j = 1; j < HEIGHT; j++)
        painter.drawLine(0, j * TILE_SIZE, WIDTH*TILE_SIZE, j * TILE_SIZE);
    if (DRAW_TOWER)
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::darkYellow);
        int32_t edge = 2 * TOWER_RADIUS + 1;
        const std::vector<std::shared_ptr<Tower>>& towers = aoi.towers();
        for (auto tower : towers)
        {
            Point pt = tower->coordinate();
            // 屏幕画矩形是从左上往右下画的，所以(0,0)的灯塔的左上坐标应该为(0, HEIGHT - edge)
            painter.drawRect(pt.x() * edge * TILE_SIZE, (HEIGHT - (pt.y() + 1) * edge) * TILE_SIZE, edge * TILE_SIZE, edge * TILE_SIZE);
        }
    }

    // 启动定时器
    gameTimer_=new QTimer(this);
    connect(gameTimer_,SIGNAL(timeout()),this,SLOT(refresh()));
    gameTimer_->start(TIME_INTERVAL);
}

Widget::~Widget()
{
}

void Widget::paintEvent(QPaintEvent *)
{
    // 绘制下一帧
    QPixmap canvas = bg_;
    for (auto obj : objects_)
    {
        if (obj->type() == OBJECT_TYPE_PC)
        {
            drawPlayer(*obj, canvas);
        }
        else
        {
            drawNpc(*obj, obj->note(), canvas);
        }
    }

    if (DRAW_TOWER)
    {
        QPainter painter(&canvas);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::cyan);
        const std::vector<std::shared_ptr<Tower>>& towers = aoi_.towers();
        for (auto tower : towers)
        {
            Point pt = tower->coordinate();
            int32_t edge = 2 * TOWER_RADIUS + 1;
            if (!tower->watchers().empty())
            {
                painter.drawRect(pt.x() * edge * TILE_SIZE, (HEIGHT - (pt.y() + 1) * edge) * TILE_SIZE, edge * TILE_SIZE, edge * TILE_SIZE);
            }
        }
    }

    // 刷新显示缓存
    QPainter painter(this);
    painter.drawPixmap(0, 0, canvas);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    std::shared_ptr<Object> player = objects_[0];
    Point cur;
    switch(event->key())
    {
    case Qt::Key_Up:
    {
        if (player->y() < HEIGHT - 1)
        {
            cur.sety(player->y() + 1);
        }
        else
        {
            cur.sety(HEIGHT - 1);
        }
        cur.setx(player->x());
        break;
    }
    case Qt::Key_Down:
    {
        if (player->y() > 0)
        {
            cur.sety(player->y() - 1);
        }
        else
        {
            cur.sety(0);
        }
        cur.setx(player->x());
        break;
    }
    case Qt::Key_Left:
    {
        if (player->x() > 0)
        {
            cur.setx(player->x() - 1);
        }
        else
        {
            cur.setx(0);
        }
        cur.sety(player->y());
        break;
    }
    case Qt::Key_Right:
    {
        if (player->x() < WIDTH - 1)
        {
            cur.setx(player->x() + 1);
        }
        else
        {
            cur.setx(WIDTH - 1);
        }
        cur.sety(player->y());
        break;
    }
    default:
        break;
    }

    aoi_.update(player, *player, cur);
    player->setx(cur.x());
    player->sety(cur.y());
}

void Widget::drawPlayer(const Point& pt, QPixmap &canvas)
{
    QPainter painter(&canvas);
    // 绘制玩家
    painter.setBrush(Qt::blue);
    Point drawPos = transform(pt);
    painter.drawRect(drawPos.x()*TILE_SIZE, drawPos.y()*TILE_SIZE, TILE_SIZE, TILE_SIZE);

    // 绘制玩家视野
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::red);
    Point leftBottom, rightTop;
    if (pt.x() > PLAYER_RANGE)
    {
        leftBottom.setx(pt.x() - PLAYER_RANGE);
    }
    else
    {
        leftBottom.setx(0);

    }

    if (pt.x() < WIDTH - PLAYER_RANGE)
    {
        rightTop.setx(pt.x() + PLAYER_RANGE);
    }
    else
    {
        rightTop.setx(WIDTH - 1);
    }

    if (pt.y() > PLAYER_RANGE)
    {
        leftBottom.sety(pt.y() - PLAYER_RANGE);
    }
    else
    {
        leftBottom.sety(0);

    }

    if (pt.y() < HEIGHT - PLAYER_RANGE)
    {
        rightTop.sety(pt.y() + PLAYER_RANGE);
    }
    else
    {
        rightTop.sety(HEIGHT - 1);
    }

    int32_t width = rightTop.x() - leftBottom.x() + 1;
    int32_t height = rightTop.y() - leftBottom.y() + 1;
    drawPos = transform(Point(leftBottom.x(), rightTop.y()));
    painter.drawRect(drawPos.x()*TILE_SIZE, drawPos.y()*TILE_SIZE, width*TILE_SIZE, height*TILE_SIZE);
}

void Widget::drawNpc(const Point& pt, bool note, QPixmap &canvas)
{
    QPainter painter(&canvas);
    // 进入玩家视野的NPC为红色，没进入玩家视野的NPC为绿色
    painter.setBrush(note ? Qt::red : Qt::green);
    Point drawPos = transform(pt);
    painter.drawRect(drawPos.x()*TILE_SIZE, drawPos.y()*TILE_SIZE, TILE_SIZE, TILE_SIZE);
}

Point Widget::random(const Point& pt)
{
    Point cur;
    int rnd = rand() % 4;
    switch(rnd)
    {
    case 0:
    {
        if (pt.y() < HEIGHT - 1)
        {
            cur.sety(pt.y() + 1);
        }
        else
        {
            cur.sety(HEIGHT - 1);
        }
        cur.setx(pt.x());
        break;
    }
    case 1:
    {
        if (pt.y() > 0)
        {
            cur.sety(pt.y() - 1);
        }
        else
        {
            cur.sety(0);
        }
        cur.setx(pt.x());
        break;
    }
    case 2:
    {
        if (pt.x() > 0)
        {
            cur.setx(pt.x() - 1);
        }
        else
        {
            cur.setx(0);
        }
        cur.sety(pt.y());
        break;
    }
    case 3:
    {
        if (pt.x() < WIDTH - 1)
        {
            cur.setx(pt.x() + 1);
        }
        else
        {
            cur.setx(WIDTH - 1);
        }
        cur.sety(pt.y());
        break;
    }
    default:
        break;
    }

    return cur;
}

Point Widget::transform(const Point&pt)
{
    // 游戏坐标和屏幕坐标Y轴要翻转下，由于画图矩形是从左上往右下的，所以还要-1，想象下0,0
    return std::move(Point(pt.x(), HEIGHT - pt.y() - 1));
}

void Widget::refresh()
{
    if (NPC_RANDOM_MOVE)
    {
        for (auto obj : objects_)
        {
            if (obj->id() != PLAYER_ID)
            {
                Point cur = random(*obj);
                aoi_.update(obj, *obj, cur);
                obj->setx(cur.x());
                obj->sety(cur.y());
            }
        }
    }
    update();
}
