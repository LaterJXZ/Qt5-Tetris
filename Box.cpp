#include "Box.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QtDebug>

OneBox::OneBox(const QColor &color):
    brushColor(color)
{

}

QRectF OneBox::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-10 - penWidth / 2,-10 - penWidth / 2,20 + penWidth, 20 + penWidth);
}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawPixmap(-10,-10,20,20,QPixmap(":/images/box.gif"));
    painter->setBrush(brushColor);
    QColor penColor = brushColor;
    penColor.setAlpha(20);
    painter->setPen(penColor);
    painter->drawRect(-10,-10,20,20);
}

QPainterPath OneBox::shape() const
{
    QPainterPath path;
    path.addRect(-9.5,-9.5,19,19);
    return path;
}

BoxGroup::BoxGroup()
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    oldTransform = transform();
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(moveOneStep()));
    currentShape = RandomShape;  
}

QRectF BoxGroup::boundingRect() const
{
    qreal penWidth = 1;
    return QRectF(-40 - penWidth / 2,-40 - penWidth / 2,80 + penWidth,80 + penWidth);
}

bool BoxGroup::isColliding()
{
    QList<QGraphicsItem *> itemList = childItems();
    for(QGraphicsItem *item : itemList){
        if(item->collidingItems().count() > 1)
            return true;
    }
    return false;
}

void BoxGroup::creatBox(const QPointF &point, BoxGroup::BoxShape shape)
{
    static const QColor colorTable[7] = {
        QColor(200,0,0,100),QColor(255,200,0,100),
        QColor(0,0,200,100),QColor(0,200,0,100),
        QColor(0,200,255,100),QColor(200,0,255,100),QColor(150,100,100,100)
    };
    int shapeID = shape;
    if(shape == RandomShape){
        shapeID = qrand() % 7;
    }
    QColor color = colorTable[shapeID];
    QList<OneBox *> list;
    setRotation(0);
    setTransform(oldTransform);
    for(int i = 0; i < 4; ++i){
        OneBox *temp = new OneBox(color);
        list<<temp;
        addToGroup(temp);
    }
    switch (shapeID) {
    case IShape:
        currentShape = IShape;
        list.at(0)->setPos(-30,-10);
        list.at(1)->setPos(-10,-10);
        list.at(2)->setPos(10,-10);
        list.at(3)->setPos(30,-10);
        break;
    case JShape:
        currentShape = JShape;
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case LShape:
        currentShape = LShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(-10,10);
        list.at(2)->setPos(-10,30);
        list.at(3)->setPos(10,30);
        break;
    case OShape:
        currentShape = OShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(-10,10);
        list.at(2)->setPos(10,-10);
        list.at(3)->setPos(10,10);
        break;
    case SShape:
        currentShape = SShape;
        list.at(0)->setPos(10,-10);
        list.at(1)->setPos(30,-10);
        list.at(2)->setPos(-10,10);
        list.at(3)->setPos(10,10);
        break;
    case TShape:
        currentShape = TShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(30,-10);
        list.at(3)->setPos(10,10);
        break;
    case ZShape:
        currentShape = ZShape;
        list.at(0)->setPos(-10,-10);
        list.at(1)->setPos(10,-10);
        list.at(2)->setPos(10,10);
        list.at(3)->setPos(30,10);
        break;
    default:
        break;
    }
    setPos(point);
    if(isColliding()){
        stopTimer();
        emit gameFinished();
    }
}

void BoxGroup::clearBoxGroup(bool destoryBox)
{
    QList<QGraphicsItem *> itemList = childItems();
    for(QGraphicsItem *item : itemList){
        removeFromGroup(item);
        if(destoryBox){
            OneBox *box = (OneBox *)item;
            box->deleteLater();
        }
    }
}

void BoxGroup::keyPressEvent(QKeyEvent *event)
{
    static qreal angle = 0;
    switch (event->key())
    {
    case Qt::Key_Down:
        moveBy(0,20);
        if(isColliding()){
            moveBy(0,-20);
            clearBoxGroup();
            emit needNewBox();
        }
        break;
    case Qt::Key_Left:
        moveBy(-20,0);
        if(isColliding())
            moveBy(20,0);
        break;
    case Qt::Key_Right:
        moveBy(20,0);
        if(isColliding())
            moveBy(-20,0);
        break;
    case Qt::Key_Up:
        setRotation(angle + 90);
        angle += 90;
        if(isColliding()){
            setRotation(angle - 90);
            angle -= 90;
        }
        break;
    case Qt::Key_Space:
        moveBy(0,20);
        while(!isColliding())
            moveBy(0,20);
        moveBy(0,-20);
        clearBoxGroup();
        emit needNewBox();
        break;
    }
}

void BoxGroup::moveOneStep()
{
    moveBy(0,20);
    if(isColliding()){
        moveBy(0,-20);
        clearBoxGroup();
        emit needNewBox();
    }
}

void BoxGroup::startTimer(int interval)
{
    timer->start(interval);
}

void BoxGroup::stopTimer()
{
    timer->stop();
}
