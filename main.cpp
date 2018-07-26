#include <ctime>
#include <iostream>
#include <QApplication>
#include "npc.h"
#include "pc.h"
#include "widget.h"

/**
    每个虚线小方块代表一个游戏坐标

    每个实线大方块代表一个灯塔坐标

    红色大方块为玩家视野

    兰色方块为存在观察者的灯塔视野

    被玩家观察到的对象为红色，未被观察到的对象为绿色
*/

int32_t TILE_SIZE = 15; // 游戏地图瓦片大小,一个瓦片对应一个游戏坐标(单位：像素)
int32_t NPC_NUMS = 100; // 随机生成的NPC个数
int32_t WIDTH = 100; // 游戏地图宽度
int32_t HEIGHT = 70; // 游戏地图高度
int32_t TIME_INTERVAL = 200; //定时器间隔时间
int32_t TOWER_RADIUS = 5; // 灯塔AOI半径
int32_t PLAYER_RANGE = 10; // 玩家视野半径
int32_t PLAYER_ID = 2000; // 玩家对象ID，必须大于NPC_NUMS
bool DRAW_TOWER = true; // 是否绘制灯塔视野
bool NPC_RANDOM_MOVE = true; // NPC是否会随机移动

/**
    随机生成NPC和玩家
 */
void autogen(TowerAoi &aoi, std::vector<std::shared_ptr<Object>>& objs)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    std::shared_ptr<Object> obj = std::make_shared<PC>(PLAYER_ID, rand() % WIDTH, rand() % HEIGHT);
    aoi.enter(obj, Point(obj->x(), obj->y()));
    objs.push_back(obj);
    for (int i = 0; i < NPC_NUMS; ++i)
    {
        obj = std::make_shared<NPC>(i, rand() % WIDTH, rand() % HEIGHT);
        aoi.enter(obj, Point(obj->x(), obj->y()));
        objs.push_back(obj);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::vector<std::shared_ptr<Object>> objects;
    TowerAoi aoi(0);
    aoi.initialize(Point(0,0), WIDTH*TILE_SIZE, HEIGHT*TILE_SIZE, TOWER_RADIUS);
    autogen(aoi, objects);
    Widget w(nullptr, aoi, objects);
    w.show();
    return a.exec();
}
