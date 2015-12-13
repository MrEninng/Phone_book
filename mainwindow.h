#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include <QSystemTrayIcon> // ???
//#include <QIcon>
//#include <QDebug>
#include <QtSql>
#include <gender_diagram.h>
#include "changeinfobutton_window.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void is_actionExit_triggered();
    void is_actionAbout_triggered();
    void is_actionAboutQt_triggered();
    void is_actionSaveAsTXT_triggered();
    void is_actionOpenAsTXT_triggered();


    void is_actionGenderDiagram_triggered();


    void is_findBox_clicked();

    //кнопки
    void is_addButton_clicked();
    void is_deleteButton_clicked();
    void is_changeInfoButton_clicked();
    void is_infoButton_clicked();
    void is_findButton_clicked();

    void change_numberLine_background();
    void change_nameLine_background();
    void change_secondNameLine_background();
    void change_surnameLine_background();
    void change_addressLine_background();



private:

    Ui::MainWindow *ui;
    QIcon *icon;
    changeInfoButton_window* change_widget;
    QSqlQueryModel queryModel;
  //  QSqlTableModel tableModel;
    unsigned int id = 0;

    bool addValidFlag = true;

    gender_diagram* gender_diagram_widget;

    static bool create_connection();

    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
