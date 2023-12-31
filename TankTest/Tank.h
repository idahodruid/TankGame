#pragma once
#include<QGraphicsRectItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "PauseMenu.h"
#include <QPointF>
#include <QLabel>
#include <QGraphicsTextItem>
#include <QGraphicsBlurEffect>
#include <QStack>
#include <QString>

class Tank :public QObject, public QGraphicsPixmapItem {

    Q_OBJECT
    PauseMenu pause;
    QGraphicsPixmapItem* health1;
    QGraphicsPixmapItem* health2;
    QGraphicsPixmapItem* health3;

public:
    Tank(QGraphicsView* view, QGraphicsItem* parent = 0);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void focusOutEvent(QFocusEvent* event);
    bool isMoving();
    void createTurret();
    void createTurret(QString str);
    void createHUD();
    float calculateAngleCos(float speed, float angle);
    float calculateAngleSin(float speed, float angle);
    void takeDamage(int damage);
    void killConfirmed(QGraphicsItem* item);
    static int distanceFormula(int x, int y, QPointF currentPos);
    void ammoSpawn();
    void shieldSpawn();
    void repairSpawn();

    void spawnBoss();

    void obstacleSpawn();

    QList<QGraphicsItem*> enemiesOnScreen;
    int enemiesDestroyed;

public slots:
    void spawn();
    void frame();
    void blur();
    void pickupSpawn();

signals:
    void positionChanged();

private:
    QMediaPlayer* bulletHandler;
    QMediaPlayer* movingHandler;
    QMediaPlayer* idleHandler;
    QPointF currentPos;
    QAudioOutput* bulletAudioPlayer;
    QAudioOutput* movingAudioPlayer;
    QAudioOutput* idleAudioPlayer;

    float traversalSpeed;
    float rotationSpeed;
    char direction;
    long counter;

    bool changeTreads;
    bool isPauseActive;
    bool isDestroyed;
    bool isBossTankSpawned;
    bool bossHasSpawned;

    QTimer* fireRateTimer;
    QGraphicsView* v;

    QMap<int, bool> keys;

    QGraphicsPixmapItem* turret;
    QGraphicsPixmapItem* fireFlash;
    QGraphicsPixmapItem* bulletHUD;
    QGraphicsTextItem* bulletCounterHUD;
    QGraphicsTextItem* bossTankText;

    int health;
    int bulletCounter;
    int MAXHEALTH;
    int shieldCoolDownCounter;

    int howManyPickups;

    QString graphicString;

    QList<QGraphicsItem*> healthList;
    QList<QGraphicsPixmapItem*> bulletList;

    QGraphicsBlurEffect* blurEffect;

    QGraphicsRectItem* healthBar;
    QGraphicsRectItem* healthBarDepleted;
    QGraphicsRectItem* fadeScreen;
};

