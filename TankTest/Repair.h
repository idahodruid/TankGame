#pragma once
#include<qtimer.h>
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

class Repair : public QObject, public QGraphicsPixmapItem {
    QTimer* timer = new QTimer();

    Q_OBJECT
public:
    Repair(QGraphicsItem* parent = 0);
    //~Shield();
    //QList<QGraphicsItem*> objectDetected();


    //char name;

public slots:
    //void spawn();


signals:


private:







    //void spawn();

};


