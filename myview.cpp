#include "myview.h"
#include "Box.h"
#include <QIcon>
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QFileInfo>
#include <QMediaPlayer>

static const qreal INITSPEED = 500;

MyView::MyView(QWidget *parent):
    QGraphicsView(parent)
{
    initView();
}

void MyView::startGame()
{
    welcomeText->hide();
    startButton->hide();
    optionButton->hide();
    helpButton->hide();
    exitButton->hide();
    maskWidget->hide();

    themeMusic->play();
    initGame();
}

void MyView::clearFullRows()
{
    for(int y = 429; y > 50; y -= 20){
        QList<QGraphicsItem *> list = scene()->items(199,y,202,22,Qt::ContainsItemShape,Qt::DescendingOrder);
        if(list.count() == 10){
            for(QGraphicsItem * item : list){
                OneBox *box = (OneBox *)item;
                QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blurEffect);
                QPropertyAnimation *animation = new QPropertyAnimation(box,"scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(4);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                connect(animation,SIGNAL(finished()),box,SLOT(deleteLater()));
            }
            rows<<y;    //记录下当前满行的行数
        }
    }
    if(rows.count() > 0){
        clearRowSound->play();
        QTimer::singleShot(400,this,SLOT(moveBox()));
    }
    else{
        boxGroup->creatBox(QPointF(300,70),nextBoxGroup->getCurrentShape());
        //提示下一个方块的地方，需要全部销毁重新生成
        nextBoxGroup->clearBoxGroup(true);
        nextBoxGroup->creatBox(QPointF(500,70));
    }
}

void MyView::moveBox()
{
    for(int i = rows.count(); i > 0; --i){
        int row = rows.at(i - 1);
        for(QGraphicsItem *item : scene()->items(199,49,202,row - 47,Qt::ContainsItemShape,Qt::DescendingOrder)){
            item->moveBy(0,20);
        }
    }
    updateScore(rows.count());
    rows.clear();
    boxGroup->creatBox(QPointF(300,70),nextBoxGroup->getCurrentShape());
    nextBoxGroup->clearBoxGroup(true);
    nextBoxGroup->creatBox(QPointF(500,70));
}

void MyView::gameOver()
{
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gameOverText->show();
    restartButton->setPos(370,200);
    finishButton->show();
}

void MyView::restartGame()
{
    maskWidget->hide();
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600,150);

    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    for(QGraphicsItem *item : scene()->items(199,49,202,402,Qt::ContainsItemShape,Qt::DescendingOrder)){
        scene()->removeItem(item);
        OneBox *box = (OneBox *)item;
        box->deleteLater();
    }

    initGame();
}

void MyView::pauseGame()
{
    boxGroup->stopTimer();
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    returnButton->show();
    pausedText->show();
    themeMusic->pause();
}

void MyView::finishGame()
{
    themeMusic->stop();
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600,150);
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    gameScoreText->hide();
    gameLevelText->hide();
    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    for(QGraphicsItem *item : scene()->items(199,49,202,402,Qt::ContainsItemShape,Qt::DescendingOrder)){
        OneBox *box = (OneBox *)item;
        box->deleteLater();
    }

    maskWidget->show();
    welcomeText->show();
    startButton->show();
    optionButton->show();
    helpButton->show();
    exitButton->show();
    scene()->setBackgroundBrush(QPixmap(":/images/background.png"));
}

void MyView::returnGame()
{
    returnButton->hide();
    pausedText->hide();
    maskWidget->hide();
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    boxGroup->startTimer(gameSpeed);
    themeMusic->play();
}

void MyView::keyPressEvent(QKeyEvent *event)
{
    if(pauseButton->isVisible())
        boxGroup->setFocus();
    else
        boxGroup->clearFocus();
    QGraphicsView::keyPressEvent(event);
}

