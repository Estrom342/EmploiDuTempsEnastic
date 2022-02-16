#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Teacher>();
    QApplication a(argc, argv);
    auto lang = QLocale::system().name().section('_', 0, 0);

    //Traduction
    QTranslator t;
    t.load(QString("qt_")+lang, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&t);
    //Encodage
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    LocalDatabase::createDatabase(":/data/localDataBase.sql");
    MainWindow w; w.showMaximized();
    QApplication::setFont(QFont("", 11, -1, true));
    QApplication::setFont(QFont("", 11, -1, true));
    QApplication::setWindowIcon(QIcon(":/img/img/icon.svg"));
    QApplication::setApplicationDisplayName("Emploi ENASTIC");
    QApplication::setApplicationName("Emploi ENASTIC");
    QApplication::setApplicationVersion("1.0");
    return a.exec();
}
