#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include "horaireview.h"
#include <QTimer>

class HomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HomeWidget(QWidget *parent = nullptr);

public slots:
    void annime();

signals:
    void showEmploiSignal();
    void checkEmploiSignal();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    QPixmap img;
    QLabel *logo;
    QPushButton *btnShow;
};

#endif // HOMEWIDGET_H
