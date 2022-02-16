#include "timetablehistorywidget.h"

TimeTableHistoryWidget::TimeTableHistoryWidget(QWidget *parent) : QWidget(parent)
{
    timeTableWidget = new EmploiWidget(); timeTableWidget->setObjectName("HISTORY");
    lsTimeTable = new QTableView();
    auto label = new QLabel("Historique d'emplois du temps");
    label->setAlignment(Qt::AlignCenter);
    label->setFont(QFont("", 16, -1, true));
    label->setStyleSheet("color: blue");
    centraleWidget = new QStackedWidget();
    centraleWidget->addWidget(timeTableWidget);
    centraleWidget->addWidget(lsTimeTable);
    auto btnShowList = new QPushButton("Liste");
    auto btnShowTimetable = new QPushButton("Apperçu");
    auto lBtn = new QHBoxLayout();
    lBtn->addWidget(btnShowList); lBtn->addWidget(btnShowTimetable);
    lBtn->setAlignment(Qt::AlignLeft);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(label);
    layout->addItem(lBtn);
    layout->addWidget(centraleWidget);
    setLayout(layout);

    auto me = this;
    connect(btnShowList, &QPushButton::clicked, this, [me]{me->centraleWidget->setCurrentWidget(me->lsTimeTable);});
    connect(btnShowTimetable, &QPushButton::clicked, this, [me](){me->centraleWidget->setCurrentWidget(me->timeTableWidget);});
    connect(lsTimeTable, &QTableView::activated, this, [me](QModelIndex const& index){
        QString data= me->model->item(index.row(), index.column())->data().toString();
        auto menu = new QMenu("");
        menu->addAction("Ouvrir", me, [me, data](){
           me->timeTableWidget->setCurrentLevel(data);
           me->centraleWidget->setCurrentWidget(me->timeTableWidget);
        });
        menu->move(QCursor::pos());
        menu->exec();
    });
}

void TimeTableHistoryWidget::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase;
    connect(database, &EmploiDatabase::connexionSignal, this, &TimeTableHistoryWidget::loadTimeTables);
    timeTableWidget->setDatabase(database);
    timeTableWidget->disconnect(database, &EmploiDatabase::connexionSignal, timeTableWidget, &EmploiWidget::lireEmploi);
}

void TimeTableHistoryWidget::loadTimeTables()
{
    auto lsEmplois = database->lsEmplois();
    QMap<QString, EmploieDuTemp> lsMap;
        model = new QStandardItemModel();
    QStringList labels; labels<<tr("Niveau")<<tr("Salle")<<tr("Date de debut")<<tr("Date de fin")<<
                    tr("Date de création")<<tr("Créé par");
        model->setHorizontalHeaderLabels(labels);
    int i=0;
    foreach(EmploieDuTemp const& em, lsEmplois){
        lsMap[em.niveau()+QString("%1").arg(i++)] = em;

         QList<QStandardItem*> items;
         items<<new QStandardItem(em.niveau())<<new QStandardItem(em.nomSalle())<<new QStandardItem(em.dateDebut().toString("dddd dd MMMM yyyy"))
             <<new QStandardItem(em.dateFin().toString("dddd dd MMMM yyyy"))<<new QStandardItem(em.dateCreation().toString("dddd dd MMMM yyyy"))
            <<new QStandardItem(em.createur());

         foreach (auto item, items) {
          item->setData(QVariant(em.niveau()+QString("%1").arg(i-1)));
         }
          model->appendRow(items);
    }
    timeTableWidget->setLsEmplois(lsMap);
    lsTimeTable->setModel(model);
    lsTimeTable->setSelectionBehavior(QTableView::SelectRows);
    lsTimeTable->setEditTriggers(QTableView::NoEditTriggers);
    lsTimeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