void MyView::initView()
{
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(CacheBackground);
    setWindowTitle(tr("俄罗斯方块"));
    setWindowIcon(QIcon(":/images/icon.png"));
    setMinimumSize(810,510);
    setMaximumSize(810,510);
    //场景
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5,5,800,500);
    scene->setBackgroundBrush(QPixmap(":/images/background.png"));
    setScene(scene);
    //边框线
    topLine = scene->addLine(197,47,403,47);
    bottomLine = scene->addLine(197,453,403,453);
    leftLine = scene->addLine(197,47,197,453);
    rightLine = scene->addLine(403,47,403,453);
    //当前方块和下一个
    boxGroup = new BoxGroup;
    connect(boxGroup,&BoxGroup::needNewBox,this,&MyView::clearFullRows);
    connect(boxGroup,&BoxGroup::gameFinished,this,&MyView::gameOver);
    scene->addItem(boxGroup);
    nextBoxGroup = new BoxGroup;
    scene->addItem(nextBoxGroup);
    //音乐音效
    themeMusic = new QMediaPlayer(this);
    themeMusic->setMedia(QUrl::fromLocalFile("../TetrisGame/sounds/theme.mp3"));
    clearRowSound = new QMediaPlayer(this);
    clearRowSound->setMedia(QUrl::fromLocalFile("../TetrisGame/sounds/clearRow.mp3"));

    gameScoreText = new QGraphicsTextItem("0");
    scene->addItem(gameScoreText);
    gameScoreText->setFont(QFont("Times",20,QFont::Bold));
    gameScoreText->setPos(650,350);
    gameLevelText = new QGraphicsTextItem("0");
    scene->addItem(gameLevelText);
    gameLevelText->setFont(QFont("Times",30,QFont::Bold));
    gameLevelText->setPos(20,150);

    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0,0,0,80)));
    mask->resize(900,600);
    maskWidget = scene->addWidget(mask);
    maskWidget->setPos(-50,-50);
    maskWidget->setZValue(1);

    QWidget *option = new QWidget;
    QPushButton *optionCloseButton = new QPushButton(tr("关闭"),option);
    QPalette palette;
    palette.setColor(QPalette::ButtonText,Qt::black);
    optionCloseButton->setPalette(palette);
    optionCloseButton->move(120,300);
    connect(optionCloseButton,SIGNAL(clicked()),option,SLOT(hide()));
    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0,0,0,180)));
    option->resize(300,400);
    QGraphicsWidget *optionWidget = scene->addWidget(option);
    optionWidget->setPos(250,50);
    optionWidget->setZValue(3);
    optionWidget->hide();

    QWidget *help = new QWidget;
    QPushButton *helpCloseButton = new QPushButton(tr("关闭"),help);
    helpCloseButton->setPalette(palette);
    helpCloseButton->move(120,300);
    connect(helpCloseButton,SIGNAL(clicked()),help,SLOT(hide()));
    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0,0,0,180)));
    help->resize(300,400);
    QGraphicsWidget *helpWidget = scene->addWidget(help);
    helpWidget->setPos(250,50);
    helpWidget->setZValue(3);
    helpWidget->hide();
    QLabel *helpLabel = new QLabel(help);
    helpLabel->setText(tr("<h1><font color = white>Later练习作品</font></h1>"));
    helpLabel->setAlignment(Qt::AlignCenter);
    helpLabel->move(30,150);

    welcomeText = new QGraphicsTextItem;
    scene->addItem(welcomeText);
    welcomeText->setHtml(tr("<font color = white>俄罗斯方块</font>"));
    welcomeText->setFont(QFont("Times", 30, QFont::Bold));
    welcomeText->setPos(250, 100);
    welcomeText->setZValue(2);
    pausedText = new QGraphicsTextItem();
    scene->addItem(pausedText);
    pausedText->setHtml(tr("<font color=white>游戏暂停中！</font>"));
    pausedText->setFont(QFont("Times", 30, QFont::Bold));
    pausedText->setPos(300, 100);
    pausedText->setZValue(2);
    pausedText->hide();
    gameOverText = new QGraphicsTextItem();
    scene->addItem(gameOverText);
    gameOverText->setHtml(tr("<font color=white>游戏结束！</font>"));
    gameOverText->setFont(QFont("Times", 30, QFont::Bold));
    gameOverText->setPos(320, 100);
    gameOverText->setZValue(2);
    gameOverText->hide();

    QPushButton *startBtn = new QPushButton(tr("开始"));
    QPushButton *optionBtn = new QPushButton(tr("选项"));
    QPushButton *helpBtn = new QPushButton(tr("帮助"));
    QPushButton *exitBtn = new QPushButton(tr("退出"));
    QPushButton *restartBtn = new QPushButton(tr("重新开始"));
    QPushButton *pauseBtn = new QPushButton(tr("暂停"));
    QPushButton *returnBtn = new QPushButton(tr("回到游戏"));
    QPushButton *menuBtn = new QPushButton(tr("主菜单"));
    QPushButton *finishBtn = new QPushButton(tr("结束游戏"));
    connect(startBtn,SIGNAL(clicked()),this,SLOT(startGame()));
    connect(optionBtn,SIGNAL(clicked()),option,SLOT(show()));
    connect(helpBtn,SIGNAL(clicked()),help,SLOT(show()));
    connect(exitBtn,SIGNAL(clicked()),qApp,SLOT(quit()));
    connect(restartBtn,SIGNAL(clicked()),this,SLOT(restartGame()));
    connect(pauseBtn,SIGNAL(clicked()),this,SLOT(pauseGame()));
    connect(returnBtn,SIGNAL(clicked()),this,SLOT(returnGame()));
    connect(menuBtn,SIGNAL(clicked()),this,SLOT(finishGame()));
    connect(finishBtn,SIGNAL(clicked()),this,SLOT(finishGame()));
    startButton = scene->addWidget(startBtn);
    optionButton = scene->addWidget(optionBtn);
    helpButton = scene->addWidget(helpBtn);
    exitButton = scene->addWidget(exitBtn);
    restartButton = scene->addWidget(restartBtn);
    pauseButton = scene->addWidget(pauseBtn);
    returnButton = scene->addWidget(returnBtn);
    showMenuButton = scene->addWidget(menuBtn);
    finishButton = scene->addWidget(finishBtn);
    startButton->setPos(370, 200);
    optionButton->setPos(370, 250);
    helpButton->setPos(370, 300);
    exitButton->setPos(370, 350);
    restartButton->setPos(600, 150);
    pauseButton->setPos(600, 200);
    showMenuButton->setPos(600, 250);
    returnButton->setPos(370, 200);
    finishButton->setPos(370, 250);
    startButton->setZValue(2);
    optionButton->setZValue(2);
    helpButton->setZValue(2);
    exitButton->setZValue(2);
    restartButton->setZValue(2);
    returnButton->setZValue(2);
    finishButton->setZValue(2);
    restartButton->hide();
    finishButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    returnButton->hide();
}

