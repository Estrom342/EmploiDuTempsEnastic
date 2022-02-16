#include "teacherwidget.h"

TeacherWidget::TeacherWidget(QWidget *parent) : QWidget(parent)
{
    auto me = this;
    database = nullptr;
    auto layout = new QVBoxLayout();

    filiere = new QComboBox();
    auto btnAjout = new QPushButton(tr("Ajouter"));
    btnAjout->setToolTip(tr("Ajouter un enseignant"));
    btnAjout->setCursor(QCursor(Qt::PointingHandCursor));

    this->btnModifier = new QPushButton(tr("Modifier"));
    auto btnModifier = this->btnModifier; btnModifier->setVisible(false);

    QStringList lsFilieres;
    lsFilieres<<"Informatique"<<"Telecommunication"<<"MTIC";
    lsFilieres<<"ITM"<<"L2 Informatique"<<"Genie logiciel"<<"Cyber securité"<<"L2 Telecome"
            <<"Reseau telecome"<<"Radiocome"<<"Systeme embarqué"<<"MTIC1"<<"MTIC2"<<"MGP"<<"Economie numerique";
    lsFilieres.clear();
    lsFilieres.sort();
    filiere->addItems(lsFilieres);
    auto lh = new QHBoxLayout();
    lh->addWidget(new QLabel("Filtrage: "), 0, Qt::AlignHCenter);
    lh->addWidget(filiere, 0, Qt::AlignHCenter);
    lh->addWidget(btnAjout, 0, Qt::AlignRight);

    layout->addWidget(new QLabel("<h1 style='color:blue; text-align:center'>"+
                                 tr("Liste des Enseignants")+"</h1>"));
    layout->addItem(lh);

    vue = new QTableView();
    vue->setSelectionBehavior(QTableView::SelectRows);
    vue->setSelectionMode(QTableView::SingleSelection);
    vue->setEditTriggers(QTableView::NoEditTriggers);
    vue->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    auto model = new QStandardItemModel();
    QStringList labels; labels<<"Titre"<<"Nom"<<"Prenom"<<"Matiere"<<"Niveau d'enseignement";
    model->setHorizontalHeaderLabels(labels);
    vue->setModel(model);

    layout->addWidget(vue);
    layout->addWidget(btnModifier, 0, Qt::AlignLeft);
    setLayout(layout);

    connect(btnAjout, &QPushButton::clicked, this, &TeacherWidget::showAddWidget);
    connect(vue, &QTableView::clicked, btnModifier, [btnModifier](const QModelIndex&){btnModifier->setVisible(true);});
    connect(btnModifier, &QPushButton::clicked, this, [me](){
        auto selection = me->vue->selectionModel()->selectedIndexes();
        if(selection.size() < 5) return;

        Teacher oldTeacher(selection.at(1).data().toString(), selection.at(2).data().toString(),
                           selection.at(0).data().toString(), selection.at(4).data().toString());
        oldTeacher.setNomMatiere(selection.at(3).data().toString());
        me->showEditWidget(oldTeacher);
    });
}

void TeacherWidget::showAddWidget()
{
    if(database == nullptr) return;
    if(!database->isOpen())
        return;
    auto w = new QWidget();
    auto lF = new QFormLayout(); lF->setSpacing(15);
    auto label = new QLabel("<div style='color:blue; font-size:20px; text-align:center;'>"+tr("Ajout d'un enseignant") + "</div>");
    auto nomEnseignant = new QLineEdit();
    auto prenomEnseignant = new QLineEdit();
    auto niveau = new QComboBox();
    auto grade = new QComboBox();
    auto matiere = new QComboBox();
    auto btnSave = new QPushButton(tr("Enregistrer"));

    auto lsNiveau= database->getLsLevel();
    QStringList lsGrade; lsGrade<<"Mr"<<"Mm"<<"Dr"<<"Pr";
    niveau->addItems(lsNiveau);
    niveau->setCurrentText(filiere->currentText());
    grade->addItems(lsGrade);
    {
        auto lsMatiere = database->getlsStaticMatiere(niveau->currentText());
        //QStringList lsMatiereName;
        auto model = new QStandardItemModel(lsMatiere.size(), 1); int i=0;
        foreach (auto matiere, lsMatiere) {
            model->setItem(i++, new QStandardItem(matiere.getName()));
        }
        matiere->setModel(model);
    }

    lF->addWidget(label);
    lF->addRow(tr("Nom de l'enseignant: "), nomEnseignant);
    lF->addRow(tr("Prénom l'enseignant: "), prenomEnseignant);
    lF->addRow(tr("Niveau: "), niveau);
    lF->addRow(tr("Matiere: "), matiere);
    lF->addRow(tr("Grade: "), grade);
    lF->addWidget(btnSave);
    lF->setAlignment(btnSave, Qt::AlignBottom);
    w->setLayout(lF);

    w->setWindowModality(Qt::ApplicationModal);
    w->resize(400, 250);
    w->move(x()+width()/2-200, y()+height()/2);
    w->setFixedSize(400, 300);

    auto me = this;
    connect(btnSave, &QPushButton::clicked, this, [nomEnseignant, prenomEnseignant, niveau, grade,matiere, me](){
        if(nomEnseignant->text().isEmpty() || prenomEnseignant->text().isEmpty()){
            QMessageBox::information(me, "", tr("Le nom et le prénom ne doivent pas être vides"));
            return;
        }
        if(matiere->currentText().isEmpty()){
            QMessageBox::critical(me, "", tr("Vous ne pouvez pas enregistrer un enseignant qui n'a pas de matière"));
            return;
        }
        Teacher t(nomEnseignant->text(), prenomEnseignant->text(), grade->currentText(), niveau->currentText());
        t.setNomMatiere(matiere->currentText());
        Teacher t2 = me->database->getTeacher(t.getNomMatiere(), t.getNiveau());
        if(t.getNom() == t2.getNom() && t.getPrenom() == t2.getPrenom())
        if(t2.getId()!=-1 && t2.getNomMatiere() == t.getNomMatiere()){
            auto txt = QString("L'enseignant de %1 existe déjà, il s'agit de %2\n"
                                "Voulez-vous le remplacer par %3 ?");
            txt = txt.arg(t.getNomMatiere()); txt = txt.arg(t2.toString());
            txt = txt.arg(t.toString());
            int rep = QMessageBox::question(me, "", txt);
            if(rep == QMessageBox::Yes){
                if(!me->database->deleteTeacher(t2)){
                    qDebug()<<"Erreur de supression de "<<t2.toString();
                    return ;
                }
            }else
                return;
        }
        if(me->database->addTeacher(t)){
            nomEnseignant->setText("");
            prenomEnseignant->setText("");
            me->actualiser("");
        }
    });
    connect(niveau, &QComboBox::currentTextChanged, this, [me, matiere](QString niveau){
        auto lsMatiere = me->database->getlsStaticMatiere(niveau);
        auto model = new QStandardItemModel(lsMatiere.size(), 1); int i=0;
        foreach (auto cours, lsMatiere) {
            model->setItem(i++, 0, new QStandardItem(cours.getName()));
        }
        matiere->setModel(model);
    });

    w->show();
}

