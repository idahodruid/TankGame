#pragma once
#include<QGraphicsRectItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsView>

#include <QGraphicsPixmapItem>

class Tank :public QObject, public QGraphicsPixmapItem {

    Q_OBJECT
public:
    Tank(QGraphicsView* view, QGraphicsItem* parent = 0);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void focusOutEvent(QFocusEvent* event);
    bool isMoving();

public slots:
    void spawn();
    void move();

private:
    QMediaPlayer* bulletHandler;
    QMediaPlayer* movingHandler;
    QMediaPlayer* idleHandler;

    QAudioOutput* bulletAudioPlayer;
    QAudioOutput* movingAudioPlayer;
    QAudioOutput* idleAudioPlayer;

    int distance;
    char direction;

    QTimer* fireRateTimer;
    QTimer* keyTimer;
    QGraphicsView* v;

    QMap<int, bool> keys;
};

