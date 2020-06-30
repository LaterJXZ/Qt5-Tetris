#ifndef ONEBOX_H
#define ONEBOX_H
#include <QGraphicsObject>
#include <QGraphicsItemGroup>

//单个小方格
class OneBox : public QGraphicsObject
{
public:
    OneBox(const QColor &color = Qt::red);
    QRectF boundingRect() const;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
    QPainterPath shape() const;

private:
    QColor brushColor;
};

//方格组，即一个游戏内的块儿
class BoxGroup : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    enum BoxShape{IShape,JShape,LShape,OShape,SShape,TShape,ZShape,RandomShape};    //方块的类型
    BoxGroup();
    QRectF boundingRect() const;
    bool isColliding(); //是否碰撞
    void creatBox(const QPointF &point = QPointF(0,0),BoxShape shape = RandomShape);
    void clearBoxGroup(bool destoryBox = false);    //将图形组加入场景之中
    BoxShape getCurrentShape(){return currentShape;}

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void needNewBox();  //需要一个新的方块信号
    void gameFinished();    //游戏结束信号

public slots:
    void moveOneStep(); //自动下移一格
    void startTimer(int interval);  //启动计时器
    void stopTimer();   //停止计时器

private:
    BoxShape currentShape;
    QTransform oldTransform;
    QTimer *timer;
};

#endif // ONEBOX_H
