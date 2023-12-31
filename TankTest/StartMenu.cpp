#include "StartMenu.h"
#include "MainHeader.h"
#include "Tank.h"
#include "GameRunner.h"
#include "instructions.h"
#include <QWindow>

StartMenu::StartMenu(QWidget* parent) : QWidget{parent}
{
    setFixedSize(1920, 1080);
    showFullScreen();
    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlag(Qt::WA_DeleteOnClose);
    //Title for the window
    setWindowTitle("Bunker Blitz");

    //Set the icon to the window
    setWindowIcon(QIcon::fromTheme("myicon", QIcon(":/images/icon.png")));

    //Setting color palette
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::black);

    //Allows window to be set to a color
    setAutoFillBackground(true);
    //Sets the color of the window
    setPalette(pal);


    int startButtonHeight = 120;
    int startButtonWidth = 250;
    int quitButtonHeight = 90;
    int quitButtonWidth = 150;
    QPushButton* startButton = new QPushButton("Start", this);
    QPushButton* quitButton = new QPushButton("Quit", this);
    QPushButton* instructionsButton = new QPushButton("Instructions", this);

    startButton->setGeometry(width() / 2 - (startButtonWidth / 2), height() / 2 - (startButtonHeight / 2), 250, 120); // 125
    instructionsButton->setGeometry(width() / 2 - (quitButtonWidth / 2), (height() / 2) + (quitButtonHeight) - 20, 150, 90);
    quitButton->setGeometry(width() / 2 - (quitButtonWidth / 2), (height() / 2) + (quitButtonHeight) + quitButtonHeight - 10, 150, 90);


    //Adjusting style for the buttons
    startButton->setStyleSheet("QPushButton {color: black; background-color: lightGreen; border-style: outset; border-width: 3px; border-color: darkGreen; font: 30px;} QPushButton:pressed { background-color: darkGreen;}");
    quitButton->setStyleSheet("QPushButton {color: black; background-color: lightGreen; border-style: outset; border-width: 3px; border-color: darkGreen; font: 20px} QPushButton:pressed { background-color: darkGreen;}" );
    instructionsButton->setStyleSheet("QPushButton {color: black; background-color: lightGreen; border-style: outset; border-width: 3px; border-color: darkGreen; font: 20px} } QPushButton:pressed { background-color: darkGreen;}");

    startButton->show();
    instructionsButton->show();
    quitButton->show();
    connect(startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
    connect(quitButton, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));
    connect(instructionsButton, SIGNAL(clicked()), this, SLOT(instructionButtonClicked()));


    backgroundMediaPlayer = new QMediaPlayer();
    backgroundAudioOutput = new QAudioOutput();

    backgroundAudioOutput->setVolume(.3);

    backgroundMediaPlayer->setAudioOutput(backgroundAudioOutput);
    backgroundMediaPlayer->setSource(QUrl("qrc:/sounds/tankTheme.wav"));
    backgroundMediaPlayer->play();

}

void StartMenu::startButtonClicked() {
    GameRunner gameRunner;
    hide();
    backgroundMediaPlayer->stop();
}

void StartMenu::instructionButtonClicked(){
    Instructions* instructionMenu = new Instructions();
    instructionMenu->show();
}

//Draws a border around the start menu
void StartMenu::paintEvent(QPaintEvent* e) {
    QPainter painter(this);
    QPen pen;
    pen.setWidth(5);

    pen.setColor(Qt::darkGreen);
    painter.setPen(pen);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    QPixmap titleImage = QPixmap(":/images/title.JPG");
    painter.drawPixmap((width() / 2) - 373.5, 175, titleImage);

    QWidget::paintEvent(e);
}
