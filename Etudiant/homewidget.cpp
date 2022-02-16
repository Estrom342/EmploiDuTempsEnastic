#include "homewidget.h"

HomeWidget::HomeWidget(QWidget *parent) : QWidget(parent)
{
    logo = new QLabel(this);
    //logo->setPixmap(QPixmap(":/img/res/logoenastic1.png"));
    img = QPixmap(":/img/res/anime/img5.JPG");
    logo->setPixmap(img);
    logo->setAlignment(Qt::AlignCenter);

    btnShow = new QPushButton(tr("Voir l'emploi du temps"), logo);
    btnShow->setFont(QFont("", 20, -1, true)); btnShow->setFlat(true);
    btnShow->setToolTip(tr("Voir les emploi du temps de la semaines"));
    btnShow->setCursor(QCursor(Qt::PointingHandCursor));

    setStyleSheet("QPushButton{background-color: #A10F9D; padding:5px; border-radius:15px;}");
    connect(btnShow, SIGNAL(clicked()), this, SIGNAL(showEmploiSignal()));

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &HomeWidget::annime);
    timer->start(3000);
}

void HomeWidget::annime()
{
    static int i = 0;
    img = QPixmap(QString(":/img/res/anime/img%1.JPG").arg(i++));
    if(!img.isNull())
        logo->setPixmap(img.scaled(logo->size()));
    i = i > 5 ? 1 : i;
}

void HomeWidget::resizeEvent(QResizeEvent *event)
{
    logo->setPixmap(img.scaled(event->size()));
    logo->resize(event->size());
    btnShow->move(logo->x() + (logo->width() - btnShow->width())/2 , logo->y() + (logo->height() - btnShow->height())/2);
}

