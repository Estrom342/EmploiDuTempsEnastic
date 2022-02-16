#include "matierewidget.h"

MatiereWidget::MatiereWidget(QWidget* parent): QWidget(parent)
{
    auto layout = new QVBoxLayout();

    auto comboBox = new QComboBox();
    auto btnAjout = new QPushButton(tr("Ajouter"));
    btnAjout->setToolTip(tr("Ajouter une matiere"));
    btnAjout->setCursor(QCursor(Qt::PointingHandCursor));
    QStringList lsFilieres;
    lsFilieres<<"Informatique"<<"Telecommunication"<<"MTIC";
    comboBox->addItems(lsFilieres);
    auto lh = new QHBoxLayout();
    lh->addWidget(new QLabel("   "));
    lh->addWidget(comboBox, 0, Qt::AlignHCenter);
    lh->addWidget(btnAjout, 0, Qt::AlignRight);

    layout->addWidget(new QLabel("<h1 style='color:blue; text-align:center'>"+
                                 tr("Liste des matières")+"</h1>"));
    layout->addItem(lh);

    auto vue = new QTableView();
    auto model = new QStandardItemModel();
    QStringList labels = QString("L1 L2 L3 L4 L5").split(' ');
    model->setHorizontalHeaderLabels(labels);
    vue->setModel(model);

    layout->addWidget(vue);
    setLayout(layout);

    connect(btnAjout, &QPushButton::clicked, this, &MatiereWidget::showAddWidget);
}

void MatiereWidget::showAddWidget()
{
    auto w = new QWidget();
    auto lF = new QFormLayout(); lF->setSpacing(15);
    auto label = new QLabel("<div style='color:blue; font-size:20px; text-align:center;'>"+tr("Ajout d'une matière") + "</div>");
    auto nomMatiere = new QLineEdit();
    auto niveau = new QComboBox();
    auto btnSave = new QPushButton(tr("Enregistrer"));
    QStringList lsNiveau= QString("L1 L2 L3 M1 M2").split(' ');
    niveau->addItems(lsNiveau);

    lF->addWidget(label);
    lF->addRow(tr("Nom de la matière: "), nomMatiere);
    lF->addRow(tr("Niveau: "), niveau);
    lF->addWidget(btnSave);
    lF->setAlignment(btnSave, Qt::AlignBottom);
    w->setLayout(lF);

    w->setWindowModality(Qt::ApplicationModal);
    w->resize(400, 250);
    w->move(x()+width()/2-200, y()+height()/2);
    w->show();
}
