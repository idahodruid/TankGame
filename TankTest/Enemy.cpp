#include "Enemy.h"
#include "Bullet.h"
#include "Tank.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QList>
#include <QDebug>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>

extern QTimer *enemyTimer;
QRandomGenerator Enemy::generator = QRandomGenerator::securelySeeded();

Enemy::Enemy(QGraphicsItem* player, QGraphicsItem* parent) : QObject(), QGraphicsPixmapItem() {

    playerTank = dynamic_cast<Tank*>(player);

    pathTravelTime = generator.bounded(10, 26) * 144;

    // set random position
    int randomNumberRotation = QRandomGenerator::global()->bounded(0, 360);

    // determines what kind of aggro-behaviour the AI will have
    int whatType = QRandomGenerator::global()->bounded(0, 3);
    switch (whatType) {
    case 0:
        isFleeType = true;
        isHoldType = false;
        isChargeType = false;
        break;
    case 1:
        isFleeType = false;
        isHoldType = true;
        isChargeType = false;
        break;
    case 2:
        isFleeType = false;
        isHoldType = false;
        isChargeType = true;
        break;
    default:
        isFleeType = true;
        isHoldType = false;
        isChargeType = false;
        break;
    }

    MAXHEALTH = 60;
    health = 60;
    traversalSpeed = .3;
    rotationSpeed = .3;
    turretRotationSpeed = .4;
    counter = 0;
    treadCounter = 0;
    bulletCoolDownCounter = 0;

    graphicString = "redChassisNoDamage";
    setPixmap(QPixmap(":/images/" + graphicString + "One.png"));

    seePlayer = false;
    roam = true;
    isAgainstWall = false;
    changeTreads = false;
    isTurning = false;
    isWallTurning = false;

    circle = new QGraphicsEllipseItem();
    turret = new QGraphicsPixmapItem();
    fireFlash = new QGraphicsPixmapItem();

    direction = 'w';


    setZValue(-3);

    setTransformOriginPoint(this->boundingRect().width() / 2, this->boundingRect().height() / 2);


    // Create the healthBar item
    healthBar = new QGraphicsRectItem(0, -10, 50, 5, this);
    healthBar->setRect(0, -10, (health / 100.0) * 50, 5);
    healthBar->setBrush(QColor("#c17d7c"));
    healthBar->setPen(QPen(Qt::white, .7));
    healthBar->setPos(-25, -15);

    // Create a QGraphicsDropShadowEffect with a white color and blur radius
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect();
    glowEffect->setBlurRadius(10);
    glowEffect->setColor(Qt::white);
    glowEffect->setOffset(0, 0);

    // Apply the effect to the healthBar item
    healthBar->setGraphicsEffect(glowEffect);

    bulletHandler = new QMediaPlayer();
    bulletAudioPlayer = new QAudioOutput();
    bulletAudioPlayer->setVolume(.15);
    bulletHandler->setAudioOutput(bulletAudioPlayer);
    bulletHandler->setSource(QUrl("qrc:/sounds/bulletFireThree.wav"));

    // connect
    connect(enemyTimer, SIGNAL(timeout()), this, SLOT(frame()));
}

void Enemy::createVision() {
    // add a circle
    circle->setRect(0, 0, 1000, 1000); // diameter 100
    circle->setTransformOriginPoint(circle->boundingRect().width() / 2, circle->boundingRect().height() / 2);
    circle->setPos(x() + this->boundingRect().width() / 2 - circle->boundingRect().width() / 2, y() + this->boundingRect().height() / 2 - circle->boundingRect().height() / 2); // center the circle on the enemy
    circle->setBrush(QBrush(Qt::blue)); // set color and style of the circle's interior\
    circle->setZValue(-5);
    circle->setVisible(false);
    scene()->addItem(circle);
}

// Creates turret and sets its point of rotation.
void Enemy::createTurret(QString str) {
    int rotationPoint = 7; // 7 pixels down the turret is where the hatch on the turret is which is where the rotation point needs to be.
    turret->setPos(x() + this->boundingRect().width() / 2 - turret->boundingRect().width() / 2, y() + this->boundingRect().height() / 2 - turret->boundingRect().height() / 2 - 10);
    turret->setPixmap(QPixmap(":/images/redTurret.png"));
    turret->setTransformOriginPoint(turret->boundingRect().width() / 2, turret->boundingRect().height() / 2 + rotationPoint);
    turret->setZValue(-2);
    turret->setRotation(rotation());
    scene()->addItem(turret);

    fireFlash->setPixmap(QPixmap(":/images/gunFlash.png"));
    fireFlash->setVisible(false);
    fireFlash->setZValue(-2);
    scene()->addItem(fireFlash);
}