void TeacherWidget::actualiser(QString level)
{
    if(filiere->currentIndex() == 0)
        level = "";
    auto lsTeacher = database->getlsTeacher(level);
    auto model = new QStandardItemModel(lsTeacher.size(), 5); int i=0;
    QStringList labels; labels<<"Titre"<<"Nom"<<"Prenom"<<"Matiere"<<"Niveau d'enseignement";
    model->setHorizontalHeaderLabels(labels);
    foreach (auto teacher, lsTeacher) {
        model->setItem(i, 0, new QStandardItem(teacher.getGrade()));
        model->setItem(i, 1, new QStandardItem(teacher.getNom()));
        model->setItem(i, 2, new QStandardItem(teacher.getPrenom()));
        model->setItem(i, 3, new QStandardItem(teacher.getNomMatiere()));
        model->setItem(i++, 4, new QStandardItem(teacher.getNiveau()));
    }
    btnModifier->setVisible(false);
    vue->setModel(model);
}

//Afficher la fenetre de modification
void TeacherWidget::showEditWidget(Teacher const &oldTeacher)
{
    if(database == nullptr) return;
    if(!database->isOpen())
        return;
    auto ollTeacherPtr = new Teacher(oldTeacher);
    auto w = new QWidget();
    auto lF = new QFormLayout(); lF->setSpacing(15);
    auto label = new QLabel("<div style='color:blue; font-size:20px; text-align:center;'>"+tr("Modification d'un enseignant") + "</div>");
    auto nomEnseignant = new QLineEdit(oldTeacher.getNom());
    auto prenomEnseignant = new QLineEdit(oldTeacher.getPrenom());
    auto grade = new QComboBox();
    auto btnSave = new QPushButton(tr("Modifier")); btnSave->setMaximumWidth(90);
    auto btnCancel = new QPushButton(tr("Annuler"));

    auto lsNiveau= database->getLsLevel();
    QStringList lsGrade; lsGrade<<"Mr"<<"Mm"<<"Dr"<<"Pr";
    grade->addItems(lsGrade); grade->setCurrentText(oldTeacher.getGrade());

    lF->addWidget(label);
    lF->addRow(tr("Nom de l'enseignant: "), nomEnseignant);
    lF->addRow(tr("Prénom l'enseignant: "), prenomEnseignant);
    lF->addRow(tr("Grade: "), grade);
    lF->addRow(btnCancel, btnSave);
    lF->setAlignment(btnSave, Qt::AlignBottom);
    w->setLayout(lF);

    w->setWindowModality(Qt::ApplicationModal);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->resize(400, 250);
    w->move(x()+(width() - w->width())/2, y()+(height() - w->height())/2);
    w->setFixedSize(400, 300);
    w->show();


    auto me = this;
    connect(btnSave, &QPushButton::clicked, this, [nomEnseignant, prenomEnseignant, grade, me, ollTeacherPtr, w](){
        if(nomEnseignant->text().isEmpty() || prenomEnseignant->text().isEmpty()){
            QMessageBox::information(me, "", tr("Le nom et le prénom ne doivent pas etre vides")); return;
        }
        Teacher enseignant(nomEnseignant->text(), prenomEnseignant->text(), grade->currentText());
        if(me->database->updateTeacher(*ollTeacherPtr, enseignant)){
            me->actualiser("");
            delete ollTeacherPtr;
            w->close();
        }
    });
    connect(btnCancel, &QPushButton::clicked, w,[w, ollTeacherPtr](){
        w->close();
        delete ollTeacherPtr;
    });
}

void TeacherWidget::setDatabase(EmploiDatabase *newDatabase)
{
    database = newDatabase; auto me = this;
    connect(filiere, &QComboBox::currentTextChanged, this, &TeacherWidget::actualiser);

    connect(database, &EmploiDatabase::connexionSignal, this, [me](){
        auto lsFiliere = me->database->getLsLevel();
        lsFiliere.insert(0, tr("Auccun filtrage"));
        me->filiere->setModel(new QStringListModel(lsFiliere));
        me->actualiser(me->filiere->currentText());
    });
}
