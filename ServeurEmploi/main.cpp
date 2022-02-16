#include <QCoreApplication>
//#include <QTextEdit>
#include "controler/serveuremploi.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto serveur = new ServeurEmploi();

    QObject::connect(serveur, &ServeurEmploi::debug, [](QString text){
        qDebug()<<text;
    });
    serveur->demarer();
    return app.exec();
}