void Enemy::frame() {
    counter++;
    isMoving = false;
    QPointF currentPos = pos();
    int currentRot = rotation();
    turret->setPos(x() + this->boundingRect().width() / 2 - turret->boundingRect().width() / 2, y() + this->boundingRect().height() / 2 - turret->boundingRect().height() / 2 - 7);
    circle->setPos(x() + this->boundingRect().width() / 2 - circle->boundingRect().width() / 2, y() + this->boundingRect().height() / 2 - circle->boundingRect().height() / 2);
    float angleTank = (rotation() - 90) * (M_PI / 180);
    float dxTank = calculateAngleCos(traversalSpeed, angleTank);
    float dyTank = calculateAngleSin(traversalSpeed, angleTank);

    if (bulletCoolDownCounter != 0) {
        bulletCoolDownCounter++;
    }
    if (bulletCoolDownCounter == 288) {
        bulletCoolDownCounter = 0;
    }


    // Collision detection is computationally expensive - will slow down program if ran too many times per second.
    // As a result I have set it so it will only run once a second.
    if (counter % 144 == 0) {
        QList<QGraphicsItem*> collidingItemsVision = objectDetected();
        roam = true;
        seePlayer = false;
        if (!collidingItemsVision.isEmpty()) {
            for (QGraphicsItem* item : collidingItemsVision) {
                if (typeid(*item) == typeid(Tank)) {
                    priorityItem = item;
                    seePlayer = true;
                    roam = false;
                    break;
                }
                else if (typeid(*item) == typeid(Enemy)) {
                }
                else if (typeid(*item) == typeid(Bullet)) {
                }
                else {
                }
            }
        }
        else {
        }
    }

    if (seePlayer) {
        float angle = (atan2((priorityItem->y() + (priorityItem->boundingRect().height() / 2)) - (y() + (this->boundingRect().height() / 2)), (priorityItem->x() + (priorityItem->boundingRect().width() / 2)) - (x() + (this->boundingRect().width()) / 2)));
        float angleDegrees = angle * (180 / M_PI) + 90;
        float rotationDifference = angleTo360(angleDegrees - turret->rotation());

        if (rotationDifference < 180) {
            turret->setRotation(turret->rotation() + turretRotationSpeed);
        }
        else if (rotationDifference > 180) {
            turret->setRotation(turret->rotation() - turretRotationSpeed);
        }

        // Enemy will turn towards player if they don't have hold ground behavior
        if (!isHoldType) {
            float rotationDifferenceChassis = angleTo360(angleDegrees - this->rotation());
            if (rotationDifferenceChassis < 180) {
                setRotation(rotation() + rotationSpeed);
            }
            else if (rotationDifferenceChassis > 180) {
                setRotation(rotation() - rotationSpeed);
            }

            if (isChargeType) {
                setPos(x() + dxTank, y() + dyTank);
                isMoving = true;
            }
            else {
                setPos(x() - .5*dxTank, y() - .5*dyTank);
                isMoving = true;
            }
        }

        float turretAngle = (turret->rotation() - 90) * (M_PI / 180);

        if ((abs(angleTo360(angleDegrees - turret->rotation())) / 360) < .02 && (bulletCoolDownCounter == 0)) {
            Bullet* bullet = new Bullet(this, direction, turretAngle);
            bullet->setPos(x() + this->boundingRect().width() / 2 - bullet->boundingRect().width() / 2, y() + this->boundingRect().height() / 2 - bullet->boundingRect().height() / 2);
            scene()->addItem(bullet);
            bullet->fireSwivel();
            bulletCoolDownCounter = 1;
            //Fire Bullet sound
            bulletHandler->setPosition(0);
            bulletHandler->play();
        }

        // Checks to see if Tank is going out of bounds
        if ((x() < 0 || (x() + boundingRect().right() > scene()->width()) || (y() < 0) || (y() + boundingRect().bottom() > scene()->height())))
        {
            setPos(currentPos);
            setRotation(currentRot);
        }
    }

    if (roam) {
        // Set new path after old path expires
        if (pathTravelTime < 0 && !isTurning && !isWallTurning) {
            isTurning = true;
            int random = generator.bounded(10, 16);
            if (random % 2 == 0) {
                isTurningLeft = true;
            }
            else {
                isTurningLeft = false;
            }
            pathTravelTime = random * 144;
            pathTurnTime = generator.bounded(90, 200);
        }

        // When it is turning
        else if (isTurning) {
            if (isTurningLeft) {
                if (pathTurnTime > 0) {
                    setRotation(rotation() + rotationSpeed);
                    pathTurnTime--;
                }
                else {
                    isTurning = false;
                }
            }
            else {
                if (pathTurnTime > 0) {
                    setRotation(rotation() - rotationSpeed);
                    pathTurnTime--;
                }
                else {
                    isTurning = false;
                }
            }
        }

        else if (isWallTurning) {
            if (isTurningLeft) {
                if (pathTurnTime > 0) {
                    setRotation(rotation() + rotationSpeed);
                    pathTurnTime--;
                }
                else {
                    isWallTurning = false;
                    isTurning = false;
                    pathTravelTime = generator.bounded(10, 26) * 144;
                }
            }
            else {
                if (pathTurnTime > 0) {
                    setRotation(rotation() - rotationSpeed);
                    pathTurnTime--;
                }
                else {
                    isWallTurning = false;
                    isTurning = false;
                    pathTravelTime = generator.bounded(10, 26) * 144;
                }
            }
        }

        // When it is trying to get away from the wall
        else if (isAgainstWall) {
            if (pathTravelTime > 0) {
                setPos(x() - dxTank, y() - dyTank);
                isMoving = true;
                pathTravelTime--;
            }
            else {
                isAgainstWall = false;
                isWallTurning = true;
                pathTurnTime = generator.bounded(190, 300);
                if (pathTurnTime % 2 == 0) {
                    isTurningLeft = true;
                }
                else {
                    isTurningLeft = false;
                }
            }
        }

        // Normal movement
        else {
            setPos(x() + dxTank, y() + dyTank);
            isMoving = true;
            pathTravelTime--;
        }

        // Checks to see if Tank is going out of bounds
        if ((x() < 0 || (x() + boundingRect().right() > scene()->width()) || (y() < 0) || (y() + boundingRect().bottom() > scene()->height())))
        {
            previousRotation = rotation();
            setPos(x() - dxTank * 2, y() - dyTank * 2);
            isMoving = true;
            setRotation(currentRot);
            isAgainstWall = true;
            isTurning = false;
            pathTravelTime = generator.bounded(1, 4) * 144;
        }

        if (rotation() > turret->rotation()) {
            turret->setRotation(turret->rotation() + turretRotationSpeed);
        }
        else if (rotation() < turret->rotation()) {
            turret->setRotation(turret->rotation() - turretRotationSpeed);
        }
    }

    // Code that handles the animation for the tank treads. Every 20 times the move() function is called while the tank is actually moving
    if ((isMoving == true)) {
        treadCounter++;
        if (treadCounter % 20 == 0) {
            if (changeTreads == true) {
                setPixmap(QPixmap(":/images/" + graphicString + "One.png"));
                changeTreads = false;
            }
            else {
                setPixmap(QPixmap(":/images/" + graphicString + "Two.png"));
                changeTreads = true;
            }
        }
    }
}

