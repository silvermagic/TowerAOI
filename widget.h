#ifndef WIDGET_H
#define WIDGET_H

#include <QTimer>
#include <QWidget>
#include "tower_aoi.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent, TowerAoi& aoi, std::vector<std::shared_ptr<Object>>& objects);
    ~Widget();

    virtual void paintEvent(QPaintEvent *); //界面刷新
    virtual void keyPressEvent(QKeyEvent *); //键盘监听

protected:
    void drawPlayer(const Point&, QPixmap &);
    void drawNpc(const Point&, bool, QPixmap &);
    Point random(const Point&);
    Point transform(const Point&);

private slots:
    void refresh();

protected:
    QTimer *gameTimer_;
    QPixmap bg_;
    TowerAoi& aoi_;
    std::vector<std::shared_ptr<Object>>& objects_;
};

#endif // WIDGET_H
