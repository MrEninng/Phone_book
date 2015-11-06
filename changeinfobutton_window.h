#ifndef CHANGEINFOBUTTON_WINDOW_H
#define CHANGEINFOBUTTON_WINDOW_H

#include <QDialog>
#include <QIcon>
#include <QtSql>
#include <QDate>
#include <QTableView>

struct person_information {
    QString number;
    QString name;
    QString second_name;
    QString surname;
    QString address;
    QString gender;
    QString date;
};

namespace Ui {
class changeInfoButton_window;
}

class changeInfoButton_window : public QDialog
{
    Q_OBJECT

public:
    explicit changeInfoButton_window(int id, QTableView* , QWidget *parent = 0);
    ~changeInfoButton_window();


private slots:
    void is_submitButton_clicked();

    void change_numberLine_background();
    void change_nameLine_background();
    void change_secondNameLine_background();
    void change_surnameLine_background();
    void change_addressLine_background();

private:
    Ui::changeInfoButton_window *ui;
    QIcon* icon;
    QString started_number;
    bool addValidFlag = true;
    int current_id = -1;
    QSqlQueryModel queryModel;
    QTableView *tableView;


    void closeEvent(QCloseEvent *);

};

#endif // CHANGEINFOBUTTON_WINDOW_H
