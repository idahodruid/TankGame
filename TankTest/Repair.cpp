#include <Repair.h>
#include <QRandomGenerator>



Repair::Repair(QGraphicsItem* parent) : QObject(), QGraphicsPixmapItem() {

    //isPauseActive = false;
    //v = view;
    //this->setFocus();
    // set random position
    int randomNumberRotation = QRandomGenerator::global()->bounded(0, 360);

    setTransformOriginPoint(boundingRect().width() / 2, boundingRect().height() / 2);
    setZValue(-3);

    setPixmap(QPixmap(":/images/GEARZ1.png"));
    setTransformOriginPoint(this->boundingRect().width() / 2, this->boundingRect().height() / 2);





    //connect(enemyTimer, SIGNAL(timeout()), this, SLOT(frame()));
    this->setZValue(-3);
}
//void Shield::spawn() {
    // create an enemy
 //   Shield* shield = new Shield();
 //   scene()->addItem(shield);
    //Obstacles* obstacle = new Obstacles();
    //scene()->addItem(obstacle);
//}