void MyView::initGame()
{
    scene()->setBackgroundBrush(QPixmap(":/images/background01.png"));
    gameScoreText->setHtml(tr("<font color = red>0</font>"));
    gameLevelText->setHtml(tr("<font color = white>开<br>始<br>啦</font>"));
    boxGroup->creatBox(QPointF(300,70));
    boxGroup->setFocus();
    boxGroup->startTimer(INITSPEED);
    gameSpeed = INITSPEED;
    nextBoxGroup->creatBox(QPointF(500,70));

    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    gameScoreText->show();
    gameLevelText->show();
    topLine->show();
    bottomLine->show();
    leftLine->show();
    rightLine->show();
    boxGroup->show();
}

void MyView::updateScore(const int fullRowNum)
{
    int score = fullRowNum * 100;
    int currentScore = gameScoreText->toPlainText().toInt();
    currentScore += score;
    gameScoreText->setHtml(tr("<font color = red>%1</font>").arg(currentScore));
    if(currentScore < 500){

    }else if(currentScore < 1500){
        gameLevelText->setHtml(tr("<font color = white>加<br>速</font>"));
        scene()->setBackgroundBrush(QPixmap(":/images/background02.png"));
        gameSpeed = 300;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
    }else{
        gameLevelText->setHtml(tr("<font color = white>最<br>高<br>速</font>"));
        scene()->setBackgroundBrush(QPixmap(":/images/background03.png"));
        gameSpeed = 100;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
    }
}
