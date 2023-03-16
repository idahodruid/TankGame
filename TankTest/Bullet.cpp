#include "Bullet.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>
#include <QMovie>
#include <QLabel>
#include <Explosion.h>
#include "Enemy.h"
#include "Tank.h"
#include "wall.h"

extern QTimer* enemyTimer;
Bullet::Bullet(QGraphicsItem* tank, char direction, float angle, QGraphicsItem* parent) : QGraphicsPixmapItem(parent) {
    //draw bullet
    direct = direction;
    this->angle = angle;
    this->tank = tank;
    this->setZValue(-4);
    speed = 5;

    if (typeid(*(tank)) == typeid(Enemy)) {
        setPixmap(QPixmap(":/images/bulletRed.png"));
    }
    if (typeid(*(tank)) == typeid(Tank)) {
        setPixmap(QPixmap(":/images/bulletGreen.png"));
    }
    setTransformOriginPoint(boundingRect().width() / 2, boundingRect().height() / 2);

}

// Four-directional shooting that uses the direction the tank is facing
void Bullet::fireDirectional() {

    // Calculate x and y velocity
    switch (direct) {
    case 'w':
        dx = 0;
        dy = speed * -1;
        setRotation(0);
        break;
    case 'a':
        dx = speed * -1;
        dy = 0;
        setRotation(270);
        break;
    case 's':
        dx = 0;
        dy = speed;
        setRotation(180);
        break;
    case 'd':
        dx = speed;
        dy = 0;
        setRotation(90);
        break;
    }

    //connect
    connect(enemyTimer, SIGNAL(timeout()), this, SLOT(move()));
    enemyTimer->start(7);
}

// Swivel shooting that uses the mouse cursor
void Bullet::fireSwivel() {

    float angleDegrees = angle * (180 / M_PI);

    // Calculate x and y velocity
    dx = speed * cos(angle);
    dy = speed * sin(angle);

    // Trig shenanigans. Not sure why angleDegrees by itself doesn't work; I dislike just tacking on 90 degrees but it's fine for now.
    setRotation(angleDegrees + 90);

    //connect
    connect(enemyTimer, SIGNAL(timeout()), this, SLOT(frame()));
    enemyTimer->start(7);
}

void Bullet::frame() {
    //Collison check for enemies
    QList<QGraphicsItem*> colliding_items = collidingItems();
    colliding_items.removeAll(tank);
    for (int i = 0; i < colliding_items.size(); i++) {
        if (typeid(*(colliding_items[i])) == typeid(Enemy)) {
            //delete enemy
            QPointF explosionPos;
            explosionPos.setX(colliding_items[i]->pos().x() - (colliding_items[i]->boundingRect().width() / 2));
            explosionPos.setY(colliding_items[i]->pos().y() - (colliding_items[i]->boundingRect().height() / 2));

            //Create explosion on collision
            Explosion* explosion = new Explosion(explosionPos);
            scene()->addItem(explosion);

            //delete bullet
            scene()->removeItem(colliding_items[i]);
            scene()->removeItem(this);
            delete this;

            //Delete enemy
            delete(colliding_items[i]);
            return;

        }
        // You could also incorporate the explosion effect into the wall
        if (typeid(*(colliding_items[i])) == typeid(Wall)) {

            //disconnect signal from timer
            disconnect(enemyTimer, SIGNAL(timeout()), this, SLOT(move()));
            enemyTimer->stop();

            //delete bullet and wall
            scene()->removeItem(colliding_items[i]);
            scene()->removeItem(this);

            //Delete objects
            delete(colliding_items[i]);
            delete(this);
            return;
        }

    }

    // Move bullet towards the direction of the cursor when it was fired
    setPos(x() + dx, y() + dy);

    // Remove bullet if it goes out of bounds
    if ((pos().y() + boundingRect().height() < 0) || (pos().y() > scene()->height()) || (pos().x() + boundingRect().width() < 0) || (pos().x() > scene()->width())) {
        scene()->removeItem(this);
        //delete timer;
        delete this;
    }
}