void Enemy::setHealth(int health) {
    // Set the size of the health bar based on the enemy's health
    healthBar->setRect(0, -10, (health / 100.0) * 50, 5);
}

void Enemy::takeDamage(int damage) {
    // Decrease the enemy's health by the specified amount
    health -= damage;

    // Update the health bar
    setHealth(health);

    float percentHealthLeft = (float) health / (float) MAXHEALTH;

    if ( (percentHealthLeft < 1) && (percentHealthLeft > .5) ) {
        graphicString = "redChassisDamagedHalf";
        if (changeTreads) {
            setPixmap(QPixmap(":/images/" + graphicString + "Two.png"));
        }
        else {
            setPixmap(QPixmap(":/images/" + graphicString + "One.png"));
        }
    }
    else if (percentHealthLeft <= .5) {
        graphicString = "redChassisDamagedFull";
        if (changeTreads) {
            setPixmap(QPixmap(":/images/" + graphicString + "Two.png"));
        }
        else {
            setPixmap(QPixmap(":/images/" + graphicString + "One.png"));
        }
    }
    // Check if the enemy has been defeated
    if (health <= 0) {
        // Remove the enemy from the scene
        scene()->removeItem(this);
        
        playerTank->killConfirmed(this);

        // Delete the enemy object
        delete this;
    }
}

// Returns list of meaningful objects colliding with this Enemy's vision circle
QList<QGraphicsItem*> Enemy::objectDetected() {
    QList<QGraphicsItem*> collidingItemsVision = circle->collidingItems();

    // Removes its own Tank graphic from list
    collidingItemsVision.removeOne(this);

    // Removes graphic items with Z-values of -10(background), -5(other vision circles), and -2(tank turrets and muzzle flashes)
    for (int i = 0; i < collidingItemsVision.size(); i++) {
        QGraphicsItem* item = collidingItemsVision[i];
        if (item->zValue() == -10 || item->zValue() == -5 || item->zValue() == -2) {
            collidingItemsVision.removeOne(item);
            i--;
        }
        else {
        }
    }
    return collidingItemsVision;
}

float Enemy::angleTo360(float rotation) {
    while (rotation >= 360.0) {
        rotation -= 360.0;
    }
    while (rotation < 0.0) {
        rotation += 360.0;
    }
    return rotation;
}

float Enemy::calculateAngleCos(float speed, float angle) {
    float dx = speed * cos(angle);
    return dx;
}

float Enemy::calculateAngleSin(float speed, float angle) {
    float dy = speed * sin(angle);
    return dy;
}

// Deletes vision-circle when enemy is destroyed
Enemy::~Enemy() {
    delete circle;
    delete turret;
    delete fireFlash;
    delete bulletAudioPlayer;
    delete bulletHandler;
    delete healthBar;
}
