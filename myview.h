#ifndef MYVIEW_H
#define MYVIEW_H
#include <QGraphicsView>

class BoxGroup;
class QMediaPlayer;

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = nullptr);

public slots:
    void startGame();
    void clearFullRows();   //消去满行
    void moveBox(); //消掉满行之后，上方的向下落
    void gameOver();
    void restartGame();
    void pauseGame();
    void finishGame();
    void returnGame();

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    BoxGroup *boxGroup;
    BoxGroup *nextBoxGroup;
    QGraphicsLineItem *topLine;
    QGraphicsLineItem *bottomLine;
    QGraphicsLineItem *leftLine;
    QGraphicsLineItem *rightLine;
    qreal gameSpeed;
    QList<int> rows;

    QGraphicsTextItem *gameScoreText;
    QGraphicsTextItem *gameLevelText;

    QGraphicsWidget *maskWidget;
    QGraphicsWidget *startButton;
    QGraphicsWidget *pauseButton;
    QGraphicsWidget *finishButton;
    QGraphicsWidget *restartButton;
    QGraphicsWidget *returnButton;
    QGraphicsWidget *optionButton;
    QGraphicsWidget *helpButton;
    QGraphicsWidget *exitButton;
    QGraphicsWidget *showMenuButton;
    QGraphicsTextItem *welcomeText;
    QGraphicsTextItem *pausedText;
    QGraphicsTextItem *gameOverText;

    QMediaPlayer *clearRowSound;
    QMediaPlayer *themeMusic;

    void initView();
    void initGame();
    void updateScore(const int fullRowNum = 0);
};

#endif // MYVIEW_H
