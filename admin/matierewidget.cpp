#include "matierewidget.h"

MatiereWidget::MatiereWidget(QWidget* parent): QWidget(parent)
{
    auto layout = new QVBoxLayout();

    filieres = new QComboBox();
    auto btnAjout = new QPushButton(tr("Ajouter"));
    this->btnModifier = new QPushButton(tr("Modifier"));
    auto btnModifier = this->btnModifier; btnModifier->setVisible(false);
    btnAjout->setToolTip(tr("Ajouter une matiere"));
    btnAjout->setCursor(QCursor(Qt::PointingHandCursor));
    QStringList lsFilieres;
    lsFilieres<<"Informatique"<<"Telecommunication"<<"MTIC";
    lsFilieres<<"ITM"<<"L2 Informatique"<<"Genie logiciel"<<"Cyber securité"<<"L2 Telecome"
            <<"Reseau telecome"<<"Radiocome"<<"Systeme embarqué"<<"MTIC1"<<"MTIC2"<<"MGP"<<"Economie numerique";
    lsFilieres.sort();
    filieres->addItems(lsFilieres);
    auto lh = new QHBoxLayout();
    lh->addWidget(new QLabel("Filtrage: "), 0, Qt::AlignHCenter);
    lh->addWidget(filieres, 0, Qt::AlignHCenter);
    lh->addWidget(btnAjout, 0, Qt::AlignRight);

    layout->addWidget(new QLabel("<h1 style='color:blue; text-align:center'>"+
                                 tr("Liste des matières")+"</h1>"));
    layout->addItem(lh);

    vue = new QTableView();
    vue->setSelectionBehavior(QTableView::SelectRows);
    vue->setSelectionMode(QTableView::SingleSelection);
    vue->setEditTriggers(QTableView::NoEditTriggers);
    vue->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto model = new QStandardItemModel();
    QStringList labels; labels<<"Nom de la matiere"<<"Niveau"<<"Eseigné par";
    model->setHorizontalHeaderLabels(labels);
    vue->setModel(model);

    layout->addWidget(vue);
    layout->addWidget(btnModifier, 0, Qt::AlignLeft);
    setLayout(layout);

    connect(btnAjout, &QPushButton::clicked, this, &MatiereWidget::showAddWidget);
    connect(btnModifier, &QPushButton::clicked, this, &MatiereWidget::showEditWidget);
    connect(filieres, &QComboBox::currentTextChanged, this, &MatiereWidget::actualiser);
    connect(vue, &QTableView::clicked, btnModifier, [btnModifier](const QModelIndex&){btnModifier->setVisible(true);});
}

void MatiereWidget::showAddWidget()
{
    if(database == nullptr || !database->isOpen()) return;
    auto w = new QWidget();
    auto lF = new QFormLayout(); lF->setSpacing(15);
    auto label = new QLabel("<div style='color:blue; font-size:20px; text-align:center;'>"+tr("Ajout d'une matière") + "</div>");
    auto nomMatiere = new QLineEdit();
    auto niveau = new QComboBox();
    auto btnSave = new QPushButton(tr("Enregistrer"));
    QStringList lsFilieres = database->getLsLevel();

    lsFilieres.sort();
    niveau->addItems(lsFilieres);

    lF->addWidget(label);
    lF->addRow(tr("Nom de la matière: "), nomMatiere);
    lF->addRow(tr("Niveau: "), niveau);
    lF->addWidget(btnSave);
    lF->setAlignment(btnSave, Qt::AlignBottom);
    w->setLayout(lF);

    auto me = this;
    connect(btnSave, &QPushButton::clicked, this, [nomMatiere, niveau, me](){
        if(nomMatiere->text().isEmpty()){
            QMessageBox::information(me, "", tr("Le nom de la matiere ne doit pas etre vide"));
            return;
        }
        Matiere matiere(nomMatiere->text());
        matiere.setNiveau(niveau->currentText());
        if(me->database->addStatictMatiere(matiere)){
            me->actualiser();
            nomMatiere->setText("");
            //nomMatiere->setFocus(Qt::inp)
           // w->deleteLater();
        }
    });

    w->setWindowModality(Qt::ApplicationModal);
    w->setFixedSize(400, 250);
    w->move(x()+width()/2-200, y()+height()/2);
    w->show();
}

