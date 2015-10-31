#ifndef CHANGEINFOBUTTON_WINDOW_H
#define CHANGEINFOBUTTON_WINDOW_H

#include <QDialog>
#include <QIcon>
#include <QtSql>

namespace Ui {
class changeInfoButton_window;
}

class changeInfoButton_window : public QDialog
{
    Q_OBJECT

public:
    explicit changeInfoButton_window(QWidget *parent = 0);
    ~changeInfoButton_window();


private slots:
    void is_submitButton_clicked();

private:
    Ui::changeInfoButton_window *ui;
    QIcon* icon;
    QSqlQueryModel queryModel;

};

#endif // CHANGEINFOBUTTON_WINDOW_H