//Modification de la matiere
void MatiereWidget::showEditWidget()
{
    if(database == nullptr || !database->isOpen()) return;

    //Recherche de la matiere dans la vue
    auto selection = vue->selectionModel()->selectedIndexes();
    if(selection.size() < 3) return;

    qDebug()<<"MatiereWidget::showEditWidget(). Nom matiere: "<<selection.at(0).data().toString();
    qDebug()<<"MatiereWidget::showEditWidget(). Nom Filiere: "<<selection.at(1).data().toString();

    auto oldMatiere = Matiere(selection.at(0).data().toString());
    oldMatiere.setNiveau(selection.at(1).data().toString());

    auto w = new QWidget();
    auto lF = new QFormLayout(); lF->setSpacing(15);
    auto label = new QLabel("<div style='color:blue; font-size:20px; text-align:center;'>"+tr("Modification de la matière") + "</div>");
    auto nomMatiere = new QLineEdit();
    auto niveau = new QComboBox();
    auto nomEnseignant = new QComboBox();
    auto btnSave = new QPushButton(tr("Modifier"));

    //Rechercher et chargement de la liste des enseignants
    QStringList lsFilieres = database->getLsLevel();
    lsFilieres.sort();
    niveau->addItems(lsFilieres);
    QStringList lsEnseignant;
    Teacher oldTeacher;
    foreach (auto teacher, database->getlsTeacher()) {
        lsEnseignant.append(teacher.toString());
        if(teacher.toString() == selection.at(2).data().toString())
            oldTeacher = teacher;
    }
    oldMatiere.setEnseignant(oldTeacher);
    lsEnseignant.sort();
    static QString personne(tr("Personne"));
    lsEnseignant.insert(0, personne);
    nomEnseignant->addItems(lsEnseignant);
    nomEnseignant->setCurrentText(selection.at(2).data().toString());


    nomMatiere->setText(selection.at(0).data().toString());
    niveau->setCurrentText(selection.at(1).data().toString());

    lF->addWidget(label);
    lF->addRow(tr("Nom de la matière: "), nomMatiere);
    lF->addRow(tr("Enseignée par: "), nomEnseignant);
    lF->addRow(tr("Niveau: "), niveau);
    lF->addWidget(btnSave);
    lF->setAlignment(btnSave, Qt::AlignBottom);
    w->setLayout(lF);

    auto me = this;
    connect(btnSave, &QPushButton::clicked, this, [nomMatiere, nomEnseignant, niveau, me, oldMatiere, w](){
        if(nomMatiere->text().isEmpty()){
            QMessageBox::information(me, "", tr("Le nom de la matiere ne doit pas etre vide"));
            return;
        }
        Teacher newTeacher;
        foreach (auto teacher, me->database->getlsTeacher()) {
            if(teacher.toString() == nomEnseignant->currentText())
                newTeacher = teacher;
        }

        Matiere newMatiere(nomMatiere->text());
        newMatiere.setNiveau(niveau->currentText());
        newMatiere.setEnseignant(newTeacher);
        w->deleteLater();
        if(me->database->updateStatictMatiere(oldMatiere, newMatiere))
            me->actualiser();
    });

    w->setWindowModality(Qt::ApplicationModal);
    w->setFixedSize(450, 250);
    w->move(x()+width()/2-200, y()+height()/2);
    w->show();
}

void MatiereWidget::actualiser()
{
    if(database == nullptr || !database->isOpen()) return;
    auto level = filieres->currentText();
    if(filieres->currentIndex() == 0)
        level = "";
    auto lsMatiere = database->getlsStaticMatiere(level);
    auto model = new QStandardItemModel(lsMatiere.size(), 3);
    int i = 0;
    foreach (auto matiere, lsMatiere) {
        model->setItem(i, 0, new QStandardItem(matiere.getName()));
        model->setItem(i, 1, new QStandardItem(matiere.getNiveau()));
        model->setItem(i++, 2, new QStandardItem(matiere.getEnseignant().toString()));
    }
    QStringList labels; labels.clear(); labels<<"Nom de la matiere"<<"Niveau"<<"Eseigné par";
    model->setHorizontalHeaderLabels(labels);
    vue->setModel(model);
    btnModifier->setVisible(false);
}

void MatiereWidget::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase;
    auto me = this;
    connect(database, &EmploiDatabase::connexionSignal, this, [me](){
        auto lsFilieres = me->database->getLsLevel();
        lsFilieres.sort();
        auto listView = new QListView();
        auto model = new QStandardItemModel();
        model->appendRow(new QStandardItem(tr("Auccun filtrage")));
        foreach (auto filiere, lsFilieres) {
            model->appendRow(new QStandardItem(filiere));
        }
        listView->setModel(model);
        me->filieres->setModel(model);
        //me->filieres->setView(listView);
    });
}

void MatiereWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if(database == nullptr || database->isOpen()) return;

    auto selection = vue->selectionModel();
    if(selection == 0) return;
    auto menu = new QMenu();

    auto indexes= selection->currentIndex();
    if(indexes.isValid()){
        menu->addAction("Editer");
        menu->move(QCursor::pos());
        menu->show();
    }else{
        menu->addAction(tr("Actualiser"), this, &MatiereWidget::actualiser);
        menu->move(QCursor::pos());
        menu->show();
    }
    event->accept();
}
